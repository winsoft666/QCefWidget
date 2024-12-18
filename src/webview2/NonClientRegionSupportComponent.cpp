#include "stdafx.h"
#include "CheckFailure.h"
#include "NonClientRegionSupportComponent.h"
#include "QWebViewWebView2Impl.h"

using namespace Microsoft::WRL;

NonClientRegionSupportComponent::NonClientRegionSupportComponent(QWebViewWebView2Impl* d) :
    ComponentBase(d) {
    m_webView = d_->GetWebView();

    wil::com_ptr<ICoreWebView2Controller> controller = d_->GetController();
    wil::com_ptr<ICoreWebView2CompositionController> compController =
        controller.try_query<ICoreWebView2CompositionController>();
    if (compController) {
        m_compController4 = compController.try_query<ICoreWebView2CompositionController4>();
    }

    CHECK_FAILURE(m_webView->get_Settings(&m_settings));

    m_settings9 = m_settings.try_query<ICoreWebView2Settings9>();

    CHECK_FAILURE(m_webView->add_NavigationStarting(
        Callback<ICoreWebView2NavigationStartingEventHandler>(
            [this](ICoreWebView2* sender, ICoreWebView2NavigationStartingEventArgs* args) -> HRESULT {
                wil::unique_cotaskmem_string uri;
                CHECK_FAILURE(args->get_Uri(&uri));
                if (m_settings9) {
                    BOOL enabled = 0;
                    CHECK_FAILURE(m_settings9->get_IsNonClientRegionSupportEnabled(&enabled));

                    BOOL expect = d_->GetOptions().nonClientRegionSupportEnabled ? TRUE : FALSE;
                    if (expect != enabled) {
                        CHECK_FAILURE(m_settings9->put_IsNonClientRegionSupportEnabled(expect));
                        if (expect) {
                            AddChangeListener();
                        }
                    }
                }

                return S_OK;
            })
            .Get(),
        &m_navigationStartingToken));

    // Turn off this scenario if we navigate away from the sample page
    CHECK_FAILURE(m_webView->add_ContentLoading(
        Callback<ICoreWebView2ContentLoadingEventHandler>(
            [this](ICoreWebView2* sender, ICoreWebView2ContentLoadingEventArgs* args) -> HRESULT {
                //wil::unique_cotaskmem_string uri;
                //sender->get_Source(&uri);
                //d_->DeleteComponent(this);
                return S_OK;
            })
            .Get(),
        &m_ContentLoadingToken));
}
//! [AddChangeListener]
void NonClientRegionSupportComponent::AddChangeListener() {
    if (m_compController4) {
        CHECK_FAILURE(m_compController4->add_NonClientRegionChanged(
            Callback<ICoreWebView2NonClientRegionChangedEventHandler>(
                [this](ICoreWebView2CompositionController* sender, ICoreWebView2NonClientRegionChangedEventArgs* args) -> HRESULT {
                    COREWEBVIEW2_NON_CLIENT_REGION_KIND region = COREWEBVIEW2_NON_CLIENT_REGION_KIND_NOWHERE;
                    args->get_RegionKind(&region);

                    wil::com_ptr<ICoreWebView2RegionRectCollectionView> regionsCollection;
                    m_compController4->QueryNonClientRegion(region, &regionsCollection);

                    UINT32 count = 0;
                    regionsCollection->get_Count(&count);

                    RECT rect;
                    regionsCollection->GetValueAtIndex(0, &rect);
                    return S_OK;
                })
                .Get(),
            &m_nonClientRegionChanged));
    }
}
//! [AddChangeListener]
NonClientRegionSupportComponent::~NonClientRegionSupportComponent() {
    CHECK_FAILURE(m_webView->remove_NavigationStarting(m_navigationStartingToken));
    CHECK_FAILURE(m_webView->remove_ContentLoading(m_ContentLoadingToken));
    if (m_compController4) {
        CHECK_FAILURE(m_compController4->remove_NonClientRegionChanged(m_nonClientRegionChanged));
    }
}