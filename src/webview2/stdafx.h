#ifndef STDAFX_H_
#define STDAFX_H_
#if defined(_MSVC_) && _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers.
#include <windows.h>

#include <string>
#include <vector>
#include <map>
#include <functional>
#include "wil/com.h"
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <wrl.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include "CheckFailure.h"

#pragma execution_character_set("utf-8")

#endif