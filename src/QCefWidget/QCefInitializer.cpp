#include "Include/QCefInitializer.h"
#include "QCefManager.h"

void InitializeQCef() {
  QCefManager::getInstance().initializeCef();
}

void UnInitializeQCef() {
  QCefManager::getInstance().uninitializeCef();
}
