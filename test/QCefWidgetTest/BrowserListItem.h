#pragma once
#include <QtWidgets>

class CefWnd;
class BrowserListItem : public QWidget {
 public:
  BrowserListItem(CefWnd* pCefWnd, QWidget* parent = nullptr);
  ~BrowserListItem();

  CefWnd* cefWnd();

 protected slots:
  void onCefWndVisibleChanged(bool bVisible);

 protected:
  void setupUi();

  QComboBox* comboBoxUrl_;
  QLabel* labelStatus_;

  QPushButton* pushButtonInvokeJS_;
  QPushButton* pushButtonDevTools_;
  QPushButton* pushButtonClose_;
  QPushButton* pushButtonVisible_;

  CefWnd* pCefWnd_;
};