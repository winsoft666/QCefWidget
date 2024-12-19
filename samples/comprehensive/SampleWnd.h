#ifndef TEST_WND_H__
#define TEST_WND_H__

#pragma execution_character_set("utf-8")

#include <QtWidgets/QtWidgets>

class SampleWnd : public QWidget {
  Q_OBJECT
 public:
  SampleWnd(QWidget* parent = 0);
  ~SampleWnd();

 protected:
  void setupUi();
  void closeEvent(QCloseEvent* event) override;
  QSize sizeHint() const override;
 private slots:
  void onPushButtonNewBrowserClicked();
  void onPushButtonQuickSettingForIrregularWndClicked();
  void onPushButtonQuickSettingForElectronClicked();
  void onWebViewWndClosed();

 private:
  bool stringToColor(QString s, QColor& c);

 private:
  bool exitFlag_ = false;

  QComboBox* comboBoxEngine_;
  QLineEdit* lineEditInitSize_;
  QLineEdit* lineEditFPS_;
  QLineEdit* lineEditWindowBkColor_;
  QLineEdit* lineEditBrowserBkColor_;
  QComboBox* comboBoxUrl_;
  QCheckBox* checkboxInitHide_;
  QCheckBox* checkboxFramelessWindow_;
  QCheckBox* checkboxTranslucentWindowBackground_;
  QCheckBox* checkboxOsrEnabled_;
  QCheckBox* checkboxUsingHideInsteadCloseWhenClickCloseButton_;
  QCheckBox* checkboxAllowExecuteUnknownProtocolViaOS_;

  QPushButton* pushButtonClose_;
  QPushButton* pushButtonNewBrowser_;
  QPushButton* pushButtonQuickSettingForIrregularWnd_;
  QPushButton* pushButtonQuickSettingForElectron_;

  QListWidget* listBrowser_;

  QLabel* labelVersion_;
};

#endif  // TEST_WND_H__
