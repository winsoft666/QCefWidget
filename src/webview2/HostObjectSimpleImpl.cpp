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
        RETURN_IF_FAILED(LoadTypeLib(L"QWebView2.tlb", &m_typeLib));
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

HRESULT STDMETHODCALLTYPE HostObjectSimple::Call_V_V(/* [in] */ INT cmd) {
    ScriptCallCallback cb = nullptr;
    if (hostCmpt_)
        cb = hostCmpt_->GetScriptCallNotify();

    if (!cb) {
        qDebug() << "Not register script call notify.";
        return E_NOTIMPL;
    }

    cb(ScriptCallMode::Call_V_V, cmd, QVariant(), nullptr);

    return S_OK;
}

HRESULT STDMETHODCALLTYPE HostObjectSimple::Call_S_V(/* [in] */ INT cmd, /* [in] */ BSTR s) {
    ScriptCallCallback cb = nullptr;
    if (hostCmpt_)
        cb = hostCmpt_->GetScriptCallNotify();

    if (!cb) {
        qDebug() << "Not register script call notify.";
        return E_NOTIMPL;
    }

    cb(ScriptCallMode::Call_S_V, cmd, QVariant(QString::fromStdWString(s)), nullptr);

    return S_OK;
}

HRESULT STDMETHODCALLTYPE HostObjectSimple::Call_S_D(/* [in] */ INT cmd, /* [in] */ BSTR s, /* [retval][out] */ double* ret) {
    ScriptCallCallback cb = nullptr;
    if (hostCmpt_)
        cb = hostCmpt_->GetScriptCallNotify();

    if (!cb) {
        qDebug() << "Not register script call notify.";
        return E_NOTIMPL;
    }

    QVariant var = cb(ScriptCallMode::Call_S_D, cmd, QVariant(QString::fromStdWString(s)), nullptr);

    *ret = var.toDouble();

    return S_OK;
}

HRESULT STDMETHODCALLTYPE HostObjectSimple::Call_S_I(/* [in] */ INT cmd, /* [in] */ BSTR s, /* [retval][out] */ INT* ret) {
    ScriptCallCallback cb = nullptr;
    if (hostCmpt_)
        cb = hostCmpt_->GetScriptCallNotify();

    if (!cb) {
        qDebug() << "Not register script call notify.";
        return E_NOTIMPL;
    }

    QVariant var = cb(ScriptCallMode::Call_S_I, cmd, QVariant(QString::fromStdWString(s)), nullptr);

    *ret = var.toInt();

    return S_OK;
}

HRESULT STDMETHODCALLTYPE HostObjectSimple::Call_S_S(/* [in] */ INT cmd, /* [in] */ BSTR s, /* [retval][out] */ BSTR* ret) {
    ScriptCallCallback cb = nullptr;
    if (hostCmpt_)
        cb = hostCmpt_->GetScriptCallNotify();

    if (!cb) {
        qDebug() << "Not register script call notify.";
        return E_NOTIMPL;
    }

    QVariant var = cb(ScriptCallMode::Call_S_S, cmd, QVariant(QString::fromStdWString(s)), nullptr);

    *ret = SysAllocString(var.toString().toStdWString().c_str());

    return S_OK;
}

HRESULT STDMETHODCALLTYPE HostObjectSimple::Call_I_V(/* [in] */ INT cmd, /* [in] */ INT i) {
    ScriptCallCallback cb = nullptr;
    if (hostCmpt_)
        cb = hostCmpt_->GetScriptCallNotify();

    if (!cb) {
        qDebug() << "Not register script call notify.";
        return E_NOTIMPL;
    }

    cb(ScriptCallMode::Call_I_V, cmd, QVariant(i), nullptr);

    return S_OK;
}

HRESULT STDMETHODCALLTYPE HostObjectSimple::Call_I_D(/* [in] */ INT cmd, /* [in] */ INT i, /* [retval][out] */ double* ret) {
    ScriptCallCallback cb = nullptr;
    if (hostCmpt_)
        cb = hostCmpt_->GetScriptCallNotify();

    if (!cb) {
        qDebug() << "Not register script call notify.";
        return E_NOTIMPL;
    }

    QVariant var = cb(ScriptCallMode::Call_I_D, cmd, QVariant(i), nullptr);

    *ret = var.toDouble();

    return S_OK;
}

HRESULT STDMETHODCALLTYPE HostObjectSimple::Call_I_I(/* [in] */ INT cmd, /* [in] */ INT i, /* [retval][out] */ INT* ret) {
    ScriptCallCallback cb = nullptr;
    if (hostCmpt_)
        cb = hostCmpt_->GetScriptCallNotify();

    if (!cb) {
        qDebug() << "Not register script call notify.";
        return E_NOTIMPL;
    }

    QVariant var = cb(ScriptCallMode::Call_I_I, cmd, QVariant(i), nullptr);

    *ret = var.toInt();

    return S_OK;
}

HRESULT STDMETHODCALLTYPE HostObjectSimple::Call_I_S(/* [in] */ INT cmd, /* [in] */ INT i, /* [retval][out] */ BSTR* ret) {
    ScriptCallCallback cb = nullptr;
    if (hostCmpt_)
        cb = hostCmpt_->GetScriptCallNotify();

    if (!cb) {
        qDebug() << "Not register script call notify.";
        return E_NOTIMPL;
    }

    QVariant var = cb(ScriptCallMode::Call_I_S, cmd, QVariant(i), nullptr);

    *ret = SysAllocString(var.toString().toStdWString().c_str());

    return S_OK;
}

HRESULT STDMETHODCALLTYPE HostObjectSimple::Call_D_V(/* [in] */ INT cmd, /* [in] */ double d) {
    ScriptCallCallback cb = nullptr;
    if (hostCmpt_)
        cb = hostCmpt_->GetScriptCallNotify();

    if (!cb) {
        qDebug() << "Not register script call notify.";
        return E_NOTIMPL;
    }

    cb(ScriptCallMode::Call_D_V, cmd, QVariant(d), nullptr);

    return S_OK;
}

HRESULT STDMETHODCALLTYPE HostObjectSimple::Call_D_D(/* [in] */ INT cmd, /* [in] */ double d, /* [retval][out] */ double* ret) {
    ScriptCallCallback cb = nullptr;
    if (hostCmpt_)
        cb = hostCmpt_->GetScriptCallNotify();

    if (!cb) {
        qDebug() << "Not register script call notify.";
        return E_NOTIMPL;
    }

    QVariant var = cb(ScriptCallMode::Call_D_D, cmd, QVariant(d), nullptr);

    *ret = var.toDouble();

    return S_OK;
}

HRESULT STDMETHODCALLTYPE HostObjectSimple::Call_D_I(/* [in] */ INT cmd, /* [in] */ double d, /* [retval][out] */ INT* ret) {
    ScriptCallCallback cb = nullptr;
    if (hostCmpt_)
        cb = hostCmpt_->GetScriptCallNotify();

    if (!cb) {
        qDebug() << "Not register script call notify.";
        return E_NOTIMPL;
    }

    QVariant var = cb(ScriptCallMode::Call_D_I, cmd, QVariant(d), nullptr);

    *ret = var.toInt();

    return S_OK;
}

HRESULT STDMETHODCALLTYPE HostObjectSimple::Call_D_S(/* [in] */ INT cmd, /* [in] */ double d, /* [retval][out] */ BSTR* ret) {
    ScriptCallCallback cb = nullptr;
    if (hostCmpt_)
        cb = hostCmpt_->GetScriptCallNotify();

    if (!cb) {
        qDebug() << "Not register script call notify.";
        return E_NOTIMPL;
    }

    QVariant var = cb(ScriptCallMode::Call_D_S, cmd, QVariant(d), nullptr);

    *ret = SysAllocString(var.toString().toStdWString().c_str());

    return S_OK;
}

HRESULT STDMETHODCALLTYPE HostObjectSimple::Call_IC_V(/* [in] */ INT cmd, /* [in] */ INT i, /* [in] */ IDispatch* dispatch) {
    ScriptCallCallback cb = nullptr;
    if (hostCmpt_)
        cb = hostCmpt_->GetScriptCallNotify();

    if (!cb) {
        qDebug() << "Not register script call notify.";
        return E_NOTIMPL;
    }

    // TODO
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE HostObjectSimple::Call_SC_V(/* [in] */ INT cmd, /* [in] */ BSTR s, /* [in] */ IDispatch* dispatch) {
    ScriptCallCallback cb = nullptr;
    if (hostCmpt_)
        cb = hostCmpt_->GetScriptCallNotify();

    if (!cb) {
        qDebug() << "Not register script call notify.";
        return E_NOTIMPL;
    }

    // TODO
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE HostObjectSimple::Call_DC_V(/* [in] */ INT cmd, /* [in] */ double d, /* [in] */ IDispatch* dispatch) {
    ScriptCallCallback cb = nullptr;
    if (hostCmpt_)
        cb = hostCmpt_->GetScriptCallNotify();

    if (!cb) {
        qDebug() << "Not register script call notify.";
        return E_NOTIMPL;
    }

    // TODO
    return E_NOTIMPL;
}
