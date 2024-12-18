#include "stdafx.h"
#include "ControlComponent.h"
#include "CheckFailure.h"
#include "QWebViewWebView2Impl.h"

using namespace Microsoft::WRL;

namespace {
std::wstring WebErrorStatusToString(COREWEBVIEW2_WEB_ERROR_STATUS status) {
    switch (status) {
#define STATUS_ENTRY(statusValue) \
    case statusValue:             \
        return L#statusValue;

        STATUS_ENTRY(COREWEBVIEW2_WEB_ERROR_STATUS_UNKNOWN);
        STATUS_ENTRY(COREWEBVIEW2_WEB_ERROR_STATUS_CERTIFICATE_COMMON_NAME_IS_INCORRECT);
        STATUS_ENTRY(COREWEBVIEW2_WEB_ERROR_STATUS_CERTIFICATE_EXPIRED);
        STATUS_ENTRY(COREWEBVIEW2_WEB_ERROR_STATUS_CLIENT_CERTIFICATE_CONTAINS_ERRORS);
        STATUS_ENTRY(COREWEBVIEW2_WEB_ERROR_STATUS_CERTIFICATE_REVOKED);
        STATUS_ENTRY(COREWEBVIEW2_WEB_ERROR_STATUS_CERTIFICATE_IS_INVALID);
        STATUS_ENTRY(COREWEBVIEW2_WEB_ERROR_STATUS_SERVER_UNREACHABLE);
        STATUS_ENTRY(COREWEBVIEW2_WEB_ERROR_STATUS_TIMEOUT);
        STATUS_ENTRY(COREWEBVIEW2_WEB_ERROR_STATUS_ERROR_HTTP_INVALID_SERVER_RESPONSE);
        STATUS_ENTRY(COREWEBVIEW2_WEB_ERROR_STATUS_CONNECTION_ABORTED);
        STATUS_ENTRY(COREWEBVIEW2_WEB_ERROR_STATUS_CONNECTION_RESET);
        STATUS_ENTRY(COREWEBVIEW2_WEB_ERROR_STATUS_DISCONNECTED);
        STATUS_ENTRY(COREWEBVIEW2_WEB_ERROR_STATUS_CANNOT_CONNECT);
        STATUS_ENTRY(COREWEBVIEW2_WEB_ERROR_STATUS_HOST_NAME_NOT_RESOLVED);
        STATUS_ENTRY(COREWEBVIEW2_WEB_ERROR_STATUS_OPERATION_CANCELED);
        STATUS_ENTRY(COREWEBVIEW2_WEB_ERROR_STATUS_REDIRECT_FAILED);
        STATUS_ENTRY(COREWEBVIEW2_WEB_ERROR_STATUS_UNEXPECTED_ERROR);

#undef STATUS_ENTRY
    }

    return L"ERROR";
}
}  // namespace

ControlComponent::ControlComponent(QWebViewWebView2Impl* d) :
    ComponentBase(d) {
    m_webView = d_->GetWebView();
    m_controller = d_->GetController();

    // Register a handler for the NavigationStarting event.
    // This handler just enables the Cancel button.
    CHECK_FAILURE(m_webView->add_NavigationStarting(
        Callback<ICoreWebView2NavigationStartingEventHandler>(
            [this](ICoreWebView2* sender, ICoreWebView2NavigationStartingEventArgs* args) -> HRESULT {
                wil::unique_cotaskmem_string uri;
                CHECK_FAILURE(sender->get_Source(&uri));

                emit d_->GetHostWidget()->loadStateChanged(true);

                return S_OK;
            })
            .Get(),
        &m_navigationStartingToken));

    //! [SourceChanged]
    // Register a handler for the SourceChanged event.
    // This handler will read the webview's source URI and update the app's address bar.
    CHECK_FAILURE(m_webView->add_SourceChanged(
        Callback<ICoreWebView2SourceChangedEventHandler>(
            [this](ICoreWebView2* sender, ICoreWebView2SourceChangedEventArgs* args) -> HRESULT {
                wil::unique_cotaskmem_string uri;
                sender->get_Source(&uri);
                if (wcscmp(uri.get(), L"about:blank") == 0) {
                    uri = wil::make_cotaskmem_string(L"");
                }

                // TODO

                return S_OK;
            })
            .Get(),
        &m_sourceChangedToken));
    //! [SourceChanged]

    //! [HistoryChanged]
    // Register a handler for the HistoryChanged event.
    // Update the Back, Forward buttons.
    CHECK_FAILURE(m_webView->add_HistoryChanged(
        Callback<ICoreWebView2HistoryChangedEventHandler>([this](ICoreWebView2* sender, IUnknown* args) -> HRESULT {
            BOOL canGoBack;
            BOOL canGoForward;
            sender->get_CanGoBack(&canGoBack);
            sender->get_CanGoForward(&canGoForward);

            // TODO

            return S_OK;
        })
            .Get(),
        &m_historyChangedToken));
    //! [HistoryChanged]

    //! [NavigationCompleted]
    // Register a handler for the NavigationCompleted event.
    // Check whether the navigation succeeded, and if not, do something.
    // Also update the Cancel buttons.
    CHECK_FAILURE(m_webView->add_NavigationCompleted(
        Callback<ICoreWebView2NavigationCompletedEventHandler>(
            [this](ICoreWebView2* sender, ICoreWebView2NavigationCompletedEventArgs* args) -> HRESULT {
                std::wstring errStatus;
                wil::unique_cotaskmem_string uri;
                CHECK_FAILURE(sender->get_Source(&uri));

                BOOL success = FALSE;
                bool isCatastrophic = false;
                CHECK_FAILURE(args->get_IsSuccess(&success));
                if (!success) {
                    COREWEBVIEW2_WEB_ERROR_STATUS webErrorStatus = COREWEBVIEW2_WEB_ERROR_STATUS_UNKNOWN;
                    if (SUCCEEDED(args->get_WebErrorStatus(&webErrorStatus))) {
                        isCatastrophic = (webErrorStatus != COREWEBVIEW2_WEB_ERROR_STATUS_OPERATION_CANCELED &&
                                          webErrorStatus != COREWEBVIEW2_WEB_ERROR_STATUS_CONNECTION_ABORTED &&
                                          webErrorStatus != COREWEBVIEW2_WEB_ERROR_STATUS_VALID_AUTHENTICATION_CREDENTIALS_REQUIRED &&
                                          webErrorStatus != COREWEBVIEW2_WEB_ERROR_STATUS_VALID_PROXY_AUTHENTICATION_REQUIRED);
                        errStatus = WebErrorStatusToString(webErrorStatus);
                    }
                }

                //emit d_->GetHostWidget()->loadUrlFinished(!!success,
                //                                          QString::fromStdWString(uri.get()),
                //                                          isCatastrophic,
                //                                          QString::fromStdWString(errStatus));

                emit d_->GetHostWidget()->loadStateChanged(false);
                return S_OK;
            })
            .Get(),
        &m_navigationCompletedToken));
    //! [NavigationCompleted]

    //! [FrameNavigationCompleted]
    // Register a handler for the FrameNavigationCompleted event.
    // Check whether the navigation succeeded, and if not, do something.
    CHECK_FAILURE(m_webView->add_FrameNavigationCompleted(
        Callback<ICoreWebView2NavigationCompletedEventHandler>(
            [this](ICoreWebView2* sender, ICoreWebView2NavigationCompletedEventArgs* args) -> HRESULT {
                BOOL success;
                CHECK_FAILURE(args->get_IsSuccess(&success));
                if (!success) {
                    COREWEBVIEW2_WEB_ERROR_STATUS webErrorStatus;
                    CHECK_FAILURE(args->get_WebErrorStatus(&webErrorStatus));
                    // The web page can cancel its own iframe loads, so we'll ignore that.
                    if (webErrorStatus != COREWEBVIEW2_WEB_ERROR_STATUS_OPERATION_CANCELED) {
                        //m_appWindow->AsyncMessageBox(
                        //    L"Iframe navigation failed: " + WebErrorStatusToString(webErrorStatus),
                        //    L"Navigation Failure");
                    }
                }
                return S_OK;
            })
            .Get(),
        &m_frameNavigationCompletedToken));
    //! [FrameNavigationCompleted]

    //! [MoveFocusRequested]
    // Register a handler for the MoveFocusRequested event.
    // This event will be fired when the user tabs out of the webview.
    // The handler will focus another window in the app, depending on which
    // direction the focus is being shifted.
    CHECK_FAILURE(m_controller->add_MoveFocusRequested(
        Callback<ICoreWebView2MoveFocusRequestedEventHandler>(
            [this](
                ICoreWebView2Controller* sender,
                ICoreWebView2MoveFocusRequestedEventArgs* args) -> HRESULT {
                //if (!g_autoTabHandle) {
                //    COREWEBVIEW2_MOVE_FOCUS_REASON reason;
                //    CHECK_FAILURE(args->get_Reason(&reason));

                //    if (reason == COREWEBVIEW2_MOVE_FOCUS_REASON_NEXT) {
                //        TabForwards(-1);
                //    }
                //    else if (reason == COREWEBVIEW2_MOVE_FOCUS_REASON_PREVIOUS) {
                //        TabBackwards(m_tabbableWindows.size());
                //    }
                //    CHECK_FAILURE(args->put_Handled(TRUE));
                //}
                return S_OK;
            })
            .Get(),
        &m_moveFocusRequestedToken));
    //! [MoveFocusRequested]

    //! [AcceleratorKeyPressed]
    // Register a handler for the AcceleratorKeyPressed event.
    CHECK_FAILURE(m_controller->add_AcceleratorKeyPressed(
        Callback<ICoreWebView2AcceleratorKeyPressedEventHandler>(
            [this](ICoreWebView2Controller* sender, ICoreWebView2AcceleratorKeyPressedEventArgs* args) -> HRESULT {
                COREWEBVIEW2_KEY_EVENT_KIND kind;
                CHECK_FAILURE(args->get_KeyEventKind(&kind));
                // We only care about key down events.
                if (kind == COREWEBVIEW2_KEY_EVENT_KIND_KEY_DOWN ||
                    kind == COREWEBVIEW2_KEY_EVENT_KIND_SYSTEM_KEY_DOWN) {
                    CHECK_FAILURE(args->put_Handled(FALSE));

                    // Filter out autorepeated keys.
                    COREWEBVIEW2_PHYSICAL_KEY_STATUS status;
                    if (SUCCEEDED(args->get_PhysicalKeyStatus(&status))) {
                        if (!status.WasKeyDown) {
                            UINT key = 0;
                            if (SUCCEEDED(args->get_VirtualKey(&key))) {
                                emit d_->GetHostWidget()->acceleratorKeyPressed(key);
                            }
                        }
                    }
                }
                return S_OK;
            })
            .Get(),
        &m_acceleratorKeyPressedToken));
    //! [AcceleratorKeyPressed]
}

ControlComponent::~ControlComponent() {
    m_webView->remove_NavigationStarting(m_navigationStartingToken);
    m_webView->remove_SourceChanged(m_sourceChangedToken);
    m_webView->remove_HistoryChanged(m_historyChangedToken);
    m_webView->remove_NavigationCompleted(m_navigationCompletedToken);
    m_webView->remove_FrameNavigationCompleted(m_frameNavigationCompletedToken);
    m_controller->remove_MoveFocusRequested(m_moveFocusRequestedToken);
    m_controller->remove_AcceleratorKeyPressed(m_acceleratorKeyPressedToken);
}

