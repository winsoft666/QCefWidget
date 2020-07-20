#pragma once
#include <QWidget>
#include "FramelessWindow.hpp"

class QCefWidget;
class FramelessCEF : public FramelessWindow<QWidget> {
public:
  FramelessCEF(QWidget* parent = nullptr);
  ~FramelessCEF();

protected:
  void setupUi();

  QCefWidget* pCefWidget_;
};