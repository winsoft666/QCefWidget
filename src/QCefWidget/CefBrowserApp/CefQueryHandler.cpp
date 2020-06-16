#include "CefQueryHandler.h"
#include "Include/QCefQuery.h"
#include "Include/QCefWidget.h"
#include "Include/QCefOpenGLWidget.h"
#include "QCefWidgetImplement.h"

CefQueryHandler::CefQueryHandler(QCefWidgetImplement *pCefViewImpl)
    : pCefImpl_(pCefViewImpl) {}

CefQueryHandler::~CefQueryHandler() {
}

bool CefQueryHandler::OnQuery(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                              int64 query_id, const CefString &request, bool persistent,
                              CefRefPtr<Callback> callback) {
  if (pCefImpl_ && pCefImpl_->getWidget()) {
    mtxCallbackMap_.lock();
    mapCallback_[query_id] = callback;
    mtxCallbackMap_.unlock();

    QString strRequest = QString::fromStdString(request.ToString());

    if (pCefImpl_->getWidgetType() == Widget) {
      QCefWidget *p = (QCefWidget *)pCefImpl_->getWidget();
      if (p)
        emit p->cefQueryRequest(QCefQuery(strRequest, query_id));
    }
    else if (pCefImpl_->getWidgetType() == OpenGLWidget) {
      QCefOpenGLWidget *p = (QCefOpenGLWidget *)pCefImpl_->getWidget();
      if (p)
        emit p->cefQueryRequest(QCefQuery(strRequest, query_id));
    }

    return true;
  }

  return false;
}

void CefQueryHandler::OnQueryCanceled(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                                      int64 query_id) {
  mtxCallbackMap_.lock();
  auto it = mapCallback_.find(query_id);
  if (it != mapCallback_.end())
    mapCallback_.erase(it);

  mtxCallbackMap_.unlock();
}

bool CefQueryHandler::Response(int64_t query, bool success, const CefString &response, int error) {
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
