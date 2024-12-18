#include "stdafx.h"
#include "AudioComponent.h"
#include "QWebViewWebView2Impl.h"

using namespace Microsoft::WRL;

AudioComponent::AudioComponent(QWebViewWebView2Impl* d) :
    ComponentBase(d) {
    m_webView = d_->GetWebView();
    auto webview2_8 = m_webView.try_query<ICoreWebView2_8>();
    if (webview2_8) {
        // Register a handler for the IsDocumentPlayingAudioChanged event.
        CHECK_FAILURE(webview2_8->add_IsDocumentPlayingAudioChanged(
            Callback<ICoreWebView2IsDocumentPlayingAudioChangedEventHandler>(
                [this, webview2_8](ICoreWebView2* sender, IUnknown* args) -> HRESULT {
                    return S_OK;
                })
                .Get(),
            &m_isDocumentPlayingAudioChangedToken));

        // Register a handler for the IsMutedChanged event.
        CHECK_FAILURE(webview2_8->add_IsMutedChanged(
            Callback<ICoreWebView2IsMutedChangedEventHandler>(
                [this, webview2_8](ICoreWebView2* sender, IUnknown* args) -> HRESULT {
                    return S_OK;
                })
                .Get(),
            &m_isMutedChangedToken));
    }
}

HRESULT AudioComponent::GetDocumentPlayAudioState(bool& playing) {
    HRESULT hr = E_FAIL;
    auto webview2_8 = m_webView.try_query<ICoreWebView2_8>();
    if (webview2_8) {
        BOOL isPlaying;
        hr = webview2_8->get_IsDocumentPlayingAudio(&isPlaying);
        playing = !!isPlaying;
    }
    return hr;
}

HRESULT AudioComponent::GetMuteState(bool& mute) const {
    HRESULT hr = E_FAIL;
    auto webview2_8 = m_webView.try_query<ICoreWebView2_8>();
    if (webview2_8) {
        BOOL isMuted;
        hr = webview2_8->get_IsMuted(&isMuted);
        mute = !!isMuted;
    }
    return hr;
}

// Toggle the mute state of the current window and show a mute or unmute icon on the title bar
HRESULT AudioComponent::ToggleMuteState() {
    HRESULT hr = E_FAIL;
    auto webview2_8 = m_webView.try_query<ICoreWebView2_8>();
    if (webview2_8) {
        BOOL isMuted;
        hr = webview2_8->get_IsMuted(&isMuted);
        if (hr == S_OK) {
            hr = webview2_8->put_IsMuted(!isMuted);
        }
    }

    return hr;
}

AudioComponent::~AudioComponent() {
    auto webview2_8 = m_webView.try_query<ICoreWebView2_8>();
    if (webview2_8) {
        CHECK_FAILURE(webview2_8->remove_IsDocumentPlayingAudioChanged(m_isDocumentPlayingAudioChangedToken));
        CHECK_FAILURE(webview2_8->remove_IsMutedChanged(m_isMutedChangedToken));
    }
}