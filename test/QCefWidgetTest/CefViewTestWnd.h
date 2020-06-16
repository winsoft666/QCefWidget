#ifndef QCEFVIEWTEST_H
#define QCEFVIEWTEST_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QtWidgets>

class QCefWidget;
class QCefOpenGLWidget;
class CefViewTestWnd : public QMainWindow {
  Q_OBJECT

public:
  CefViewTestWnd(QWidget *parent = 0);
  ~CefViewTestWnd();

  void closeEvent(QCloseEvent *event) override;
  QSize sizeHint() const override;
  void resizeEvent(QResizeEvent *event) override;
protected:
  void setupUi();
private slots:
  void onShowTransparentCefWnd();
  void onTriggerTestEvent();
  void onPushButtonApplyClicked();
private:
  QWidget* centralWidget_;
  QPushButton* pushButtonBack_;
  QPushButton* pushButtonForward_;
  QPushButton* pushButtonReload_;
  QPushButton* pushButtonTest_;
  QPushButton* pushButtonApply_;
  QMenu* menuTest_;
  QAction* actionShowTransparentCef_;
  QAction* actionTriggerTestEvent_;
  QComboBox* comboBoxUrl_;
  QPlainTextEdit* plainTextEditLog_;
  QCefWidget* cefWidget_;
  QCefOpenGLWidget* cefOpenGLWidget_;
  QSplitter* splitterMain_;
  QSplitter* splitterCef_;
  QWidget* widgetBottom_;
  QCheckBox* checkboxHookTopLevelCloseMsg_;
  QCheckBox* checkboxIgnoreTopLevelCloseMsg_;
};

#endif // QCEFVIEWTEST_H
