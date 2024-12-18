#if defined(_MSVC_) && _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include <functional>
#include <map>
#include <string>
#include <wrl\client.h>
#include "HostObjectSimple_h.h"

class HostObjectComponent;
class HostObjectSimple : public Microsoft::WRL::RuntimeClass<
                             Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>,
                             IHostObjectSimple,
                             IDispatch> {
   public:
    HostObjectSimple(HostObjectComponent* hostCmpt);

    // IDispatch implementation
    STDMETHODIMP GetTypeInfoCount(UINT* pctinfo) override;

    STDMETHODIMP GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo) override;

    STDMETHODIMP GetIDsOfNames(
        REFIID riid,
        LPOLESTR* rgszNames,
        UINT cNames,
        LCID lcid,
        DISPID* rgDispId) override;

    STDMETHODIMP Invoke(
        DISPID dispIdMember,
        REFIID riid,
        LCID lcid,
        WORD wFlags,
        DISPPARAMS* pDispParams,
        VARIANT* pVarResult,
        EXCEPINFO* pExcepInfo,
        UINT* puArgErr) override;

    virtual HRESULT STDMETHODCALLTYPE Call_V_V(
        /* [in] */ INT cmd) override;

    virtual HRESULT STDMETHODCALLTYPE Call_S_V(
        /* [in] */ INT cmd,
        /* [in] */ BSTR s) override;

    virtual HRESULT STDMETHODCALLTYPE Call_S_D(
        /* [in] */ INT cmd,
        /* [in] */ BSTR s,
        /* [retval][out] */ double* ret) override;

    virtual HRESULT STDMETHODCALLTYPE Call_S_I(
        /* [in] */ INT cmd,
        /* [in] */ BSTR s,
        /* [retval][out] */ INT* ret) override;

    virtual HRESULT STDMETHODCALLTYPE Call_S_S(
        /* [in] */ INT cmd,
        /* [in] */ BSTR s,
        /* [retval][out] */ BSTR* ret) override;

    virtual HRESULT STDMETHODCALLTYPE Call_I_V(
        /* [in] */ INT cmd,
        /* [in] */ INT i) override;

    virtual HRESULT STDMETHODCALLTYPE Call_I_D(
        /* [in] */ INT cmd,
        /* [in] */ INT i,
        /* [retval][out] */ double* ret) override;

    virtual HRESULT STDMETHODCALLTYPE Call_I_I(
        /* [in] */ INT cmd,
        /* [in] */ INT i,
        /* [retval][out] */ INT* ret) override;

    virtual HRESULT STDMETHODCALLTYPE Call_I_S(
        /* [in] */ INT cmd,
        /* [in] */ INT i,
        /* [retval][out] */ BSTR* ret) override;

    virtual HRESULT STDMETHODCALLTYPE Call_D_V(
        /* [in] */ INT cmd,
        /* [in] */ double d) override;

    virtual HRESULT STDMETHODCALLTYPE Call_D_D(
        /* [in] */ INT cmd,
        /* [in] */ double d,
        /* [retval][out] */ double* ret) override;

    virtual HRESULT STDMETHODCALLTYPE Call_D_I(
        /* [in] */ INT cmd,
        /* [in] */ double d,
        /* [retval][out] */ INT* ret) override;

    virtual HRESULT STDMETHODCALLTYPE Call_D_S(
        /* [in] */ INT cmd,
        /* [in] */ double d,
        /* [retval][out] */ BSTR* ret) override;

    virtual HRESULT STDMETHODCALLTYPE Call_IC_V(
        /* [in] */ INT cmd,
        /* [in] */ INT i,
        /* [in] */ IDispatch* dispatch) override;

    virtual HRESULT STDMETHODCALLTYPE Call_SC_V(
        /* [in] */ INT cmd,
        /* [in] */ BSTR s,
        /* [in] */ IDispatch* dispatch) override;

    virtual HRESULT STDMETHODCALLTYPE Call_DC_V(
        /* [in] */ INT cmd,
        /* [in] */ double d,
        /* [in] */ IDispatch* dispatch) override;

   private:
    HostObjectComponent* hostCmpt_ = nullptr;
    wil::com_ptr<ITypeLib> m_typeLib;
};
