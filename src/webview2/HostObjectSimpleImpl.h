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

  virtual HRESULT STDMETHODCALLTYPE NotifyCPP(
      /* [in] */ BSTR s) override;

 private:
  HostObjectComponent* hostCmpt_ = nullptr;
  wil::com_ptr<ITypeLib> m_typeLib;
};
