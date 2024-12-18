

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0628 */
/* at Tue Jan 19 11:14:07 2038
 */
/* Compiler settings for D:/CODE/OOO/QWebView/src/webview2/HostObjectSimple.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.01.0628 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */


#ifndef __HostObjectSimple_h_h__
#define __HostObjectSimple_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef DECLSPEC_XFGVIRT
#if defined(_CONTROL_FLOW_GUARD_XFG)
#define DECLSPEC_XFGVIRT(base, func) __declspec(xfg_virtual(base, func))
#else
#define DECLSPEC_XFGVIRT(base, func)
#endif
#endif

/* Forward Declarations */ 

#ifndef __IHostObjectSimple_FWD_DEFINED__
#define __IHostObjectSimple_FWD_DEFINED__
typedef interface IHostObjectSimple IHostObjectSimple;

#endif 	/* __IHostObjectSimple_FWD_DEFINED__ */


#ifndef __HostObjectSimple_FWD_DEFINED__
#define __HostObjectSimple_FWD_DEFINED__

#ifdef __cplusplus
typedef class HostObjectSimple HostObjectSimple;
#else
typedef struct HostObjectSimple HostObjectSimple;
#endif /* __cplusplus */

#endif 	/* __HostObjectSimple_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __HostObjectSimpleLibrary_LIBRARY_DEFINED__
#define __HostObjectSimpleLibrary_LIBRARY_DEFINED__

/* library HostObjectSimpleLibrary */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_HostObjectSimpleLibrary;

#ifndef __IHostObjectSimple_INTERFACE_DEFINED__
#define __IHostObjectSimple_INTERFACE_DEFINED__

/* interface IHostObjectSimple */
/* [local][object][uuid] */ 


EXTERN_C const IID IID_IHostObjectSimple;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3a14c9c0-bc3e-453f-a314-4ce4a0ec81d8")
    IHostObjectSimple : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Call_V_V( 
            /* [in] */ INT cmd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Call_S_V( 
            /* [in] */ INT cmd,
            /* [in] */ BSTR s) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Call_S_D( 
            /* [in] */ INT cmd,
            /* [in] */ BSTR s,
            /* [retval][out] */ double *ret) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Call_S_I( 
            /* [in] */ INT cmd,
            /* [in] */ BSTR s,
            /* [retval][out] */ INT *ret) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Call_S_S( 
            /* [in] */ INT cmd,
            /* [in] */ BSTR s,
            /* [retval][out] */ BSTR *ret) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Call_I_V( 
            /* [in] */ INT cmd,
            /* [in] */ INT i) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Call_I_D( 
            /* [in] */ INT cmd,
            /* [in] */ INT i,
            /* [retval][out] */ double *ret) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Call_I_I( 
            /* [in] */ INT cmd,
            /* [in] */ INT i,
            /* [retval][out] */ INT *ret) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Call_I_S( 
            /* [in] */ INT cmd,
            /* [in] */ INT i,
            /* [retval][out] */ BSTR *ret) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Call_D_V( 
            /* [in] */ INT cmd,
            /* [in] */ double d) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Call_D_D( 
            /* [in] */ INT cmd,
            /* [in] */ double d,
            /* [retval][out] */ double *ret) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Call_D_I( 
            /* [in] */ INT cmd,
            /* [in] */ double d,
            /* [retval][out] */ INT *ret) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Call_D_S( 
            /* [in] */ INT cmd,
            /* [in] */ double d,
            /* [retval][out] */ BSTR *ret) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Call_IC_V( 
            /* [in] */ INT cmd,
            /* [in] */ INT i,
            /* [in] */ IDispatch *dispatch) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Call_SC_V( 
            /* [in] */ INT cmd,
            /* [in] */ BSTR s,
            /* [in] */ IDispatch *dispatch) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Call_DC_V( 
            /* [in] */ INT cmd,
            /* [in] */ double d,
            /* [in] */ IDispatch *dispatch) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IHostObjectSimpleVtbl
    {
        BEGIN_INTERFACE
        
        DECLSPEC_XFGVIRT(IUnknown, QueryInterface)
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IHostObjectSimple * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        DECLSPEC_XFGVIRT(IUnknown, AddRef)
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IHostObjectSimple * This);
        
        DECLSPEC_XFGVIRT(IUnknown, Release)
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IHostObjectSimple * This);
        
        DECLSPEC_XFGVIRT(IHostObjectSimple, Call_V_V)
        HRESULT ( STDMETHODCALLTYPE *Call_V_V )( 
            IHostObjectSimple * This,
            /* [in] */ INT cmd);
        
        DECLSPEC_XFGVIRT(IHostObjectSimple, Call_S_V)
        HRESULT ( STDMETHODCALLTYPE *Call_S_V )( 
            IHostObjectSimple * This,
            /* [in] */ INT cmd,
            /* [in] */ BSTR s);
        
        DECLSPEC_XFGVIRT(IHostObjectSimple, Call_S_D)
        HRESULT ( STDMETHODCALLTYPE *Call_S_D )( 
            IHostObjectSimple * This,
            /* [in] */ INT cmd,
            /* [in] */ BSTR s,
            /* [retval][out] */ double *ret);
        
        DECLSPEC_XFGVIRT(IHostObjectSimple, Call_S_I)
        HRESULT ( STDMETHODCALLTYPE *Call_S_I )( 
            IHostObjectSimple * This,
            /* [in] */ INT cmd,
            /* [in] */ BSTR s,
            /* [retval][out] */ INT *ret);
        
        DECLSPEC_XFGVIRT(IHostObjectSimple, Call_S_S)
        HRESULT ( STDMETHODCALLTYPE *Call_S_S )( 
            IHostObjectSimple * This,
            /* [in] */ INT cmd,
            /* [in] */ BSTR s,
            /* [retval][out] */ BSTR *ret);
        
        DECLSPEC_XFGVIRT(IHostObjectSimple, Call_I_V)
        HRESULT ( STDMETHODCALLTYPE *Call_I_V )( 
            IHostObjectSimple * This,
            /* [in] */ INT cmd,
            /* [in] */ INT i);
        
        DECLSPEC_XFGVIRT(IHostObjectSimple, Call_I_D)
        HRESULT ( STDMETHODCALLTYPE *Call_I_D )( 
            IHostObjectSimple * This,
            /* [in] */ INT cmd,
            /* [in] */ INT i,
            /* [retval][out] */ double *ret);
        
        DECLSPEC_XFGVIRT(IHostObjectSimple, Call_I_I)
        HRESULT ( STDMETHODCALLTYPE *Call_I_I )( 
            IHostObjectSimple * This,
            /* [in] */ INT cmd,
            /* [in] */ INT i,
            /* [retval][out] */ INT *ret);
        
        DECLSPEC_XFGVIRT(IHostObjectSimple, Call_I_S)
        HRESULT ( STDMETHODCALLTYPE *Call_I_S )( 
            IHostObjectSimple * This,
            /* [in] */ INT cmd,
            /* [in] */ INT i,
            /* [retval][out] */ BSTR *ret);
        
        DECLSPEC_XFGVIRT(IHostObjectSimple, Call_D_V)
        HRESULT ( STDMETHODCALLTYPE *Call_D_V )( 
            IHostObjectSimple * This,
            /* [in] */ INT cmd,
            /* [in] */ double d);
        
        DECLSPEC_XFGVIRT(IHostObjectSimple, Call_D_D)
        HRESULT ( STDMETHODCALLTYPE *Call_D_D )( 
            IHostObjectSimple * This,
            /* [in] */ INT cmd,
            /* [in] */ double d,
            /* [retval][out] */ double *ret);
        
        DECLSPEC_XFGVIRT(IHostObjectSimple, Call_D_I)
        HRESULT ( STDMETHODCALLTYPE *Call_D_I )( 
            IHostObjectSimple * This,
            /* [in] */ INT cmd,
            /* [in] */ double d,
            /* [retval][out] */ INT *ret);
        
        DECLSPEC_XFGVIRT(IHostObjectSimple, Call_D_S)
        HRESULT ( STDMETHODCALLTYPE *Call_D_S )( 
            IHostObjectSimple * This,
            /* [in] */ INT cmd,
            /* [in] */ double d,
            /* [retval][out] */ BSTR *ret);
        
        DECLSPEC_XFGVIRT(IHostObjectSimple, Call_IC_V)
        HRESULT ( STDMETHODCALLTYPE *Call_IC_V )( 
            IHostObjectSimple * This,
            /* [in] */ INT cmd,
            /* [in] */ INT i,
            /* [in] */ IDispatch *dispatch);
        
        DECLSPEC_XFGVIRT(IHostObjectSimple, Call_SC_V)
        HRESULT ( STDMETHODCALLTYPE *Call_SC_V )( 
            IHostObjectSimple * This,
            /* [in] */ INT cmd,
            /* [in] */ BSTR s,
            /* [in] */ IDispatch *dispatch);
        
        DECLSPEC_XFGVIRT(IHostObjectSimple, Call_DC_V)
        HRESULT ( STDMETHODCALLTYPE *Call_DC_V )( 
            IHostObjectSimple * This,
            /* [in] */ INT cmd,
            /* [in] */ double d,
            /* [in] */ IDispatch *dispatch);
        
        END_INTERFACE
    } IHostObjectSimpleVtbl;

    interface IHostObjectSimple
    {
        CONST_VTBL struct IHostObjectSimpleVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IHostObjectSimple_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IHostObjectSimple_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IHostObjectSimple_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IHostObjectSimple_Call_V_V(This,cmd)	\
    ( (This)->lpVtbl -> Call_V_V(This,cmd) ) 

#define IHostObjectSimple_Call_S_V(This,cmd,s)	\
    ( (This)->lpVtbl -> Call_S_V(This,cmd,s) ) 

#define IHostObjectSimple_Call_S_D(This,cmd,s,ret)	\
    ( (This)->lpVtbl -> Call_S_D(This,cmd,s,ret) ) 

#define IHostObjectSimple_Call_S_I(This,cmd,s,ret)	\
    ( (This)->lpVtbl -> Call_S_I(This,cmd,s,ret) ) 

#define IHostObjectSimple_Call_S_S(This,cmd,s,ret)	\
    ( (This)->lpVtbl -> Call_S_S(This,cmd,s,ret) ) 

#define IHostObjectSimple_Call_I_V(This,cmd,i)	\
    ( (This)->lpVtbl -> Call_I_V(This,cmd,i) ) 

#define IHostObjectSimple_Call_I_D(This,cmd,i,ret)	\
    ( (This)->lpVtbl -> Call_I_D(This,cmd,i,ret) ) 

#define IHostObjectSimple_Call_I_I(This,cmd,i,ret)	\
    ( (This)->lpVtbl -> Call_I_I(This,cmd,i,ret) ) 

#define IHostObjectSimple_Call_I_S(This,cmd,i,ret)	\
    ( (This)->lpVtbl -> Call_I_S(This,cmd,i,ret) ) 

#define IHostObjectSimple_Call_D_V(This,cmd,d)	\
    ( (This)->lpVtbl -> Call_D_V(This,cmd,d) ) 

#define IHostObjectSimple_Call_D_D(This,cmd,d,ret)	\
    ( (This)->lpVtbl -> Call_D_D(This,cmd,d,ret) ) 

#define IHostObjectSimple_Call_D_I(This,cmd,d,ret)	\
    ( (This)->lpVtbl -> Call_D_I(This,cmd,d,ret) ) 

#define IHostObjectSimple_Call_D_S(This,cmd,d,ret)	\
    ( (This)->lpVtbl -> Call_D_S(This,cmd,d,ret) ) 

#define IHostObjectSimple_Call_IC_V(This,cmd,i,dispatch)	\
    ( (This)->lpVtbl -> Call_IC_V(This,cmd,i,dispatch) ) 

#define IHostObjectSimple_Call_SC_V(This,cmd,s,dispatch)	\
    ( (This)->lpVtbl -> Call_SC_V(This,cmd,s,dispatch) ) 

#define IHostObjectSimple_Call_DC_V(This,cmd,d,dispatch)	\
    ( (This)->lpVtbl -> Call_DC_V(This,cmd,d,dispatch) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IHostObjectSimple_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_HostObjectSimple;

#ifdef __cplusplus

class DECLSPEC_UUID("637abc45-11f7-4dde-84b4-317d62a638d3")
HostObjectSimple;
#endif
#endif /* __HostObjectSimpleLibrary_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


