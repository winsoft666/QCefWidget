#include "stdafx.h"
#include "HostObjectSimpleImpl.h"
#include "HostObjectComponent.h"
#include <QDebug>

HostObjectSimple::HostObjectSimple(HostObjectComponent* hostCmpt) :
    hostCmpt_(hostCmpt) {
}

STDMETHODIMP HostObjectSimple::GetTypeInfoCount(UINT* pctinfo) {
    *pctinfo = 1;
    return S_OK;
}

STDMETHODIMP HostObjectSimple::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo) {
    if (0 != iTInfo) {
        return TYPE_E_ELEMENTNOTFOUND;
    }
    if (!m_typeLib) {
        RETURN_IF_FAILED(LoadTypeLib(L"QWebView.WebView2.tlb", &m_typeLib));
    }
    return m_typeLib->GetTypeInfoOfGuid(__uuidof(IHostObjectSimple), ppTInfo);
}

STDMETHODIMP HostObjectSimple::GetIDsOfNames(
    REFIID riid,
    LPOLESTR* rgszNames,
    UINT cNames,
    LCID lcid,
    DISPID* rgDispId) {
    wil::com_ptr<ITypeInfo> typeInfo;
    RETURN_IF_FAILED(GetTypeInfo(0, lcid, &typeInfo));
    return typeInfo->GetIDsOfNames(rgszNames, cNames, rgDispId);
}

STDMETHODIMP HostObjectSimple::Invoke(
    DISPID dispIdMember,
    REFIID riid,
    LCID lcid,
    WORD wFlags,
    DISPPARAMS* pDispParams,
    VARIANT* pVarResult,
    EXCEPINFO* pExcepInfo,
    UINT* puArgErr) {
    wil::com_ptr<ITypeInfo> typeInfo;
    RETURN_IF_FAILED(GetTypeInfo(0, lcid, &typeInfo));
    return typeInfo->Invoke(this, dispIdMember, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
}

HRESULT STDMETHODCALLTYPE HostObjectSimple::NotifyCPP(/* [in] */ BSTR s) {
    ScriptCallCallback cb = nullptr;
    if (hostCmpt_)
        cb = hostCmpt_->GetScriptCallNotify();

    if (!cb) {
        qDebug() << "Not register script call notify.";
        return E_NOTIMPL;
    }

    cb(QVariant(QString::fromStdWString(s)), nullptr);

    return S_OK;
}
