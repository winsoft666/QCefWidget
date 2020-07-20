#pragma once
#include <string>
#include "include/cef_image.h"
#include "include/cef_stream.h"
#include "include/wrapper/cef_resource_manager.h"

// Retrieve a resource as a steam reader.
CefRefPtr<CefStreamReader> GetBinaryResourceReader(const char *resource_name);

// Create a new provider for loading binary resources.
CefResourceManager::Provider *CreateBinaryResourceProvider(const std::string &url_path, const std::string &resource_path_prefix);