#include "stdafx.h"
#include "QWebViewWebView2Impl.h"
#include <shellapi.h>
#include <ShObjIdl_core.h>
#include <DispatcherQueue.h>
#include <QDebug>
#include <WebView2EnvironmentOptions.h>

using namespace Microsoft::WRL;

QWebViewWebView2Impl::QWebViewWebView2Impl(QWebViewWebView2* host) :
    host_(host) {
  // Initialize COM as STA.
  CHECK_FAILURE(OleInitialize(NULL));
}

QWebViewWebView2Impl::QWebViewWebView2Impl(const WebView2Options& opts, QWebViewWebView2* host) :
    options_(opts),
    host_(host) {
  // Initialize COM as STA.
  CHECK_FAILURE(OleInitialize(NULL));
}

QWebViewWebView2Impl::~QWebViewWebView2Impl() {
}

void QWebViewWebView2Impl::DeleteComponent(ComponentBase* component) {
  for (auto iter = m_components.begin(); iter != m_components.end(); iter++) {
    if (iter->get() == component) {
      m_components.erase(iter);
      return;
    }
  }
}

void QWebViewWebView2Impl::DeleteAllComponents() {
  while (!m_components.empty()) {
    m_components.pop_back();
  }
}

template <class ComponentType>
std::unique_ptr<ComponentType> QWebViewWebView2Impl::MoveComponent() {
  for (auto iter = m_components.begin(); iter != m_components.end(); iter++) {
    if (dynamic_cast<ComponentType*>(iter->get())) {
      auto wanted = reinterpret_cast<std::unique_ptr<ComponentType>&&>(std::move(*iter));
      m_components.erase(iter);
      return std::move(wanted);
    }
  }
  return nullptr;
}

HRESULT QWebViewWebView2Impl::ReinitializeWebView() {
  // Save the settings component from being deleted when the WebView is closed, so we can
  // copy its properties to the next settings component.
  m_oldSettingsComponent = MoveComponent<SettingsComponent>();

  if (m_webView) {
    // We need to close the current webviews and wait for the browser_process to exit
    // This is so the new webviews don't use the old browser exe
    UninitializeWebView();
  }
  return InitializeWebView();
}

HRESULT QWebViewWebView2Impl::InitializeWebView() {
  if (initialized_)
    return S_OK;

  if (winrt::try_get_activation_factory<winrt::Windows::UI::ViewManagement::UISettings>()) {
    m_uiSettings = winrt::Windows::UI::ViewManagement::UISettings();
    m_uiSettings.TextScaleFactorChanged({this, &QWebViewWebView2Impl::OnTextScaleChanged});
  }

  if (options_.creationMode == CreationMode::VisualDComp || options_.creationMode == CreationMode::TargetDComp) {
    HRESULT hr = DCompositionCreateDevice2(nullptr, IID_PPV_ARGS(&m_dcompDevice));
    if (!SUCCEEDED(hr)) {
      ShowFailure(hr, L"Attempting to create WebView using DComp Visual is not supported. DComp device creation failed. Current OS may not support DComp.");
      return hr;
    }
  }
  else if (options_.creationMode == CreationMode::VisualWinComp) {
    HRESULT hr = TryCreateDispatcherQueue();
    if (!SUCCEEDED(hr)) {
      ShowFailure(hr, L"Attempting to create WebView using WinComp Visual is not supported. WinComp compositor creation failed. Current OS may not support WinComp.");
      return hr;
    }
    m_wincompCompositor = winrtComp::Compositor();
  }

  //! [CreateCoreWebView2EnvironmentWithOptions]
  auto envOptions = Microsoft::WRL::Make<CoreWebView2EnvironmentOptions>();
  CHECK_FAILURE(envOptions->put_AllowSingleSignOnUsingOSPrimaryAccount(options_.AADSSOEnabled ? TRUE : FALSE));
  CHECK_FAILURE(envOptions->put_ExclusiveUserDataFolderAccess(options_.exclusiveUserDataFolderAccess ? TRUE : FALSE));
  if (!options_.language.isEmpty())
    CHECK_FAILURE(envOptions->put_Language(options_.language.toStdWString().c_str()));
  CHECK_FAILURE(envOptions->put_IsCustomCrashReportingEnabled(options_.customCrashReportingEnabled ? TRUE : FALSE));

  //! [CoreWebView2CustomSchemeRegistration]
  Microsoft::WRL::ComPtr<ICoreWebView2EnvironmentOptions4> options4;
  if (envOptions.As(&options4) == S_OK) {
    const WCHAR* allowedOrigins[1] = {L"https://*.example.com"};

    auto customSchemeRegistration = Microsoft::WRL::Make<CoreWebView2CustomSchemeRegistration>(L"custom-scheme");
    customSchemeRegistration->SetAllowedOrigins(1, allowedOrigins);

    auto customSchemeRegistration2 = Microsoft::WRL::Make<CoreWebView2CustomSchemeRegistration>(L"wv2rocks");
    customSchemeRegistration2->put_TreatAsSecure(TRUE);
    customSchemeRegistration2->SetAllowedOrigins(1, allowedOrigins);
    customSchemeRegistration2->put_HasAuthorityComponent(TRUE);

    auto customSchemeRegistration3 = Microsoft::WRL::Make<CoreWebView2CustomSchemeRegistration>(L"custom-scheme-not-in-allowed-origins");
    ICoreWebView2CustomSchemeRegistration* registrations[3] = {
        customSchemeRegistration.Get(),
        customSchemeRegistration2.Get(),
        customSchemeRegistration3.Get()};
    options4->SetCustomSchemeRegistrations(2, static_cast<ICoreWebView2CustomSchemeRegistration**>(registrations));
  }
  //! [CoreWebView2CustomSchemeRegistration]

  Microsoft::WRL::ComPtr<ICoreWebView2EnvironmentOptions5> options5;
  if (envOptions.As(&options5) == S_OK) {
    CHECK_FAILURE(options5->put_EnableTrackingPrevention(options_.trackingPreventionEnabled ? TRUE : FALSE));
  }

  Microsoft::WRL::ComPtr<ICoreWebView2EnvironmentOptions6> options6;
  if (envOptions.As(&options6) == S_OK) {
    CHECK_FAILURE(options6->put_AreBrowserExtensionsEnabled(TRUE));
  }

  LPCWSTR subFolder = nullptr;
  HRESULT hr = CreateCoreWebView2EnvironmentWithOptions(
      subFolder,
      options_.userDataFolder.toStdWString().c_str(),
      envOptions.Get(),
      Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(this, &QWebViewWebView2Impl::OnCreateEnvironmentCompleted).Get());

  if (SUCCEEDED(hr))
    initialized_ = true;
  return hr;
}

void QWebViewWebView2Impl::UninitializeWebView() {
  // 1. Delete components.
  DeleteAllComponents();

  // 2. If cleanup needed and BrowserProcessExited event interface available,
  // register to cleanup upon browser exit.
  wil::com_ptr<ICoreWebView2Environment5> environment5;
  if (m_webViewEnvironment) {
    environment5 = m_webViewEnvironment.try_query<ICoreWebView2Environment5>();
  }

#if 0  // TODO
    if (options_.enableCleanupUserDataFolder && environment5) {
        // Before closing the WebView, register a handler with code to run once the
        // browser process and associated processes are terminated.
        CHECK_FAILURE(environment5->add_BrowserProcessExited(
            Callback<ICoreWebView2BrowserProcessExitedEventHandler>(
                [environment5, this](ICoreWebView2Environment* sender, ICoreWebView2BrowserProcessExitedEventArgs* args) {
                    COREWEBVIEW2_BROWSER_PROCESS_EXIT_KIND kind;
                    UINT32 pid;
                    CHECK_FAILURE(args->get_BrowserProcessExitKind(&kind));
                    CHECK_FAILURE(args->get_BrowserProcessId(&pid));

                    // If a new WebView is created from this CoreWebView2Environment after
                    // the browser has exited but before our handler gets to run, a new
                    // browser process will be created and lock the user data folder
                    // again. Do not attempt to cleanup the user data folder in these
                    // cases. We check the PID of the exited browser process against the
                    // PID of the browser process to which our last CoreWebView2 attached.
                    if (pid == m_newestBrowserPid) {
                        // Watch for graceful browser process exit. Let ProcessFailed event
                        // handler take care of failed browser process termination.
                        if (kind == COREWEBVIEW2_BROWSER_PROCESS_EXIT_KIND_NORMAL) {
                            CHECK_FAILURE(environment5->remove_BrowserProcessExited(m_browserExitedEventToken));
                            // Release the environment only after the handler is invoked.
                            // Otherwise, there will be no environment to raise the event when
                            // the collection of WebView2 Runtime processes exit.
                            m_webViewEnvironment = nullptr;
                            CleanupUserDataFolder();
                        }
                    }
                    else {
                        // The exiting process is not the last in use. Do not attempt cleanup
                        // as we might still have a webview open over the user data folder.
                        // Do not block from event handler.
                        qDebug() << "A new browser process prevented cleanup of the user data folder.";
                    }

                    return S_OK;
                })
                .Get(),
            &m_browserExitedEventToken));
    }
#endif

  // 3. Close the webview.
  if (m_controller) {
    m_controller->Close();
    m_controller = nullptr;
    m_webView = nullptr;
    m_webView3 = nullptr;
  }

  // 4. If BrowserProcessExited event interface is not available, release
  // environment and proceed to cleanup immediately. If the interface is
  // available, release environment only if not waiting for the event.
  if (!environment5) {
    m_webViewEnvironment = nullptr;
#if 0  // TODO
        if (options_.enableCleanupUserDataFolder) {
            CleanupUserDataFolder();
        }
#endif
  }
  else if (true || !options_.cleanupUserDataFolderEnabled) {  // TODO
    // Release the environment object here only if no cleanup is needed.
    // If cleanup is needed, the environment object release is deferred
    // until the browser process exits, otherwise the handler for the
    // BrowserProcessExited event will not be called.
    m_webViewEnvironment = nullptr;
  }

  // reset profile name
  m_profileName = L"";

  initialized_ = false;
}

HWND QWebViewWebView2Impl::GetWindowHwnd() {
  HWND h = NULL;
  if (host_) {
    h = (HWND)host_->winId();
  }
  return h;
}

ICoreWebView2* QWebViewWebView2Impl::GetWebView() {
  return m_webView.get();
}

ICoreWebView2Controller* QWebViewWebView2Impl::GetController() {
  return m_controller.get();
}

ICoreWebView2Environment* QWebViewWebView2Impl::GetEnvironment() {
  return m_webViewEnvironment.get();
}

double QWebViewWebView2Impl::GetTextScale() {
  return m_uiSettings ? m_uiSettings.TextScaleFactor() : 1.0f;
}

WebView2Options QWebViewWebView2Impl::GetOptions() const {
  return options_;
}

WebView2Options& QWebViewWebView2Impl::GetOptionsRef() {
  return options_;
}

bool QWebViewWebView2Impl::ComponentsHandleWindowMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* result) {
  for (auto& component : m_components) {
    if (component->HandleWindowMessage(hWnd, message, wParam, lParam, result)) {
      return true;
    }
  }
  return false;
}

HRESULT QWebViewWebView2Impl::OnCreateEnvironmentCompleted(HRESULT result, ICoreWebView2Environment* environment) {
  if (result != S_OK) {
    ShowFailure(result, L"Failed to create environment object.");
    //emit host_->errorOccurred((long)result);
    return S_OK;
  }

  m_webViewEnvironment = environment;

  auto webViewEnvironment3 = m_webViewEnvironment.try_query<ICoreWebView2Environment3>();
  if (webViewEnvironment3 && (m_dcompDevice || m_wincompCompositor)) {
    result = webViewEnvironment3->CreateCoreWebView2CompositionController(
        GetWindowHwnd(),
        Callback<ICoreWebView2CreateCoreWebView2CompositionControllerCompletedHandler>(
            [this](HRESULT result, ICoreWebView2CompositionController* compositionController) -> HRESULT {
              auto controller = wil::com_ptr<ICoreWebView2CompositionController>(compositionController).query<ICoreWebView2Controller>();
              return OnCreateCoreWebView2ControllerCompleted(result, controller.get());
            })
            .Get());
  }
  else {
    result = m_webViewEnvironment->CreateCoreWebView2Controller(
        GetWindowHwnd(),
        Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(this, &QWebViewWebView2Impl::OnCreateCoreWebView2ControllerCompleted).Get());
  }
  return result;
}

HRESULT QWebViewWebView2Impl::OnCreateCoreWebView2ControllerCompleted(HRESULT result, ICoreWebView2Controller* controller) {
  if (result == S_OK) {
    m_controller = controller;
    wil::com_ptr<ICoreWebView2> coreWebView2;
    CHECK_FAILURE(m_controller->get_CoreWebView2(&coreWebView2));

    m_webView3 = coreWebView2.try_query<ICoreWebView2_3>();

    // We should check for failure here because if this app is using a newer
    // SDK version compared to the install of the Edge browser, the Edge
    // browser might not have support for the latest version of the
    // ICoreWebView2_N interface.
    coreWebView2.query_to(&m_webView);
    // Save PID of the browser process serving last WebView created from our
    // CoreWebView2Environment. We know the controller was created with
    // S_OK, and it hasn't been closed (we haven't called Close and no
    // ProcessFailed event could have been raised yet) so the PID is available.
    CHECK_FAILURE(m_webView->get_BrowserProcessId(&m_newestBrowserPid));

    //! [CoreWebView2Profile]
    auto webView2_13 = coreWebView2.try_query<ICoreWebView2_13>();
    if (webView2_13) {
      wil::com_ptr<ICoreWebView2Profile> profile;
      CHECK_FAILURE(webView2_13->get_Profile(&profile));

      wil::unique_cotaskmem_string profile_name;
      CHECK_FAILURE(profile->get_ProfileName(&profile_name));
      m_profileName = profile_name.get();

      BOOL inPrivate = FALSE;
      CHECK_FAILURE(profile->get_IsInPrivateModeEnabled(&inPrivate));

      if (!options_.downloadPath.isEmpty()) {
        CHECK_FAILURE(profile->put_DefaultDownloadFolderPath(options_.downloadPath.toStdWString().c_str()));
      }
    }
    //! [CoreWebView2Profile]

    // Create components. These will be deleted when the WebView is closed.
    NewComponent<ScriptComponent>(this);
    NewComponent<AudioComponent>(this);
    NewComponent<ViewComponent>(this,
                                m_dcompDevice.get(),
                                m_wincompCompositor,
                                options_.creationMode == CreationMode::TargetDComp);
    ApplyOptionsToViewComponent();

    NewComponent<ControlComponent>(this);
    NewComponent<SettingsComponent>(this, m_oldSettingsComponent.get());
    if (m_oldSettingsComponent) {
      m_oldSettingsComponent = nullptr;
    }
    else {
      ApplyOptionsToSettingComponent();
    }
    NewComponent<HostObjectComponent>(this);
    NewComponent<ProcessComponent>(this);
    NewComponent<NonClientRegionSupportComponent>(this);
    NewComponent<CookieManagementComponent>(this);

    DoSetVirtualHostNameAndFolder();

    // We have a few of our own event handlers to register here as well
    RegisterEventHandlers();

    // Set the initial size of the WebView
    RECT rc = {0};
    GetClientRect(GetWindowHwnd(), &rc);
    ResizeEverything(rc.right - rc.left, rc.bottom - rc.top);

    if (options_.initialUrl != "none") {
      CHECK_FAILURE(m_webView->Navigate(options_.initialUrl.toStdWString().c_str()));
    }
  }
  else if (result == E_ABORT) {
    // Webview creation was aborted because the user closed this window.
    // No need to report this as an error.
  }
  else if (result == HRESULT_FROM_WIN32(ERROR_DELETE_PENDING)) {
    ShowFailure(result, L"Failed to create webview, because the profile's name has been marked as deleted, please use a different profile's name");
    //emit host_->errorOccurred((long)result);
  }
  else {
    ShowFailure(result, L"Failed to create webview");
    //emit host_->errorOccurred((long)result);
  }
  return S_OK;
}

void QWebViewWebView2Impl::RegisterEventHandlers() {
  //! [ContainsFullScreenElementChanged]
  // Register a handler for the ContainsFullScreenChanged event.
  CHECK_FAILURE(m_webView->add_ContainsFullScreenElementChanged(
      Callback<ICoreWebView2ContainsFullScreenElementChangedEventHandler>(
          [this](ICoreWebView2* sender, IUnknown* args) -> HRESULT {
            BOOL containsFullscreenElement = FALSE;
            CHECK_FAILURE(sender->get_ContainsFullScreenElement(&containsFullscreenElement));
            emit host_->fullscreenChanged(containsFullscreenElement);
            return S_OK;
          })
          .Get(),
      nullptr));
  //! [ContainsFullScreenElementChanged]

  //! [NewWindowRequested]
  // Register a handler for the NewWindowRequested event.
  // This handler will defer the event, create a new app window, and then once the
  // new window is ready, it'll provide that new window's WebView as the response to
  // the request.
  CHECK_FAILURE(m_webView->add_NewWindowRequested(
      Callback<ICoreWebView2NewWindowRequestedEventHandler>(
          [this](ICoreWebView2* sender, ICoreWebView2NewWindowRequestedEventArgs* args) {
            args->put_Handled(FALSE);
            return S_OK;
          })
          .Get(),
      nullptr));
  //! [NewWindowRequested]

  //! [WindowCloseRequested]
  // Register a handler for the WindowCloseRequested event.
  // This handler will close the app window if it is not the main window.
  CHECK_FAILURE(m_webView->add_WindowCloseRequested(
      Callback<ICoreWebView2WindowCloseRequestedEventHandler>(
          [this](ICoreWebView2* sender, IUnknown* args) {
            return S_OK;
          })
          .Get(),
      nullptr));
  //! [WindowCloseRequested]

  //! [ProfileDeleted]
  auto webView2_13 = m_webView.try_query<ICoreWebView2_13>();
  CHECK_FEATURE_RETURN_EMPTY(webView2_13);
  wil::com_ptr<ICoreWebView2Profile> webView2Profile;
  CHECK_FAILURE(webView2_13->get_Profile(&webView2Profile));
  CHECK_FEATURE_RETURN_EMPTY(webView2Profile);
  auto webView2Profile8 = webView2Profile.try_query<ICoreWebView2Profile8>();
  CHECK_FEATURE_RETURN_EMPTY(webView2Profile8);
  CHECK_FAILURE(webView2Profile8->add_Deleted(
      Microsoft::WRL::Callback<ICoreWebView2ProfileDeletedEventHandler>(
          [this](ICoreWebView2Profile* sender, IUnknown* args) {
            qDebug() << "The profile has been marked for deletion. Any associated webview2 objects will be closed.";
            emit host_->profileDeleted();
            return S_OK;
          })
          .Get(),
      nullptr));
  //! [ProfileDeleted]
}

// Updates the sizing and positioning of everything in the window.
void QWebViewWebView2Impl::ResizeEverything(UINT width, UINT height) {
  RECT availableBounds = {0};
  qreal scale = host_->devicePixelRatioF();
  availableBounds.right = width;
  availableBounds.bottom = height;

  if (auto view = GetComponent<ViewComponent>()) {
    view->SetBounds(availableBounds);
  }
}

void QWebViewWebView2Impl::OpenDevToolsWindow() {
  if (m_webView) {
    m_webView->OpenDevToolsWindow();
  }
}

void QWebViewWebView2Impl::Navigate(const std::wstring& url) {
  if (m_webView) {
    m_webView->Navigate(url.c_str());
  }
  else {
    options_.initialUrl = QString::fromStdWString(url);
  }
}

std::wstring QWebViewWebView2Impl::GetUrl() const {
  wil::unique_cotaskmem_string uri;
  m_webView->get_Source(&uri);
  return uri.get();
}

void QWebViewWebView2Impl::Reload() {
  CHECK_FAILURE(m_webView->Reload());
}

void QWebViewWebView2Impl::StopLoading() {
  CHECK_FAILURE(m_webView->Stop());
}

bool QWebViewWebView2Impl::CanGoBack() {
  BOOL r = FALSE;
  CHECK_FAILURE(m_webView->get_CanGoBack(&r));
  return !!r;
}

void QWebViewWebView2Impl::GoBack() {
  CHECK_FAILURE(m_webView->GoBack());
}

bool QWebViewWebView2Impl::CanGoForward() {
  BOOL r = FALSE;
  CHECK_FAILURE(m_webView->get_CanGoForward(&r));
  return !!r;
}

void QWebViewWebView2Impl::GoForward() {
  CHECK_FAILURE(m_webView->GoForward());
}

bool QWebViewWebView2Impl::IsMute() {
  AudioComponent* audioCmpt = GetComponent<AudioComponent>();
  if (!audioCmpt) {
    qWarning() << "AudioComponent is null";
    return false;
  }

  bool mute = false;
  CHECK_FAILURE(audioCmpt->GetMuteState(mute));
  return mute;
}

void QWebViewWebView2Impl::SetMute(bool mute) {
  AudioComponent* audioCmpt = GetComponent<AudioComponent>();
  if (!audioCmpt) {
    qWarning() << "AudioComponent is null";
    return;
  }
  bool curMute = false;
  CHECK_FAILURE(audioCmpt->GetMuteState(curMute));
  if (curMute != mute) {
    audioCmpt->ToggleMuteState();
  }
}

void QWebViewWebView2Impl::RegisterSciptCallNotify(ScriptCallCallback cb) {
  scriptCallCb_ = cb;
}

ScriptCallCallback QWebViewWebView2Impl::GetSciptCallNotify() const {
  return scriptCallCb_;
}

HRESULT QWebViewWebView2Impl::PostStringWebMessage(const std::wstring& str) {
  ScriptComponent* scmpt = GetComponent<ScriptComponent>();
  if (!scmpt)
    return E_NOINTERFACE;

  return scmpt->PostStringWebMessage(str);
}

HRESULT QWebViewWebView2Impl::PostJSONWebMessage(const std::wstring& strJSON) {
  ScriptComponent* scmpt = GetComponent<ScriptComponent>();
  if (!scmpt)
    return E_NOINTERFACE;

  return scmpt->PostJsonWebMessage(strJSON);
}

HRESULT QWebViewWebView2Impl::ExecuteScriptWithResult(const std::wstring& s, ScriptResultCallback cb) {
  ScriptComponent* scmpt = GetComponent<ScriptComponent>();
  if (!scmpt)
    return E_NOINTERFACE;

  return scmpt->ExecuteScriptWithResult(s, cb);
}

QMap<QString, QString> QWebViewWebView2Impl::virtualHostNameAndFolder() const {
  return virtualHostNameAndFolderMap_;
}

void QWebViewWebView2Impl::SetVirtualHostNameAndFolder(const QMap<QString, QString>& m) {
  virtualHostNameAndFolderMap_ = m;

  if (m_webView3) {
    DoSetVirtualHostNameAndFolder();
  }
}

QWebViewWebView2* QWebViewWebView2Impl::GetHostWidget() {
  return host_;
}

// We have our own implementation of DCompositionCreateDevice2 that dynamically
// loads dcomp.dll to create the device. Not having a static dependency on dcomp.dll
// enables the sample app to run on versions of Windows that don't support dcomp.
HRESULT QWebViewWebView2Impl::DCompositionCreateDevice2(IUnknown* renderingDevice, REFIID riid, void** ppv) {
  HRESULT hr = E_FAIL;
  static decltype(::DCompositionCreateDevice2)* fnCreateDCompDevice2 = nullptr;
  if (fnCreateDCompDevice2 == nullptr) {
    HMODULE hmod = ::LoadLibraryEx(L"dcomp.dll", nullptr, 0);
    if (hmod != nullptr) {
      fnCreateDCompDevice2 = reinterpret_cast<decltype(::DCompositionCreateDevice2)*>(::GetProcAddress(hmod, "DCompositionCreateDevice2"));
    }
  }
  if (fnCreateDCompDevice2 != nullptr) {
    hr = fnCreateDCompDevice2(renderingDevice, riid, ppv);
  }
  return hr;
}

// WinRT APIs cannot run without a DispatcherQueue. This helper function creates a
// DispatcherQueueController (which instantiates a DispatcherQueue under the covers) that will
// manage tasks for the WinRT APIs. The DispatcherQueue implementation lives in
// CoreMessaging.dll Similar to dcomp.dll, we load CoreMessaging.dll dynamically so the sample
// app can run on versions of windows that don't have CoreMessaging.
HRESULT QWebViewWebView2Impl::TryCreateDispatcherQueue() {
  namespace winSystem = winrt::Windows::System;

  HRESULT hr = S_OK;
  thread_local winSystem::DispatcherQueueController dispatcherQueueController{nullptr};

  if (dispatcherQueueController == nullptr) {
    hr = E_FAIL;
    static decltype(::CreateDispatcherQueueController)* fnCreateDispatcherQueueController =
        nullptr;
    if (fnCreateDispatcherQueueController == nullptr) {
      HMODULE hmod = ::LoadLibraryEx(L"CoreMessaging.dll", nullptr, 0);
      if (hmod != nullptr) {
        fnCreateDispatcherQueueController =
            reinterpret_cast<decltype(::CreateDispatcherQueueController)*>(
                ::GetProcAddress(hmod, "CreateDispatcherQueueController"));
      }
    }
    if (fnCreateDispatcherQueueController != nullptr) {
      winSystem::DispatcherQueueController controller{nullptr};
      DispatcherQueueOptions options{
          sizeof(DispatcherQueueOptions), DQTYPE_THREAD_CURRENT, DQTAT_COM_STA};
      hr = fnCreateDispatcherQueueController(
          options, reinterpret_cast<ABI::Windows::System::IDispatcherQueueController**>(
                       winrt::put_abi(controller)));
      dispatcherQueueController = controller;
    }
  }

  return hr;
}

void QWebViewWebView2Impl::ApplyOptionsToViewComponent() {
  ViewComponent* viewComp = GetComponent<ViewComponent>();
  if (viewComp) {
    QColor bgColor = options_.backgroundColor;
    if (bgColor.isValid()) {
      viewComp->SetBackgroundColor(bgColor.rgb(), bgColor.alpha() == 0);
    }
  }
}

void QWebViewWebView2Impl::ApplyOptionsToSettingComponent() {
  SettingsComponent* settingComp = GetComponent<SettingsComponent>();
  if (settingComp) {
    CHECK_FAILURE(settingComp->EnableDefaultContextMenu(options_.contextMenuEnabled));
    CHECK_FAILURE(settingComp->SetBuiltInErrorPageEnabled(options_.builtInErrorPageEnabled));
  }

  AudioComponent* audioComp = GetComponent<AudioComponent>();
  if (audioComp) {
    bool curMute = false;
    if (SUCCEEDED(audioComp->GetMuteState(curMute))) {
      if (curMute != options_.mute) {
        CHECK_FAILURE(audioComp->ToggleMuteState());
      }
    }
  }
}

void QWebViewWebView2Impl::DoSetVirtualHostNameAndFolder() {
  if (m_webView3) {
    for (QMap<QString, QString>::const_iterator it = virtualHostNameAndFolderMap_.cbegin(); it != virtualHostNameAndFolderMap_.cend(); it++) {
      //! [AddVirtualHostNameToFolderMapping]
      // Setup host resource mapping for local files.
      CHECK_FAILURE(m_webView3->SetVirtualHostNameToFolderMapping(
          it.key().toStdWString().c_str(),
          it.value().toStdWString().c_str(),
          COREWEBVIEW2_HOST_RESOURCE_ACCESS_KIND_DENY_CORS));
      //! [AddVirtualHostNameToFolderMapping]
    }
  }
}

void QWebViewWebView2Impl::OnTextScaleChanged(winrt::Windows::UI::ViewManagement::UISettings const& uiSettings,
                                              winrt::Windows::Foundation::IInspectable const& args) {
  ViewComponent* v = GetComponent<ViewComponent>();
  if (v) {
    v->UpdateDpiAndTextScale();
  }
}

std::wstring QWebViewWebView2Impl::GetLocalPath(std::wstring relativePath, bool keep_exe_path) {
  WCHAR rawPath[MAX_PATH];
  GetModuleFileNameW(NULL, rawPath, MAX_PATH);
  std::wstring path(rawPath);
  if (keep_exe_path) {
    path.append(relativePath);
  }
  else {
    std::size_t index = path.find_last_of(L"\\") + 1;
    path.replace(index, path.length(), relativePath);
  }
  return path;
}

void QWebViewWebView2Impl::CleanupUserDataFolder() {
  // For non-UWP apps, the default user data folder {Executable File Name}.WebView2
  // is in the same directory next to the app executable. If end
  // developers specify userDataFolder during WebView environment
  // creation, they would need to pass in that explicit value here.
  // For more information about userDataFolder:
  // https://learn.microsoft.com/microsoft-edge/webview2/reference/win32/webview2-idl#createcorewebview2environmentwithoptions
  WCHAR userDataFolder[MAX_PATH] = L"";
  // Obtain the absolute path for relative paths that include "./" or "../"
  _wfullpath(userDataFolder, GetLocalPath(L".WebView2", true).c_str(), MAX_PATH);
  std::wstring userDataFolderPath(userDataFolder);

  CHECK_FAILURE(DeleteFileRecursive(userDataFolderPath));
}

HRESULT QWebViewWebView2Impl::DeleteFileRecursive(std::wstring path) {
  wil::com_ptr<IFileOperation> fileOperation;
  CHECK_FAILURE(
      CoCreateInstance(CLSID_FileOperation, NULL, CLSCTX_ALL, IID_PPV_ARGS(&fileOperation)));

  // Turn off all UI from being shown to the user during the operation.
  CHECK_FAILURE(fileOperation->SetOperationFlags(FOF_NO_UI));

  wil::com_ptr<IShellItem> userDataFolder;
  CHECK_FAILURE(
      SHCreateItemFromParsingName(path.c_str(), NULL, IID_PPV_ARGS(&userDataFolder)));

  // Add the operation
  CHECK_FAILURE(fileOperation->DeleteItem(userDataFolder.get(), NULL));
  userDataFolder.reset();

  // Perform the operation to delete the directory
  CHECK_FAILURE(fileOperation->PerformOperations());

  CHECK_FAILURE(fileOperation->Release());
  OleUninitialize();
  return S_OK;
}
