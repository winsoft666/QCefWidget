#pragma once
#include <include/cef_v8.h>
#include <list>
#include <map>

#include "QCefProtocol.h"
#include "../../../QCefWidget/Include/QCefVersion.h"

class QCefClient : public CefBaseRefCounted {
  // class Accessor
  //	: public CefV8Accessor
  //{
  // public:
  //	virtual bool Get(const CefString& name,
  //		const CefRefPtr<CefV8Value> object,
  //		CefRefPtr<CefV8Value>& retval,
  //		CefString& exception) override;

  //	virtual bool Set(const CefString& name,
  //		const CefRefPtr<CefV8Value> object,
  //		const CefRefPtr<CefV8Value> value,
  //		CefString& exception) override;

  // private:
  //	IMPLEMENT_REFCOUNTING(Accessor);
  //};

  typedef struct _EventListener {
    CefRefPtr<CefV8Value> callback_;
    CefRefPtr<CefV8Context> context_;
  } EventListener;

  typedef std::list<EventListener> EventListenerList;

  typedef std::map<CefString, std::list<EventListener>> EventListenerListMap;

  class V8Handler : public CefV8Handler {
   public:
    V8Handler(CefRefPtr<CefBrowser> browser,
              CefRefPtr<CefFrame> frame,
              QCefClient::EventListenerListMap& eventListenerListMap);

    virtual bool Execute(const CefString& function,
                         CefRefPtr<CefV8Value> object,
                         const CefV8ValueList& arguments,
                         CefRefPtr<CefV8Value>& retval,
                         CefString& exception) override;

   protected:
    void ExecuteInvokeMethod(const CefString& function,
                             CefRefPtr<CefV8Value> object,
                             const CefV8ValueList& arguments,
                             CefRefPtr<CefV8Value>& retval,
                             CefString& exception);

    void ExecuteAddEventListener(const CefString& function,
                                 CefRefPtr<CefV8Value> object,
                                 const CefV8ValueList& arguments,
                                 CefRefPtr<CefV8Value>& retval,
                                 CefString& exception);

    void ExecuteRemoveEventListener(const CefString& function,
                                    CefRefPtr<CefV8Value> object,
                                    const CefV8ValueList& arguments,
                                    CefRefPtr<CefV8Value>& retval,
                                    CefString& exception);

   private:
    CefRefPtr<CefBrowser> browser_;
    CefRefPtr<CefFrame> frame_;
    QCefClient::EventListenerListMap& eventListenerListMap_;

   private:
    IMPLEMENT_REFCOUNTING(V8Handler);
  };

 public:
  QCefClient(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame);

  CefRefPtr<CefV8Value> GetObject();

  void ExecuteEventListener(const CefString eventName, CefRefPtr<CefDictionaryValue> dict);

 private:
  CefRefPtr<CefV8Value> object_;

  CefRefPtr<CefBrowser> browser_;

  CefRefPtr<CefFrame> frame_;

  EventListenerListMap eventListenerListMap_;

 private:
  IMPLEMENT_REFCOUNTING(QCefClient);
};
