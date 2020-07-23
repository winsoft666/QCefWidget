#ifndef TEST_WND_H__
#define TEST_WND_H__

#include <QtWidgets/QtWidgets>

class TestWnd : public QWidget {
  Q_OBJECT
public:
  TestWnd(QWidget *parent = 0);
  ~TestWnd();
protected:
  void setupUi();
  void closeEvent(QCloseEvent *event) override;
  QSize sizeHint() const override;
private slots:
  void onPushButtonNewBrowserClicked();
  void onPushButtonGetSourceCodeClicked();
private:
  bool stringToColor(QString s, QColor& c);
private:
  QLineEdit* lineEditInitSize_;
  QLineEdit* lineEditFPS_;
  QLineEdit* lineEditWindowBkColor_;
  QLineEdit* lineEditBrowserBkColor_;
  QComboBox* comboBoxUrl_;
  QCheckBox* checkboxFramelessWindow_;
  QCheckBox* checkboxTranslucentWindowBackground_;
  QCheckBox* checkboxOsrEnabled_;
  QCheckBox* checkboxContextMenuEnabled_;
  QCheckBox* checkboxAutoShowDevToolsContextMenu_;

  QPushButton* pushButtonNewBrowser_;

  QListWidget* listBrowser_;

  QPushButton* pushButtonGetSourceCode_;
};

#endif // TEST_WND_H__
