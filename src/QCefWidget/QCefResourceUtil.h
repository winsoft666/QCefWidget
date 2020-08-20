#ifndef QCEF_RESOURCE_UTIL_H_
#define QCEF_RESOURCE_UTIL_H_
#pragma once
#include <string>
#include "include/cef_image.h"
#include "include/cef_stream.h"
#include "include/wrapper/cef_resource_manager.h"
#include <QByteArray>

// Retrieve a resource as a steam reader.
CefRefPtr<CefStreamReader> GetBinaryResourceReader(const char* resourceName);

// Create a new provider for loading binary resources.
CefResourceManager::Provider*
CreateBinaryResourceProvider(const std::string& urlPath,
                             const std::string& resourcePathPrefix);

class QCefResourceProvider;
class CustomResourceProvider : public CefResourceManager::Provider {
public:
  CustomResourceProvider(QCefResourceProvider* provider)
    : provider_(provider) {}

  bool OnRequest(scoped_refptr<CefResourceManager::Request> request) OVERRIDE;

private:
  QCefResourceProvider* provider_;

  DISALLOW_COPY_AND_ASSIGN(CustomResourceProvider);
};

class ResourceOwner : public CefBaseRefCounted {
public:
  QByteArray buffer;

private:
  IMPLEMENT_REFCOUNTING(ResourceOwner);
};

#endif // !QCEF_RESOURCE_UTIL_H_