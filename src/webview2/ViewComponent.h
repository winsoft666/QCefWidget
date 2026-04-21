#ifndef VIEW_COMPONENT_H_
#define VIEW_COMPONENT_H_
#if defined(_MSVC_) && _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include "ComponentBase.h"
#include <dcomp.h>
#include <unordered_set>
#include <winrt/Windows.UI.Composition.Desktop.h>
#include <winrt/Windows.UI.Composition.h>
#include "WebView2.h"

// This component handles commands from the View menu, as well as the ZoomFactorChanged
// event, and any functionality related to sizing and visibility of the WebView.
// It also manages interaction with the compositor if running in windowless mode.

class DCompTargetImpl;
class DropTarget;

namespace winrtComp = winrt::Windows::UI::Composition;

class ViewComponent : public ComponentBase {
  friend class DCompTargetImpl;

 public:
  enum class TransformType {
    kIdentity = 0,
    kScale2X,
    kRotate30Deg,
    kRotate60DegDiagonally
  };

  ViewComponent(QWebViewWebView2Impl* d, IDCompositionDevice* dcompDevice, winrtComp::Compositor wincompCompositor, bool isDCompTargetMode);

  bool HandleWindowMessage(
      HWND hWnd,
      UINT message,
      WPARAM wParam,
      LPARAM lParam,
      LRESULT* result) override;

  void SetBounds(RECT bounds);
  RECT GetBounds();

  // Converts a screen point to a WebView client point while taking into account WebView's offset.
  void OffsetPointToWebView(LPPOINT point);

  void UpdateDpiAndTextScale();

  void SetPreferredColorScheme(COREWEBVIEW2_PREFERRED_COLOR_SCHEME value);

  void ResizeWebView();
  void ToggleVisibility();
  void Suspend();
  void Resume();
  void ToggleMemoryUsageTargetLevel();
  void SetBackgroundColor(COLORREF color, bool transparent);
  void SetSizeRatio(float ratio);
  void SetZoomFactor(float zoom);
  void SetScale(float scale);
  void SetTransform(TransformType transformType);
  void SetRasterizationScale(float additionalScale);
  void SetBoundsMode(COREWEBVIEW2_BOUNDS_MODE boundsMode);
  void ShowWebViewBounds();
  void ShowWebViewZoom();
  void ToggleDefaultDownloadDialog();

  ~ViewComponent() override;

 private:
  wil::com_ptr<ICoreWebView2Controller> m_controller;
  wil::com_ptr<ICoreWebView2Controller3> m_controller3;
  wil::com_ptr<ICoreWebView2> m_webView;
  wil::com_ptr<ICoreWebView2_9> m_webView2_9;

  bool m_isDcompTargetMode;
  bool m_isVisible = true;
  float m_webViewRatio = 1.0f;
  float m_webViewZoomFactor = 1.0f;
  RECT m_webViewBounds = {};
  float m_webViewScale = 1.0f;
  bool m_useCursorId = false;
  wil::com_ptr<DropTarget> m_dropTarget;
  float m_webviewAdditionalRasterizationScale = 1.0f;
  COREWEBVIEW2_BOUNDS_MODE m_boundsMode = COREWEBVIEW2_BOUNDS_MODE_USE_RAW_PIXELS;
  COREWEBVIEW2_COLOR m_webViewColor = {255, 255, 255, 255};

  EventRegistrationToken m_zoomFactorChangedToken = {};
  EventRegistrationToken m_rasterizationScaleChangedToken = {};
  EventRegistrationToken m_navigationStartingToken = {};

  bool OnMouseMessage(UINT message, WPARAM wParam, LPARAM lParam);
  bool OnPointerMessage(UINT message, WPARAM wParam, LPARAM lParam);
  void TrackMouseEvents(DWORD mouseTrackingFlags);

  wil::com_ptr<ICoreWebView2CompositionController> m_compositionController;
  bool m_isTrackingMouse = false;
  bool m_isCapturingMouse = false;
  std::unordered_set<UINT> m_pointerIdsStartingInWebView;
  D2D1_MATRIX_4X4_F m_webViewTransformMatrix = D2D1::Matrix4x4F();

  void BuildDCompTreeUsingVisual();
  void DestroyDCompVisualTree();

  wil::com_ptr<IDCompositionDevice> m_dcompDevice;
  wil::com_ptr<IDCompositionTarget> m_dcompHwndTarget;
  wil::com_ptr<IDCompositionVisual> m_dcompRootVisual;
  wil::com_ptr<IDCompositionVisual> m_dcompWebViewVisual;

  void BuildWinCompVisualTree();
  void DestroyWinCompVisualTree();

  winrt::Windows::UI::Composition::Compositor m_wincompCompositor{nullptr};
  winrt::Windows::UI::Composition::Desktop::DesktopWindowTarget m_wincompHwndTarget{nullptr};
  winrt::Windows::UI::Composition::ContainerVisual m_wincompRootVisual{nullptr};
  winrt::Windows::UI::Composition::ContainerVisual m_wincompWebViewVisual{nullptr};

  // This member is used to exercise the put_RootVisualTarget API with an IDCompositionTarget.
  // Distinct/unrelated to the dcompHwndTarget
  wil::com_ptr<DCompTargetImpl> m_dcompTarget;

  EventRegistrationToken m_cursorChangedToken = {};
  EventRegistrationToken m_documentTitleChangedToken = {};
};
#endif  // !VIEW_COMPONENT_H_
