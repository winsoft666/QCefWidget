#ifndef QWEBVIEW2_IMPL_H_
#define QWEBVIEW2_IMPL_H_
#if defined(_MSVC_) && _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include "QWebView.WebView2.h"
#include "CheckFailure.h"
#include "ViewComponent.h"
#include "ScriptComponent.h"
#include "AudioComponent.h"
#include "ControlComponent.h"
#include "SettingsComponent.h"
#include "ProcessComponent.h"
#include "HostObjectComponent.h"
#include "NonClientRegionSupportComponent.h"
#include "CookieManagementComponent.h"

namespace winrtComp = winrt::Windows::UI::Composition;

#define WUM_ASYNC_TASK (WM_USER + 1989)

class QWebViewWebView2Impl {
 public:
  QWebViewWebView2Impl(QWebViewWebView2* host);
  QWebViewWebView2Impl(const WebView2Options& opts, QWebViewWebView2* host);
  virtual ~QWebViewWebView2Impl();

  // Creates and registers a component
  template <class ComponentType, class... Args>
  void NewComponent(Args&&... args) {
    m_components.emplace_back(new ComponentType(std::forward<Args>(args)...));
  }

  template <class ComponentType>
  ComponentType* GetComponent() {
    for (auto& component : m_components) {
      if (auto wanted = dynamic_cast<ComponentType*>(component.get())) {
        return wanted;
      }
    }
    return nullptr;
  }

  template <class ComponentType>
  std::unique_ptr<ComponentType> MoveComponent();

  void DeleteComponent(ComponentBase* component);
  void DeleteAllComponents();

  HRESULT ReinitializeWebView();

  bool IsInitialized() const {
    return initialized_;
  }

  HRESULT InitializeWebView();
  void UninitializeWebView();

  HWND GetWindowHwnd();
  ICoreWebView2* GetWebView();
  ICoreWebView2Controller* GetController();
  ICoreWebView2Environment* GetEnvironment();
  double GetTextScale();

  WebView2Options GetOptions() const;
  WebView2Options& GetOptionsRef();

  bool ComponentsHandleWindowMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* result);

  void ResizeEverything(UINT width, UINT height);

  void OpenDevToolsWindow();

  void Navigate(const std::wstring& url);
  std::wstring GetUrl() const;

  void Reload();
  void StopLoading();

  bool CanGoBack();
  void GoBack();

  bool CanGoForward();
  void GoForward();

  bool IsMute();
  void SetMute(bool mute);

  void RegisterSciptCallNotify(ScriptCallCallback cb);
  ScriptCallCallback GetSciptCallNotify() const;

  HRESULT PostStringWebMessage(const std::wstring& str);
  HRESULT PostJSONWebMessage(const std::wstring& strJSON);
  HRESULT ExecuteScriptWithResult(const std::wstring& s, ScriptResultCallback cb);

  QMap<QString, QString> virtualHostNameAndFolder() const;
  void SetVirtualHostNameAndFolder(const QMap<QString, QString>& m);

  void RunAsync(std::function<void()> callback) {
    auto* task = new std::function<void()>(std::move(callback));
    PostMessage((HWND)host_->winId(), WUM_ASYNC_TASK, reinterpret_cast<WPARAM>(task), 0);
  }

  QWebViewWebView2* GetHostWidget();

 protected:
  HRESULT OnCreateEnvironmentCompleted(HRESULT result, ICoreWebView2Environment* environment);
  HRESULT OnCreateCoreWebView2ControllerCompleted(HRESULT result, ICoreWebView2Controller* controller);
  void RegisterEventHandlers();
  void OnTextScaleChanged(winrt::Windows::UI::ViewManagement::UISettings const& uiSettings,
                          winrt::Windows::Foundation::IInspectable const& args);

  std::wstring GetLocalPath(std::wstring relativePath, bool keep_exe_path);
  void CleanupUserDataFolder();
  HRESULT DeleteFileRecursive(std::wstring path);
  HRESULT DCompositionCreateDevice2(IUnknown* renderingDevice, REFIID riid, void** ppv);
  HRESULT TryCreateDispatcherQueue();

  void ApplyOptionsToViewComponent();
  void ApplyOptionsToSettingComponent();

  void DoSetVirtualHostNameAndFolder();

 private:
  bool initialized_ = false;
  QWebViewWebView2* host_ = nullptr;
  WebView2Options options_;
  ScriptCallCallback scriptCallCb_;
  QMap<QString, QString> virtualHostNameAndFolderMap_;

  EventRegistrationToken m_browserExitedEventToken = {};
  UINT32 m_newestBrowserPid = 0;
  std::wstring m_profileName;

  std::vector<std::unique_ptr<ComponentBase>> m_components;

  std::unique_ptr<SettingsComponent> m_oldSettingsComponent;

  // The following is state that belongs with the webview, and should be reinitialized along with it.
  // Everything here is undefined when m_webView is null.
  wil::com_ptr<ICoreWebView2Environment> m_webViewEnvironment;
  wil::com_ptr<ICoreWebView2Controller> m_controller;
  wil::com_ptr<ICoreWebView2> m_webView;
  wil::com_ptr<ICoreWebView2_3> m_webView3;

  wil::com_ptr<IDCompositionDevice> m_dcompDevice;
  winrtComp::Compositor m_wincompCompositor{nullptr};
  winrt::Windows::UI::ViewManagement::UISettings m_uiSettings{nullptr};
};

#endif  // !QWEBVIEW2_IMPL_H_
