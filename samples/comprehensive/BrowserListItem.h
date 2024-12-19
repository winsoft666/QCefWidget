#pragma once
#include <QtWidgets>

#pragma execution_character_set("utf-8")

class WebViewWnd;
class BrowserListItem : public QWidget {
  Q_OBJECT
 public:
  BrowserListItem(WebViewWnd* w, QWidget* parent = nullptr);
  ~BrowserListItem();

  WebViewWnd* webViewWnd();

 protected:
  void setupUi();
  void updateStatus();

  QComboBox* comboBoxUrl_;
  QLabel* labelStatus_;

  QPushButton* pushButtonNotifyJS_;
  QPushButton* pushButtonDevTools_;
  QPushButton* pushButtonClose_;
  QPushButton* pushButtonVisible_;

  WebViewWnd* webViewWnd_;
};