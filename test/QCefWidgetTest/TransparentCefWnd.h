#ifndef TRANSPARENT_WND_H__
#define TRANSPARENT_WND_H__

#include <QtWidgets/QtWidgets>

class QCefWidget;
class QCefOpenGLWidget;
class TransparentCefWnd : public QWidget {
  Q_OBJECT

public:
  TransparentCefWnd(bool bCefOpenGLWidget, QWidget *parent = 0);
  ~TransparentCefWnd();

  void navigateToUrl(const QString &url);
  void showDevTools();
  void closeDevTools();
protected:
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void setupUi();

private:
  QCefWidget *cefWidget_;
  QCefOpenGLWidget* cefOpenGLWidget_;

  bool leftMousePressed_;
  QPoint beginDragPos_;

  bool bCefOpenGLWidget_;
};

#endif // TRANSPARENT_WND_H__
