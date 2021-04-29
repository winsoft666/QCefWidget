#pragma once
#include <QWidget>

class QDashboardWidget : public QWidget {
  Q_OBJECT
 public:
  QDashboardWidget(QWidget* parent = nullptr);
  virtual ~QDashboardWidget();

  void setValuePrefix(const QString& prefix);
  QString valuePrefix() const;

  void setValueSuffix(const QString& suffix);
  QString valueSuffix() const;

  void setMaxValue(int maxValue);
  int maxValue() const;

  void setMinValue(int minValue);
  int minValue() const;

  void setValue(int value);
  int value() const;

  void setDividingStep(int step);
  int dividingStep() const;

  void setBiggerDividingMulriple(int mulriple);
  int biggerDividingMulriple() const;

  void setBiggestDividingMulriple(int mulriple);
  int biggestDividingMulriple() const;

  void setDividingAngleOffset(int startAngle, int endAngle);
  int dividingStartAngleOffset() const;
  int dividingEndAngleOffset() const;

  void setFrameWidth(int w);
  int frameWidth() const;

  void setFrameBackground(QBrush& brush);
  QBrush frameBackground() const;

  void setPlateBackground(QBrush& brush);
  QBrush plateBackground() const;

 protected:
  void paintEvent(QPaintEvent* event) override;
  void drawFrame(QPainter& painter, int radius, int refSize);
  void drawDividing(QPainter& painter, int radius, int refSize);

  void drawDividingNumber(QPainter& painter, int radius, int refSize);
  void drawNumberValue(QPainter& painter, int radius, int refSize);

  void drawNeedle(QPainter& painter, int radius, int refSize);

 protected:
  int maxValue_;
  int minValue_;
  int curValue_;

  int frameWidth_;
  QBrush frameBackground_;

  QBrush plateBackground_;

  int startAngle_;
  int endAngle_;

  int dividingStep_;
  int biggerDividingMulriple_;
  int biggestDividingMulriple_;

  QString valuePrefix_;
  QString valueSuffix_;
};