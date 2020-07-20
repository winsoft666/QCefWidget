#ifndef QCEF_RESOURCE_UTIL_H_
#define QCEF_RESOURCE_UTIL_H_
#pragma once
#include <string>
#include "include/cef_image.h"
#include "include/cef_stream.h"
#include "include/wrapper/cef_resource_manager.h"

// Retrieve a resource as a steam reader.
CefRefPtr<CefStreamReader> GetBinaryResourceReader(const char *resourceName);

// Create a new provider for loading binary resources.
CefResourceManager::Provider *CreateBinaryResourceProvider(const std::string &urlPath, const std::string &resourcePathPrefix);

#endif // !QCEF_RESOURCE_UTIL_H_