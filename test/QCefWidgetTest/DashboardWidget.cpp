#include "DashboardWidget.h"
#include <QPainter>

namespace {
int kBiggestDividingWidth = 13;
int kBiggerDividingWidth = 10;
int kDividingWidth = 5;
}  // namespace

QDashboardWidget::QDashboardWidget(QWidget* parent) :
    QWidget(parent),
    maxValue_(100),
    minValue_(0),
    curValue_(0),
    startAngle_(150),
    endAngle_(30),
    dividingStep_(1),
    frameWidth_(10),
    biggerDividingMulriple_(5),
    biggestDividingMulriple_(10) {}

QDashboardWidget::~QDashboardWidget() {}

void QDashboardWidget::setValuePrefix(const QString& prefix) {
  valuePrefix_ = prefix;

  update();
}

QString QDashboardWidget::valuePrefix() const {
  return valuePrefix_;
}

void QDashboardWidget::setValueSuffix(const QString& suffix) {
  valueSuffix_ = suffix;

  update();
}

QString QDashboardWidget::valueSuffix() const {
  return valueSuffix_;
}

void QDashboardWidget::setMaxValue(int maxValue) {
  if (maxValue <= minValue_)
    return;
  maxValue_ = maxValue;

  update();
}

int QDashboardWidget::maxValue() const {
  return maxValue_;
}

void QDashboardWidget::setMinValue(int minValue) {
  if (minValue >= maxValue_)
    return;
  minValue_ = minValue;

  update();
}

int QDashboardWidget::minValue() const {
  return minValue_;
}

void QDashboardWidget::setValue(int value) {
  if (value > maxValue_)
    value = maxValue_;
  if (value < minValue_)
    value = minValue_;
  if (curValue_ != value) {
    curValue_ = value;
    update();
  }
}

int QDashboardWidget::value() const {
  return curValue_;
}

void QDashboardWidget::setDividingStep(int step) {
  if (dividingStep_ != step && step > 0) {
    dividingStep_ = step;
    update();
  }
}

int QDashboardWidget::dividingStep() const {
  return dividingStep_;
}

void QDashboardWidget::setBiggerDividingMulriple(int mulriple) {
  if (biggerDividingMulriple_ != mulriple) {
    biggerDividingMulriple_ = mulriple;
    update();
  }
}

int QDashboardWidget::biggerDividingMulriple() const {
  return biggerDividingMulriple_;
}

void QDashboardWidget::setBiggestDividingMulriple(int mulriple) {
  if (biggestDividingMulriple_ != mulriple) {
    biggestDividingMulriple_ = mulriple;
    update();
  }
}

int QDashboardWidget::biggestDividingMulriple() const {
  return biggestDividingMulriple_;
}

void QDashboardWidget::setDividingAngleOffset(int startAngle, int endAngle) {
  startAngle_ = startAngle;
  endAngle_ = endAngle;

  update();
}

int QDashboardWidget::dividingStartAngleOffset() const {
  return startAngle_;
}

int QDashboardWidget::dividingEndAngleOffset() const {
  return endAngle_;
}

void QDashboardWidget::setFrameWidth(int w) {
  if (frameWidth_ != w && w >= 0) {
    frameWidth_ = w;

    update();
  }
}

int QDashboardWidget::frameWidth() const {
  return frameWidth_;
}

void QDashboardWidget::setFrameBackground(QBrush& brush) {
  frameBackground_ = brush;

  update();
}

QBrush QDashboardWidget::frameBackground() const {
  return frameBackground_;
}

void QDashboardWidget::setPlateBackground(QBrush& brush) {
  plateBackground_ = brush;
}

QBrush QDashboardWidget::plateBackground() const {
  return plateBackground_;
}

void QDashboardWidget::paintEvent(QPaintEvent* event) {
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

void QDashboardWidget::drawFrame(QPainter& painter, int radius, int refSize) {
  Q_ASSERT(frameWidth_ < radius);

  if (frameWidth_ > 0) {
    painter.save();
    painter.setPen(Qt::NoPen);

    if (frameBackground_.style() == Qt::NoBrush) {
      QLinearGradient lg1(-radius, -radius, radius, radius);
      lg1.setColorAt(0.1, Qt::white);
      lg1.setColorAt(1, Qt::black);
      painter.setBrush(lg1);
    }
    else {
      painter.setBrush(frameBackground_);
    }
    painter.drawEllipse(QPoint(0, 0), radius, radius);

    if (plateBackground_.style() == Qt::NoBrush) {
      painter.setBrush(Qt::black);
    }
    else {
      painter.setBrush(plateBackground_);
    }
    painter.drawEllipse(
        QPoint(0, 0), radius - frameWidth_, radius - frameWidth_);

    painter.restore();
  }
}

void QDashboardWidget::drawDividing(QPainter& painter,
                                    int radius,
                                    int refSize) {
  painter.save();
  painter.rotate(startAngle_);  //将坐标系顺时针旋转，到达起始位置

  QPen pen(Qt::white);
  painter.setPen(pen);

  Q_ASSERT((maxValue_ - minValue_) % dividingStep_ == 0);

  int step = (maxValue_ - minValue_) / dividingStep_;
  double angleStep = (360.0 - (startAngle_ - endAngle_)) / step;

  int newRadius = radius - frameWidth_ - 2;

  for (int i = minValue_; i <= maxValue_; i += dividingStep_) {
    if (i >= maxValue_ * 0.8) {
      pen.setColor(Qt::red);
      painter.setPen(pen);
    }

    if (biggestDividingMulriple_ != 0 &&
        (i % (biggestDividingMulriple_ * dividingStep_) == 0)) {
      pen.setWidth(2);
      painter.setPen(pen);
      painter.drawLine(newRadius, 0, newRadius - kBiggestDividingWidth, 0);
    }
    else if (biggerDividingMulriple_ != 0 &&
             (i % (biggerDividingMulriple_ * dividingStep_) == 0)) {
      pen.setWidth(1);
      painter.setPen(pen);
      painter.drawLine(newRadius, 0, newRadius - kBiggerDividingWidth, 0);
    }
    else if (i % dividingStep_ == 0) {
      pen.setWidth(0);
      painter.setPen(pen);
      painter.drawLine(newRadius - 2, 0, newRadius - 2 - kDividingWidth, 0);
    }
    painter.rotate(angleStep);
  }

  painter.restore();
}

void QDashboardWidget::drawDividingNumber(QPainter& painter,
                                          int radius,
                                          int refSize) {
  painter.save();

  painter.setPen(Qt::white);

  double x, y;
  double angle, angleArc;
  double w, h;
  QFontMetricsF fm(this->font());

  double anglePerVel =
      (360.f - (startAngle_ - endAngle_)) / (maxValue_ - minValue_);
  double indicatorRadius =
      radius - frameWidth_ - 2 - kBiggestDividingWidth - 12;

  for (int i = minValue_; i <= maxValue_; i += dividingStep_) {
    if (biggestDividingMulriple_ != 0) {
      if (i % (biggestDividingMulriple_ * dividingStep_) == 0) {
        angle = 360.f - (startAngle_ + (i - minValue_) * anglePerVel);  //角度
        angleArc = angle * 3.14f / 180.f;                               //转换为弧度

        x = indicatorRadius * cos(angleArc);
        y = -indicatorRadius *
            sin(angleArc);  // 取负是因为Qt坐标系Y轴和数学坐标系Y轴方向相反

        QString speed = QString::number(i);

        w = fm.width(speed);
        h = fm.height();
        painter.drawText(QPointF(x - w / 2.f, y + h / 4.f), speed);
      }
    }
    else if (biggerDividingMulriple_ != 0) {
      if (i % (biggerDividingMulriple_ * dividingStep_) == 0) {
        angle = 360.f - (startAngle_ + (i - minValue_) * anglePerVel);  //角度
        angleArc = angle * 3.14f / 180.f;                               //转换为弧度

        x = indicatorRadius * cos(angleArc);
        y = -indicatorRadius *
            sin(angleArc);  // 取负是因为Qt坐标系Y轴和数学坐标系Y轴方向相反

        QString speed = QString::number(i);

        w = fm.width(speed);
        h = fm.height();
        painter.drawText(QPointF(x - w / 2.f, y + h / 4.f), speed);
      }
    }
  }

  painter.restore();
}

void QDashboardWidget::drawNumberValue(QPainter& painter,
                                       int radius,
                                       int refSize) {
  painter.save();
  painter.setPen(Qt::white);
  QString speed =
      QString("%1%2%3").arg(valuePrefix_).arg(curValue_).arg(valueSuffix_);
  QFontMetricsF fm(this->font());
  qreal w = fm.size(Qt::TextSingleLine, speed).width();
  painter.drawText(-w / 2, -20, speed);

  painter.restore();
}

void QDashboardWidget::drawNeedle(QPainter& painter, int radius, int refSize) {
  painter.save();

  double anglePerVel =
      (360.f - (startAngle_ - endAngle_)) / (maxValue_ - minValue_);
  double curAngle = startAngle_ + curValue_ * anglePerVel;
  painter.rotate(curAngle);  //旋转坐标系

  QRadialGradient haloGradient(0, 0, 60, 0, 0);  //辐射渐变
  haloGradient.setColorAt(0, QColor(60, 60, 60));
  haloGradient.setColorAt(1, QColor(160, 160, 160));  //灰
  painter.setPen(Qt::white);                          //定义线条文本颜色  设置线条的颜色
  painter.setBrush(haloGradient);                     //刷子定义形状如何填满 填充后的颜色

  static const QPointF points[3] = {
      QPointF(0.0, 2),
      QPointF(0.0, -2),
      QPointF(70.0, 0),
  };
  painter.drawPolygon(points, 3);  //绘制指针
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
