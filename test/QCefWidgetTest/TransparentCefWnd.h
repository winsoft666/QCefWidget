#ifndef TRANSPARENT_WND_H__
#define TRANSPARENT_WND_H__

#include <QtWidgets/QtWidgets>

class QCefWidget;
class QCefOpenGLWidget;
class TransparentCefWnd : public QWidget {
  Q_OBJECT

public:
  TransparentCefWnd(QWidget *parent = 0);
  ~TransparentCefWnd();
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;

protected:
  void setupUi();

private:
  QCefWidget *cefWidget_;
  QCefOpenGLWidget* cefOpenGLWidget_;

  bool leftMousePressed_;
  QPoint beginDragPos_;
};

#endif // TRANSPARENT_WND_H__
