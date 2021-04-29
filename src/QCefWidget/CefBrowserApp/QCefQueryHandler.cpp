#include "QCefQueryHandler.h"
#include "Include/QCefQuery.h"
#include "Include/QCefWidget.h"
#include "Include/QCefOpenGLWidget.h"
#include "QCefWidgetImpl.h"

QCefQueryHandler::QCefQueryHandler(QCefWidgetImpl* pImpl) :
    pCefImpl_(pImpl) {}

QCefQueryHandler::~QCefQueryHandler() {}

bool QCefQueryHandler::OnQuery(CefRefPtr<CefBrowser> browser,
                               CefRefPtr<CefFrame> frame,
                               int64 query_id,
                               const CefString& request,
                               bool persistent,
                               CefRefPtr<Callback> callback) {
  if (pCefImpl_ && pCefImpl_->getWidget()) {
    mtxCallbackMap_.lock();
    mapCallback_[query_id] = callback;
    mtxCallbackMap_.unlock();

    QString strRequest = QString::fromStdString(request.ToString());

    if (pCefImpl_->getWidgetType() == WT_Widget) {
      QCefWidget* p = (QCefWidget*)pCefImpl_->getWidget();
      if (p)
        emit p->cefQueryRequest(QCefQuery(strRequest, query_id));
    }
#ifndef QT_NO_OPENGL
    else if (pCefImpl_->getWidgetType() == WT_OpenGLWidget) {
      QCefOpenGLWidget* p = (QCefOpenGLWidget*)pCefImpl_->getWidget();
      if (p)
        emit p->cefQueryRequest(QCefQuery(strRequest, query_id));
    }
#endif
    return true;
  }

  return false;
}

void QCefQueryHandler::OnQueryCanceled(CefRefPtr<CefBrowser> browser,
                                       CefRefPtr<CefFrame> frame,
                                       int64 query_id) {
  mtxCallbackMap_.lock();
  auto it = mapCallback_.find(query_id);
  if (it != mapCallback_.end())
    mapCallback_.erase(it);

  mtxCallbackMap_.unlock();
}

bool QCefQueryHandler::Response(int64_t query,
                                bool success,
                                const CefString& response,
                                int error) {
  CefRefPtr<Callback> cb;
  mtxCallbackMap_.lock();
  auto it = mapCallback_.find(query);
  if (it != mapCallback_.end()) {
    cb = it->second;
    mapCallback_.erase(it);
  }
  mtxCallbackMap_.unlock();

  if (!cb)
    return false;

  if (success)
    cb->Success(response);
  else
    cb->Failure(error, response);

  return true;
}
