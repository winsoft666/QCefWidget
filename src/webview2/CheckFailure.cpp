#include "stdafx.h"
#include <iomanip>
#include <sstream>
#include <QString>
#include <QDebug>

// Notify the user of a failure with a message box.
void ShowFailure(HRESULT hr, const std::wstring& message) {
    std::wstringstream formattedMessage;
    formattedMessage << message << ": 0x" << std::hex << std::setw(8) << hr << " ("
                     << winrt::hresult_error(hr).message().c_str() << ")";
    qCritical() << QString::fromStdWString(formattedMessage.str());
}

// If something failed, show the error code and fail fast.
void CheckFailure(HRESULT hr, const std::wstring& message) {
    if (FAILED(hr)) {
        ShowFailure(hr, message);
    }
}

void FeatureNotAvailable() {
    qCritical() << "This feature is not available in the browser version currently being used.";
}
