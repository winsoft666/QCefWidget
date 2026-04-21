#ifndef AUDIO_COMPONENET_H_
#define AUDIO_COMPONENET_H_
#if defined(_MSVC_) && _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000
#include "ComponentBase.h"
#include <WebView2.h>

class AudioComponent : public ComponentBase {
   public:
    AudioComponent(QWebViewWebView2Impl* d);

    HRESULT GetDocumentPlayAudioState(bool& playing);
    HRESULT GetMuteState(bool& mute) const;
    HRESULT ToggleMuteState();

    ~AudioComponent() override;

   private:
    wil::com_ptr<ICoreWebView2> m_webView;

    EventRegistrationToken m_isDocumentPlayingAudioChangedToken = {};
    EventRegistrationToken m_isMutedChangedToken = {};
};
#endif
