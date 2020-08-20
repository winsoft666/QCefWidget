#pragma once
#include <QPushButton>

namespace LayoutHelper {
template<typename T>
T* newWidget(const QString& objectName) {
  T* pT = new T();
  Q_ASSERT(pT);
  if (pT) {
    pT->setObjectName(objectName);
  }

  return pT;
}

template<typename T>
T* newWidget(const QString& objectName, const QString& text) {
  T* pT = new T();
  Q_ASSERT(pT);
  if (pT) {
    pT->setObjectName(objectName);
    pT->setText(text);
  }

  return pT;
}

QPushButton* newPushButton(const QString& objectName,
                           const QString& text,
                           const QCursor& cursor) {
  QPushButton* pButton = new QPushButton();
  Q_ASSERT(pButton);
  if (pButton) {
    pButton->setObjectName(objectName);
    pButton->setText(text);
    pButton->setCursor(cursor);
  }

  return pButton;
}

QPushButton* newPushButton(const QString& objectName,
                           const QString& text,
                           bool bPointingHandCursor = true) {
  QPushButton* pButton = new QPushButton();
  Q_ASSERT(pButton);
  if (pButton) {
    pButton->setObjectName(objectName);
    pButton->setText(text);
    if (bPointingHandCursor)
      pButton->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
  }

  return pButton;
}
} // namespace LayoutHelper
