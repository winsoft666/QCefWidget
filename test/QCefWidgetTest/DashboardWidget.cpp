#include "DashboardWidget.h"
#include <QPainter>

QDashboardWidget::QDashboardWidget(QWidget *parent)
    : QWidget(parent)
    , maxValue_(100)
    , minValue_(0)
    , curValue_(0)
    , startAngle_(150)
    , endAngle_(30)
    , dividingStep_(1)
    , biggerDividingMulriple_(5)
    , biggestDividingMulriple_(10) {}

QDashboardWidget::~QDashboardWidget() {}

void QDashboardWidget::setValuePrefix(const QString &prefix) {
  valuePrefix_ = prefix;

  update();
}

QString QDashboardWidget::valuePrefix() const { return valuePrefix_; }

void QDashboardWidget::setValueSuffix(const QString &suffix) {
  valueSuffix_ = suffix;

  update();
}

QString QDashboardWidget::valueSuffix() const { return valueSuffix_; }

void QDashboardWidget::setMaxValue(int maxValue) {
  maxValue_ = maxValue;

  update();
}

int QDashboardWidget::maxValue() const { return maxValue_; }

void QDashboardWidget::setMinValue(int minValue) {
  minValue_ = minValue;

  update();
}

int QDashboardWidget::minValue() const { return minValue_; }

void QDashboardWidget::setValue(int value) {
  curValue_ = value;
  update();
}

int QDashboardWidget::value() const { return curValue_; }

void QDashboardWidget::setDividingStep(int step) { dividingStep_ = step; }

int QDashboardWidget::dividingStep() const { return dividingStep_; }

void QDashboardWidget::setBiggerDividingMulriple(int mulriple) { biggerDividingMulriple_ = mulriple; }

int QDashboardWidget::biggerDividingMulriple() const { return biggerDividingMulriple_; }

void QDashboardWidget::setBiggestDividingMulriple(int mulriple) { biggestDividingMulriple_ = mulriple; }

int QDashboardWidget::biggestDividingMulriple() const { return biggestDividingMulriple_; }

void QDashboardWidget::setDividingAngleOffset(int startAngle, int endAngle) {
  startAngle_ = startAngle;
  endAngle_ = endAngle;

  update();
}

int QDashboardWidget::dividingStartAngleOffset() const { return startAngle_; }

int QDashboardWidget::dividingEndAngleOffset() const { return endAngle_; }

void QDashboardWidget::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  int refSize = qMin(width(), height());
  int radius = refSize / 2;

  painter.translate(radius, radius);

  drawFrame(painter, radius, refSize);
  drawDividing(painter, radius, refSize);
  drawDividingNumber(painter, radius, refSize);
  drawNumberValue(painter, radius, refSize);
  drawNeedle(painter, radius, refSize);
}

void QDashboardWidget::drawFrame(QPainter &painter, int radius, int refSize) {
  painter.save();
  painter.setPen(Qt::NoPen);
  QLinearGradient lg1(-radius, -radius, radius, radius);
  lg1.setColorAt(0.1, Qt::white);
  lg1.setColorAt(1, Qt::black);
  painter.setBrush(lg1);
  painter.drawEllipse(QPoint(0, 0), radius, radius);

  painter.setBrush(Qt::black);
  painter.drawEllipse(QPoint(0, 0), radius - 10, radius - 10);

  painter.restore();
}

void QDashboardWidget::drawDividing(QPainter &painter, int radius, int refSize) {
  painter.save();
  painter.rotate(startAngle_); //将坐标系顺时针旋转，到达起始位置

  QPen pen(Qt::white);
  painter.setPen(pen);
#if 1
  int step = (maxValue_ - minValue_) / dividingStep_;
  double angleStep = (360.0 - (startAngle_ - endAngle_)) / step;

  for (int i = minValue_; i <= maxValue_; i += dividingStep_) {
    if (i >= maxValue_ * 0.8) {
      pen.setColor(Qt::red);
      painter.setPen(pen);
    }

    if (biggestDividingMulriple_ != 0 && (i % (biggestDividingMulriple_ * dividingStep_) == 0)) {
      pen.setWidth(2);
      painter.setPen(pen);
      painter.drawLine(radius - 12, 0, radius - 12 - 13, 0);
    }
    else if (biggerDividingMulriple_ != 0 &&  (i % (biggerDividingMulriple_ * dividingStep_) == 0)) {
      pen.setWidth(1);
      painter.setPen(pen);
      painter.drawLine(radius - 12, 0, radius - 12 - 10, 0);
    }
    else if (i % dividingStep_ == 0) {
      pen.setWidth(0);
      painter.setPen(pen);
      painter.drawLine(radius - 14, 0, radius - 14 - 5, 0);
    }
    painter.rotate(angleStep);
  }
#endif
  painter.restore();
}

void QDashboardWidget::drawDividingNumber(QPainter &painter, int radius, int refSize) {
  painter.save();

  painter.setPen(Qt::white);

  double x, y;
  double angle, angleArc;
  double w, h;
  QFontMetricsF fm(this->font());

  double anglePerVel = (360.f - (startAngle_ - endAngle_)) / (maxValue_ - minValue_);
  double indicatorRadius = radius - 12 - 13 - 12;

  for (int i = minValue_; i <= maxValue_; i += 20) //每隔20Km设置一个数字
  {
    if (i % 20 == 0) {
      angle = 360.f - (startAngle_ + (i - minValue_) * anglePerVel); //角度
      angleArc = angle * 3.14f / 180.f;                              //转换为弧度

      x = indicatorRadius * cos(angleArc);
      y = -indicatorRadius * sin(angleArc); // 取负是因为Qt坐标系Y轴和数学坐标系Y轴方向相反

      QString speed = QString::number(i);

      w = fm.width(speed);
      h = fm.height();
      painter.drawText(QPointF(x - w / 2.f, y + h / 4.f), speed);
    }
  }

  painter.restore();
}

void QDashboardWidget::drawNumberValue(QPainter &painter, int radius, int refSize) {
  painter.save();
  painter.setPen(Qt::white);
  QString speed = QString("%1%2%3").arg(valuePrefix_).arg(curValue_).arg(valueSuffix_);
  QFontMetricsF fm(this->font());
  qreal w = fm.size(Qt::TextSingleLine, speed).width();
  painter.drawText(-w / 2, -20, speed);

  painter.restore();
}

void QDashboardWidget::drawNeedle(QPainter &painter, int radius, int refSize) {
  painter.save();
  //绘制指针
  double anglePerVel = (360.f - (startAngle_ - endAngle_)) / (maxValue_ - minValue_);
  double curAngle = startAngle_ + curValue_ * anglePerVel;
  painter.rotate(curAngle); //旋转坐标系

  QRadialGradient haloGradient(0, 0, 60, 0, 0); //辐射渐变
  haloGradient.setColorAt(0, QColor(60, 60, 60));
  haloGradient.setColorAt(1, QColor(160, 160, 160)); //灰
  painter.setPen(Qt::white);                         //定义线条文本颜色  设置线条的颜色
  painter.setBrush(haloGradient);                    //刷子定义形状如何填满 填充后的颜色

  static const QPointF points[3] = {
      QPointF(0.0, 2),
      QPointF(0.0, -2),
      QPointF(70.0, 0),
  };
  painter.drawPolygon(points, 3); //绘制指针
  painter.restore();

  painter.save();
  //绘制旋转中心
  QRadialGradient rg(0, 0, 10);
  rg.setColorAt(0.0, Qt::darkGray);
  rg.setColorAt(0.5, Qt::white);
  rg.setColorAt(1.0, Qt::darkGray);
  painter.setPen(Qt::NoPen);
  painter.setBrush(rg);
  painter.drawEllipse(QPoint(0, 0), 5, 5);

  painter.restore();
}
