#include "QCefResourceUtil.h"
#include "include/base/cef_logging.h"
#include "include/cef_stream.h"
#include "include/wrapper/cef_byte_read_handler.h"
#include "include/wrapper/cef_stream_resource_handler.h"
#include "QCefGlobalSetting.h"
#include <QDebug>
#include "Include/QCefResourceProvider.h"

namespace {
bool loadBinaryResource(int binaryId,
                        const QString& resourceType,
                        DWORD& dwSize,
                        LPBYTE& pBytes) {
  HINSTANCE hInst = GetModuleHandle(NULL);
  HRSRC hRes = FindResourceW(
    hInst, MAKEINTRESOURCE(binaryId), resourceType.toStdWString().c_str());
  if (hRes) {
    HGLOBAL hGlob = LoadResource(hInst, hRes);
    if (hGlob) {
      dwSize = SizeofResource(hInst, hRes);
      pBytes = (LPBYTE)LockResource(hGlob);
      if (dwSize > 0 && pBytes)
        return true;
    }
  }

  DWORD dwGLE = GetLastError();
  qWarning() << "Load resource failed:" << binaryId << "," << resourceType;
  return false;
}

// Provider of binary resources.
class BinaryResourceProvider : public CefResourceManager::Provider {
public:
  BinaryResourceProvider(const std::string& urlPath,
                         const std::string& resourcePathPrefix)
    : urlPath_(urlPath)
    , resourcePathPrefix_(resourcePathPrefix) {
    DCHECK(!urlPath.empty());
    if (!resourcePathPrefix_.empty() &&
        resourcePathPrefix_[resourcePathPrefix_.length() - 1] != '/') {
      resourcePathPrefix_ += "/";
    }
  }

  bool OnRequest(scoped_refptr<CefResourceManager::Request> request) OVERRIDE {
    CEF_REQUIRE_IO_THREAD();

    const std::string& url = request->url();
    if (url.find(urlPath_) != 0L) {
      // Not handled by this provider.
      return false;
    }

    CefRefPtr<CefResourceHandler> handler;

    std::string relative_path = url.substr(urlPath_.length());
    if (!relative_path.empty()) {
      if (!resourcePathPrefix_.empty())
        relative_path = resourcePathPrefix_ + relative_path;

      CefRefPtr<CefStreamReader> stream =
        GetBinaryResourceReader(relative_path.data());
      if (stream.get()) {
        handler = new CefStreamResourceHandler(
          request->mime_type_resolver().Run(url), stream);
      }
    }

    request->Continue(handler);
    return true;
  }

private:
  std::string urlPath_;
  std::string resourcePathPrefix_;

  DISALLOW_COPY_AND_ASSIGN(BinaryResourceProvider);
};


bool getResourceId(const QString& resourceName, QPair<int, QString>& rcInfo) {
  QCefGlobalSetting::initializeInstance();
  QMap<QString, QPair<int, QString>> resourceMap =
    QCefGlobalSetting::resource_map;

  if (resourceMap.contains(resourceName)) {
    rcInfo = resourceMap[resourceName];
    return true;
  }
  return false;
}
} // namespace

bool loadBinaryResource(const char* resourceName, std::string& resourceData) {
  QPair<int, QString> rcInfo;
  if (!getResourceId(resourceName, rcInfo))
    return false;

  DWORD dwSize = 0;
  LPBYTE pBytes = NULL;

  if (loadBinaryResource(rcInfo.first, rcInfo.second, dwSize, pBytes)) {
    resourceData = std::string(reinterpret_cast<char*>(pBytes), dwSize);
    return true;
  }

  NOTREACHED(); // The resource should be found.
  return false;
}

CefRefPtr<CefStreamReader> GetBinaryResourceReader(const char* resourceName) {
  QPair<int, QString> rcInfo;
  if (!getResourceId(resourceName, rcInfo))
    return NULL;

  DWORD dwSize;
  LPBYTE pBytes;

  if (loadBinaryResource(rcInfo.first, rcInfo.second, dwSize, pBytes)) {
    return CefStreamReader::CreateForHandler(
      new CefByteReadHandler(pBytes, dwSize, NULL));
  }

  NOTREACHED(); // The resource should be found.
  return NULL;
}

CefResourceManager::Provider*
CreateBinaryResourceProvider(const std::string& urlPath,
                             const std::string& resourcePathPrefix) {
  return new BinaryResourceProvider(urlPath, resourcePathPrefix);
}

bool CustomResourceProvider::OnRequest(
  scoped_refptr<CefResourceManager::Request> request) {
  CEF_REQUIRE_IO_THREAD();
  if (!provider_)
    return false;

  QString url = QString::fromStdString(request->url());
  CefRefPtr<ResourceOwner> owner = new ResourceOwner();
  if (!provider_->onRequest(url, owner->buffer))
    return false;

  CefRefPtr<CefResourceHandler> handler;

  CefRefPtr<CefStreamReader> stream = CefStreamReader::CreateForHandler(
    new CefByteReadHandler((const unsigned char*)owner->buffer.constData(),
                           (size_t)owner->buffer.size(),
                           owner));
  if (stream.get()) {
    handler = new CefStreamResourceHandler(
      request->mime_type_resolver().Run(request->url()), stream);
  }

  request->Continue(handler);
  return true;
}
