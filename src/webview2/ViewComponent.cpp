#include "stdafx.h"
#include "ViewComponent.h"
#include "DCompTargetImpl.h"
#include "DropTarget.h"
#include <d2d1helper.h>
#include <sstream>
#include <windows.ui.composition.interop.h>
#include <windowsx.h>
#include "DpiUtil.h"
#include "QWebViewWebView2Impl.h"

using namespace Microsoft::WRL;
namespace numerics = winrt::Windows::Foundation::Numerics;
static D2D1_MATRIX_4X4_F Convert3x2MatrixTo4x4Matrix(D2D1_MATRIX_3X2_F* matrix3x2);

ViewComponent::ViewComponent(
    QWebViewWebView2Impl* d,
    IDCompositionDevice* dcompDevice,
    winrtComp::Compositor wincompCompositor,
    bool isDcompTargetMode) :
    ComponentBase(d),
    m_dcompDevice(dcompDevice),
    m_wincompCompositor(wincompCompositor),
    m_isDcompTargetMode(isDcompTargetMode) {
  m_webView = d_->GetWebView();
  m_controller = d_->GetController();

  // Register a handler for the ZoomFactorChanged event.
  // This handler just announces the new level of zoom on the window's title bar.
  CHECK_FAILURE(m_controller->add_ZoomFactorChanged(
      Callback<ICoreWebView2ZoomFactorChangedEventHandler>(
          [this](ICoreWebView2Controller* sender, IUnknown* args) -> HRESULT {
            //double zoomFactor;
            //CHECK_FAILURE(sender->get_ZoomFactor(&zoomFactor));

            return S_OK;
          })
          .Get(),
      &m_zoomFactorChangedToken));

  CHECK_FAILURE(m_webView->add_NavigationStarting(
      Callback<ICoreWebView2NavigationStartingEventHandler>(
          [this](ICoreWebView2* sender, ICoreWebView2NavigationStartingEventArgs* args) -> HRESULT {
            //wil::unique_cotaskmem_string newUri;
            //CHECK_FAILURE(args->get_Uri(&newUri));

            //wil::unique_cotaskmem_string oldUri;
            //CHECK_FAILURE(m_webView->get_Source(&oldUri));

            return S_OK;
          })
          .Get(),
      &m_navigationStartingToken));

  CHECK_FAILURE(m_webView->add_DocumentTitleChanged(
      Callback<ICoreWebView2DocumentTitleChangedEventHandler>(
          [this](ICoreWebView2* sender, IUnknown* args) -> HRESULT {
            wil::unique_cotaskmem_string title;
            CHECK_FAILURE(sender->get_DocumentTitle(&title));

            emit d_->GetHostWidget()->titleChanged(QString::fromStdWString(title.get()));

            return S_OK;
          })
          .Get(),
      &m_documentTitleChangedToken));

  m_controller3 = m_controller.try_query<ICoreWebView2Controller3>();
  if (m_controller3) {
    //! [RasterizationScaleChanged]
    CHECK_FAILURE(m_controller3->add_RasterizationScaleChanged(
        Callback<ICoreWebView2RasterizationScaleChangedEventHandler>(
            [this](ICoreWebView2Controller* sender, IUnknown* args) -> HRESULT {
              //double rasterizationScale;
              //CHECK_FAILURE(m_controller3->get_RasterizationScale(&rasterizationScale));

              return S_OK;
            })
            .Get(),
        &m_rasterizationScaleChangedToken));
    //! [RasterizationScaleChanged]
  }

  m_webView2_9 = m_webView.try_query<ICoreWebView2_9>();
  if (m_webView2_9) {
  }

  ResizeWebView();
  UpdateDpiAndTextScale();
}

bool ViewComponent::HandleWindowMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* result) {
  if (message == WM_SIZE) {
    if (wParam == SIZE_MINIMIZED) {
      // Hide the webview when the app window is minimized.
      m_controller->put_IsVisible(FALSE);
      Suspend();
    }
    else if (wParam == SIZE_RESTORED) {
      // When the app window is restored, show the webview (unless the user has toggle visibility off).
      if (m_isVisible) {
        Resume();
        m_controller->put_IsVisible(TRUE);
      }
    }
  }

  if ((message >= WM_MOUSEFIRST && message <= WM_MOUSELAST) || (message == WM_NCRBUTTONUP || message == WM_NCRBUTTONDOWN) || message == WM_MOUSELEAVE) {
    return OnMouseMessage(message, wParam, lParam);
  }

  if (
      message == WM_POINTERACTIVATE || message == WM_POINTERDOWN ||
      message == WM_POINTERENTER || message == WM_POINTERLEAVE || message == WM_POINTERUP ||
      message == WM_POINTERUPDATE) {
    return OnPointerMessage(message, wParam, lParam);
  }

  if (message == WM_MOVE || message == WM_MOVING) {
    m_controller->NotifyParentWindowPositionChanged();
    return true;
  }

  return false;
}

void ViewComponent::SetPreferredColorScheme(COREWEBVIEW2_PREFERRED_COLOR_SCHEME value) {
  wil::com_ptr<ICoreWebView2_13> webView2_13;
  webView2_13 = m_webView.try_query<ICoreWebView2_13>();

  if (webView2_13) {
    wil::com_ptr<ICoreWebView2Profile> profile;
    CHECK_FAILURE(webView2_13->get_Profile(&profile));
    profile->put_PreferredColorScheme(value);
  }
}

void ViewComponent::UpdateDpiAndTextScale() {
  if (m_controller3) {
    BOOL isWebViewDetectingScaleChanges;
    CHECK_FAILURE(m_controller3->get_ShouldDetectMonitorScaleChanges(&isWebViewDetectingScaleChanges));
    if (!isWebViewDetectingScaleChanges) {
      SetRasterizationScale(m_webviewAdditionalRasterizationScale);
    }
  }
}

void ViewComponent::ToggleVisibility() {
  BOOL visible;
  m_controller->get_IsVisible(&visible);
  m_isVisible = !visible;
  m_controller->put_IsVisible(m_isVisible);
}

void ViewComponent::Suspend() {
  wil::com_ptr<ICoreWebView2_3> webView;
  webView = m_webView.try_query<ICoreWebView2_3>();
  CHECK_FEATURE_RETURN_EMPTY(webView);
  HRESULT hr = webView->TrySuspend(
      Callback<ICoreWebView2TrySuspendCompletedHandler>(
          [this](HRESULT errorCode, BOOL isSuccessful) -> HRESULT {
            if ((errorCode != S_OK) || !isSuccessful) {
              ShowFailure(errorCode, L"Suspend failed");
            }
            return S_OK;
          })
          .Get());

  if (FAILED(hr))
    ShowFailure(hr, L"Call to TrySuspend failed");
}

void ViewComponent::ToggleMemoryUsageTargetLevel() {
  wil::com_ptr<ICoreWebView2_19> webView;
  webView = m_webView.try_query<ICoreWebView2_19>();
  CHECK_FEATURE_RETURN_EMPTY(webView);

  COREWEBVIEW2_MEMORY_USAGE_TARGET_LEVEL memory_target_level = COREWEBVIEW2_MEMORY_USAGE_TARGET_LEVEL_NORMAL;
  CHECK_FAILURE(webView->get_MemoryUsageTargetLevel(&memory_target_level));

  memory_target_level = (memory_target_level == COREWEBVIEW2_MEMORY_USAGE_TARGET_LEVEL_LOW)
                            ? COREWEBVIEW2_MEMORY_USAGE_TARGET_LEVEL_NORMAL
                            : COREWEBVIEW2_MEMORY_USAGE_TARGET_LEVEL_LOW;
  CHECK_FAILURE(webView->put_MemoryUsageTargetLevel(memory_target_level));
  MessageBox(
      nullptr,
      (memory_target_level == COREWEBVIEW2_MEMORY_USAGE_TARGET_LEVEL_LOW)
          ? L"MemoryUsageTargetLevel is set to LOW."
          : L"MemoryUsageTargetLevel is set to NORMAL.",
      L"MemoryUsageTargetLevel change", MB_OK);
}

void ViewComponent::Resume() {
  wil::com_ptr<ICoreWebView2_3> webView;
  webView = m_webView.try_query<ICoreWebView2_3>();
  CHECK_FEATURE_RETURN_EMPTY(webView);
  webView->Resume();
}

void ViewComponent::SetBackgroundColor(COLORREF color, bool transparent) {
  m_webViewColor.R = GetRValue(color);
  m_webViewColor.G = GetGValue(color);
  m_webViewColor.B = GetBValue(color);
  m_webViewColor.A = transparent ? 0 : 255;
  wil::com_ptr<ICoreWebView2Controller2> controller2 = m_controller.query<ICoreWebView2Controller2>();
  controller2->put_DefaultBackgroundColor(m_webViewColor);
}

void ViewComponent::SetSizeRatio(float ratio) {
  m_webViewRatio = ratio;
  ResizeWebView();
}

void ViewComponent::SetZoomFactor(float zoom) {
  m_webViewZoomFactor = zoom;
  m_controller->put_ZoomFactor(zoom);
}

void ViewComponent::SetBounds(RECT bounds) {
  m_webViewBounds = bounds;
  ResizeWebView();
}

RECT ViewComponent::GetBounds() {
  return m_webViewBounds;
}

void ViewComponent::OffsetPointToWebView(LPPOINT point) {
  ::ScreenToClient(d_->GetWindowHwnd(), point);
  point->x -= m_webViewBounds.left;
  point->y -= m_webViewBounds.top;
}

void ViewComponent::SetScale(float scale) {
  RECT bounds;
  CHECK_FAILURE(m_controller->get_Bounds(&bounds));
  double scaleChange = scale / m_webViewScale;

  bounds.bottom = LONG((bounds.bottom - bounds.top) * scaleChange + bounds.top);
  bounds.right = LONG((bounds.right - bounds.left) * scaleChange + bounds.left);

  m_webViewScale = scale;
  m_controller->SetBoundsAndZoomFactor(bounds, scale);
}

// Update the bounds of the WebView window to fit available space.
void ViewComponent::ResizeWebView() {
  SIZE webViewSize = {
      LONG((m_webViewBounds.right - m_webViewBounds.left) * m_webViewRatio * m_webViewScale),
      LONG((m_webViewBounds.bottom - m_webViewBounds.top) * m_webViewRatio * m_webViewScale)};

  RECT desiredBounds = m_webViewBounds;
  desiredBounds.bottom = LONG(webViewSize.cy + m_webViewBounds.top);
  desiredBounds.right = LONG(webViewSize.cx + m_webViewBounds.left);

  m_controller->put_Bounds(desiredBounds);
  if (m_compositionController) {
    POINT webViewOffset = {m_webViewBounds.left, m_webViewBounds.top};

    if (m_dcompDevice) {
      CHECK_FAILURE(m_dcompRootVisual->SetOffsetX(float(webViewOffset.x)));
      CHECK_FAILURE(m_dcompRootVisual->SetOffsetY(float(webViewOffset.y)));
      CHECK_FAILURE(m_dcompRootVisual->SetClip({0, 0, float(webViewSize.cx), float(webViewSize.cy)}));
      CHECK_FAILURE(m_dcompDevice->Commit());
    }
    else if (m_wincompCompositor) {
      if (m_wincompRootVisual != nullptr) {
        numerics::float2 size = {static_cast<float>(webViewSize.cx), static_cast<float>(webViewSize.cy)};
        m_wincompRootVisual.Size(size);

        numerics::float3 offset = {static_cast<float>(webViewOffset.x), static_cast<float>(webViewOffset.y), 0.0f};
        m_wincompRootVisual.Offset(offset);

        winrtComp::IInsetClip insetClip = m_wincompCompositor.CreateInsetClip();
        m_wincompRootVisual.Clip(insetClip.as<winrtComp::CompositionClip>());
      }
    }
  }
}

// Show the current bounds of the WebView.
void ViewComponent::ShowWebViewBounds() {
  RECT bounds;
  HRESULT result = m_controller->get_Bounds(&bounds);
  if (SUCCEEDED(result)) {
    std::wstringstream message;
    message << L"Left:\t" << bounds.left << L"\n"
            << L"Top:\t" << bounds.top << L"\n"
            << L"Right:\t" << bounds.right << L"\n"
            << L"Bottom:\t" << bounds.bottom << std::endl;
    MessageBox(nullptr, message.str().c_str(), L"WebView Bounds", MB_OK);
  }
}

// Show the current zoom factor of the WebView.
void ViewComponent::ShowWebViewZoom() {
  double zoomFactor;
  HRESULT result = m_controller->get_ZoomFactor(&zoomFactor);
  if (SUCCEEDED(result)) {
    std::wstringstream message;
    message << L"Zoom Factor:\t" << zoomFactor << std::endl;
    MessageBox(nullptr, message.str().c_str(), L"WebView Zoom Factor", MB_OK);
  }
}

void ViewComponent::SetTransform(TransformType transformType) {
  if (!m_compositionController) {
    MessageBox(
        nullptr,
        L"Setting transform is not supported in windowed mode."
        "Choose a windowless mode for creation before trying to apply a transform.",
        L"Applying transform failed.", MB_OK);
    return;
  }

  if (transformType == TransformType::kRotate30Deg) {
    D2D1_POINT_2F center = D2D1::Point2F(
        (m_webViewBounds.right - m_webViewBounds.left) / 2.f,
        (m_webViewBounds.bottom - m_webViewBounds.top) / 2.f);
    D2D1::Matrix3x2F rotated = D2D1::Matrix3x2F::Rotation(30.0f, center);
    m_webViewTransformMatrix = Convert3x2MatrixTo4x4Matrix(&rotated);
  }
  else if (transformType == TransformType::kRotate60DegDiagonally) {
    m_webViewTransformMatrix = D2D1::Matrix4x4F::RotationArbitraryAxis(float(m_webViewBounds.right), float(m_webViewBounds.bottom), 0, 60);
  }
  else if (transformType == TransformType::kIdentity) {
    m_webViewTransformMatrix = D2D1::Matrix4x4F();
  }

  if (m_dcompDevice && !m_wincompCompositor) {
    wil::com_ptr<IDCompositionVisual3> dcompWebViewVisual3;
    m_dcompWebViewVisual->QueryInterface(IID_PPV_ARGS(&dcompWebViewVisual3));
    CHECK_FAILURE(dcompWebViewVisual3->SetTransform(m_webViewTransformMatrix));
    CHECK_FAILURE(m_dcompDevice->Commit());
  }
  else if (m_wincompCompositor && !m_dcompDevice) {
    if (m_wincompWebViewVisual != nullptr) {
      m_wincompWebViewVisual.TransformMatrix(*reinterpret_cast<numerics::float4x4*>(&m_webViewTransformMatrix));
    }
  }
  else {
    FAIL_FAST();
  }
}

void ViewComponent::SetRasterizationScale(float additionalScale) {
  if (m_controller3) {
    CHECK_FAILURE(m_controller3->put_ShouldDetectMonitorScaleChanges(FALSE));
    m_webviewAdditionalRasterizationScale = additionalScale;
    double rasterizationScale = additionalScale * DpiUtil::GetDpiScaleForWindow(d_->GetWindowHwnd()) * d_->GetTextScale();
    CHECK_FAILURE(m_controller3->put_RasterizationScale(rasterizationScale));
  }
}

void ViewComponent::SetBoundsMode(COREWEBVIEW2_BOUNDS_MODE boundsMode) {
  if (m_controller3) {
    m_boundsMode = boundsMode;
    CHECK_FAILURE(m_controller3->put_BoundsMode(boundsMode));
    ResizeWebView();
  }
}

static D2D1_MATRIX_4X4_F Convert3x2MatrixTo4x4Matrix(D2D1_MATRIX_3X2_F* matrix3x2) {
  D2D1_MATRIX_4X4_F matrix4x4 = D2D1::Matrix4x4F();
  matrix4x4._11 = matrix3x2->m11;
  matrix4x4._12 = matrix3x2->m12;
  matrix4x4._21 = matrix3x2->m21;
  matrix4x4._22 = matrix3x2->m22;
  matrix4x4._41 = matrix3x2->dx;
  matrix4x4._42 = matrix3x2->dy;
  return matrix4x4;
}

bool ViewComponent::OnMouseMessage(UINT message, WPARAM wParam, LPARAM lParam) {
  // Manually relay mouse messages to the WebView
  if (m_dcompDevice || m_wincompCompositor) {
    POINT point;
    POINTSTOPOINT(point, lParam);
    if (message == WM_MOUSEWHEEL ||
        message == WM_MOUSEHWHEEL || message == WM_NCRBUTTONDOWN || message == WM_NCRBUTTONUP) {
      // Mouse wheel messages are delivered in screen coordinates.
      // SendMouseInput expects client coordinates for the WebView, so convert
      // the point from screen to client.
      ::ScreenToClient(d_->GetWindowHwnd(), &point);
    }
    // Send the message to the WebView if the mouse location is inside the
    // bounds of the WebView, if the message is telling the WebView the
    // mouse has left the client area, or if we are currently capturing
    // mouse events.
    bool isMouseInWebView = PtInRect(&m_webViewBounds, point);
    if (isMouseInWebView || message == WM_MOUSELEAVE || m_isCapturingMouse) {
      DWORD mouseData = 0;

      switch (message) {
        case WM_MOUSEWHEEL:
        case WM_MOUSEHWHEEL:
          mouseData = GET_WHEEL_DELTA_WPARAM(wParam);
          break;
        case WM_XBUTTONDBLCLK:
        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP:
          mouseData = GET_XBUTTON_WPARAM(wParam);
          break;
        case WM_MOUSEMOVE:
          if (!m_isTrackingMouse) {
            // WebView needs to know when the mouse leaves the client area
            // so that it can dismiss hover popups. TrackMouseEvent will
            // provide a notification when the mouse leaves the client area.
            TrackMouseEvents(TME_LEAVE);
            m_isTrackingMouse = true;
          }
          break;
        case WM_MOUSELEAVE:
          m_isTrackingMouse = false;
          break;
      }

      // We need to capture the mouse in case the user drags the
      // mouse outside of the window bounds and we still need to send
      // mouse messages to the WebView process. This is useful for
      // scenarios like dragging the scroll bar or panning a map.
      // This is very similar to the Pointer Message case where a
      // press started inside of the WebView.
      if (message == WM_LBUTTONDOWN || message == WM_MBUTTONDOWN ||
          message == WM_RBUTTONDOWN || message == WM_XBUTTONDOWN) {
        if (isMouseInWebView && ::GetCapture() != d_->GetWindowHwnd()) {
          m_isCapturingMouse = true;
          ::SetCapture(d_->GetWindowHwnd());
        }
      }
      else if (message == WM_LBUTTONUP || message == WM_MBUTTONUP ||
               message == WM_RBUTTONUP || message == WM_XBUTTONUP) {
        if (::GetCapture() == d_->GetWindowHwnd()) {
          m_isCapturingMouse = false;
          ::ReleaseCapture();
        }
      }

      // Adjust the point from app client coordinates to webview client coordinates.
      // WM_MOUSELEAVE messages don't have a point, so don't adjust the point.
      if (message != WM_MOUSELEAVE) {
        point.x -= m_webViewBounds.left;
        point.y -= m_webViewBounds.top;
      }

      CHECK_FAILURE(m_compositionController->SendMouseInput(
          static_cast<COREWEBVIEW2_MOUSE_EVENT_KIND>(message),
          static_cast<COREWEBVIEW2_MOUSE_EVENT_VIRTUAL_KEYS>(GET_KEYSTATE_WPARAM(wParam)),
          mouseData, point));
      return true;
    }
    else if (message == WM_MOUSEMOVE && m_isTrackingMouse) {
      // When the mouse moves outside of the WebView, but still inside the app
      // turn off mouse tracking and send the WebView a leave event.
      m_isTrackingMouse = false;
      TrackMouseEvents(TME_LEAVE | TME_CANCEL);
      OnMouseMessage(WM_MOUSELEAVE, 0, 0);
    }
  }
  return false;
}

bool ViewComponent::OnPointerMessage(UINT message, WPARAM wParam, LPARAM lParam) {
  bool handled = false;
  if (m_dcompDevice || m_wincompCompositor) {
    POINT point;
    POINTSTOPOINT(point, lParam);
    UINT pointerId = GET_POINTERID_WPARAM(wParam);

    ::ScreenToClient(d_->GetWindowHwnd(), &point);

    bool pointerStartedInWebView = m_pointerIdsStartingInWebView.find(pointerId) != m_pointerIdsStartingInWebView.end();
    // We want to send pointer input to the WebView for all pointers that either is in the
    // WebView or started inside the WebView. For example, if a user started a page scroll
    // inside of the WebView but dragged their finger outside of the WebView, we need to
    // keep sending pointer events for those pointers.
    if (PtInRect(&m_webViewBounds, point) || pointerStartedInWebView) {
      if (!pointerStartedInWebView &&
          (message == WM_POINTERENTER || message == WM_POINTERDOWN)) {
        m_pointerIdsStartingInWebView.insert(pointerId);
      }
      else if (message == WM_POINTERLEAVE) {
        m_pointerIdsStartingInWebView.erase(pointerId);
      }

      handled = true;
      // Add the current offset before creating the CoreWebView2PointerInfo
      D2D1_MATRIX_4X4_F m_webViewInputTransformMatrix = m_webViewTransformMatrix;
      m_webViewInputTransformMatrix._41 += m_webViewBounds.left;
      m_webViewInputTransformMatrix._42 += m_webViewBounds.top;
      wil::com_ptr<ICoreWebView2PointerInfo> pointer_info;
      // TODO
      CHECK_FAILURE(m_compositionController->SendPointerInput(static_cast<COREWEBVIEW2_POINTER_EVENT_KIND>(message), pointer_info.get()));
    }
  }
  return handled;
}

void ViewComponent::TrackMouseEvents(DWORD mouseTrackingFlags) {
  TRACKMOUSEEVENT tme;
  tme.cbSize = sizeof(tme);
  tme.dwFlags = mouseTrackingFlags;
  tme.hwndTrack = d_->GetWindowHwnd();
  tme.dwHoverTime = 0;
  ::TrackMouseEvent(&tme);
}

//! [BuildDCompTree]
// Create host app visual that the WebView will connect to.
//   - Create a IDCompositionTarget for the host window
//   - Create a visual and set that as the IDCompositionTarget's root
//   - Create another visual and add that to the IDCompositionTarget's root.
//     This visual will be the visual root for the WebView.
void ViewComponent::BuildDCompTreeUsingVisual() {
  CHECK_FAILURE_BOOL(m_dcompDevice != nullptr);

  if (m_dcompWebViewVisual == nullptr) {
    CHECK_FAILURE(m_dcompDevice->CreateTargetForHwnd(d_->GetWindowHwnd(), TRUE, &m_dcompHwndTarget));
    CHECK_FAILURE(m_dcompDevice->CreateVisual(&m_dcompRootVisual));
    CHECK_FAILURE(m_dcompHwndTarget->SetRoot(m_dcompRootVisual.get()));
    CHECK_FAILURE(m_dcompDevice->CreateVisual(&m_dcompWebViewVisual));
    CHECK_FAILURE(m_dcompRootVisual->AddVisual(m_dcompWebViewVisual.get(), TRUE, nullptr));
  }
}
//! [BuildDCompTree]

void ViewComponent::DestroyDCompVisualTree() {
  if (m_dcompWebViewVisual) {
    m_dcompWebViewVisual->RemoveAllVisuals();
    m_dcompWebViewVisual.reset();

    m_dcompRootVisual->RemoveAllVisuals();
    m_dcompRootVisual.reset();

    m_dcompHwndTarget->SetRoot(nullptr);
    m_dcompHwndTarget.reset();

    m_dcompDevice->Commit();
  }

  if (m_dcompTarget) {
    m_dcompTarget->RemoveOwnerRef();
    m_dcompTarget = nullptr;
  }
}

void ViewComponent::BuildWinCompVisualTree() {
  namespace abiComp = ABI::Windows::UI::Composition;

  if (m_wincompWebViewVisual == nullptr) {
    auto interop = m_wincompCompositor.as<abiComp::Desktop::ICompositorDesktopInterop>();
    winrt::check_hresult(interop->CreateDesktopWindowTarget(
        d_->GetWindowHwnd(),
        false,
        reinterpret_cast<abiComp::Desktop::IDesktopWindowTarget**>(winrt::put_abi(m_wincompHwndTarget))));

    m_wincompRootVisual = m_wincompCompositor.CreateContainerVisual();
    m_wincompHwndTarget.Root(m_wincompRootVisual);

    m_wincompWebViewVisual = m_wincompCompositor.CreateContainerVisual();
    m_wincompRootVisual.Children().InsertAtTop(m_wincompWebViewVisual);
  }
}

void ViewComponent::DestroyWinCompVisualTree() {
  if (m_wincompWebViewVisual != nullptr) {
    m_wincompWebViewVisual.Children().RemoveAll();
    m_wincompWebViewVisual = nullptr;

    m_wincompRootVisual.Children().RemoveAll();
    m_wincompRootVisual = nullptr;

    m_wincompHwndTarget.Root(nullptr);
    m_wincompHwndTarget = nullptr;
  }
}

void ViewComponent::ToggleDefaultDownloadDialog() {
  if (m_webView2_9) {
    BOOL isOpen;
    m_webView2_9->get_IsDefaultDownloadDialogOpen(&isOpen);
    if (isOpen) {
      m_webView2_9->CloseDefaultDownloadDialog();
    }
    else {
      m_webView2_9->OpenDefaultDownloadDialog();
    }
  }
}

ViewComponent::~ViewComponent() {
  m_webView->remove_NavigationStarting(m_navigationStartingToken);
  m_webView->remove_DocumentTitleChanged(m_documentTitleChangedToken);
  m_controller->remove_ZoomFactorChanged(m_zoomFactorChangedToken);
  if (m_controller3) {
    m_controller3->remove_RasterizationScaleChanged(m_rasterizationScaleChangedToken);
  }
  if (m_dropTarget) {
    RevokeDragDrop(d_->GetWindowHwnd());
    m_dropTarget = nullptr;
  }
  if (m_compositionController) {
    m_compositionController->remove_CursorChanged(m_cursorChangedToken);
    // If the webview closes because the AppWindow is closed (as opposed to being closed
    // explicitly), this will no-op because in this case, the webview closes before the ViewComponent
    // is destroyed. If the webview is closed explicitly, this will succeed.
    m_compositionController->put_RootVisualTarget(nullptr);
    DestroyDCompVisualTree();
    DestroyWinCompVisualTree();
  }
}
