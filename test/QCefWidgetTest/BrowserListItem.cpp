#include "BrowserListItem.h"
#include "CefWnd.h"
#include "BuiltInUrl.h"

BrowserListItem::BrowserListItem(CefWnd *pCefWnd, QWidget *parent)
    : QWidget(parent)
    , pCefWnd_(pCefWnd) {
  setupUi();

  connect(pCefWnd, &CefWnd::cefWndVisibleChange, this, &BrowserListItem::onCefWndVisibleChanged);

  comboBoxUrl_->lineEdit()->setText(pCefWnd_->initUrl());
}

BrowserListItem::~BrowserListItem() {}

CefWnd *BrowserListItem::cefWnd() {
  return pCefWnd_;
}

void BrowserListItem::onCefWndVisibleChanged(bool bVisible) {
  if (bVisible) {
    pushButtonVisible_->setProperty("status", "visible");
  }
  else {
    pushButtonVisible_->setProperty("status", "invisible");
  }
  this->style()->unpolish(pushButtonVisible_);
  this->style()->polish(pushButtonVisible_);
}

void BrowserListItem::setupUi() {
  QHBoxLayout *hlMain = new QHBoxLayout();
  hlMain->setContentsMargins(2, 2, 2, 2);

  comboBoxUrl_ = new QComboBox();
  comboBoxUrl_->setObjectName("comboBoxUrl");
  comboBoxUrl_->setFixedHeight(22);
  comboBoxUrl_->setEditable(true);
  comboBoxUrl_->addItems(getBuiltInUrl());
  comboBoxUrl_->setStyleSheet("font:normal normal 12px Arial;");

  pushButtonVisible_ = new QPushButton();
  pushButtonVisible_->setFixedSize(20, 20);
  pushButtonVisible_->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
  pushButtonVisible_->setStyleSheet(
      "QPushButton[status=\"visible\"] {border: none;image: url(:/QCefWidgetTest/images/visible.svg);} QPushButton[status=\"invisible\"] {border: none;image: url(:/QCefWidgetTest/images/invisible.svg);}");
  pushButtonVisible_->setProperty("status", "invisible");

  pushButtonInvokeJS_ = new QPushButton();
  pushButtonInvokeJS_->setFixedSize(18, 18);
  pushButtonInvokeJS_->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
  pushButtonInvokeJS_->setStyleSheet(
      "QPushButton {border: none;image: url(:/QCefWidgetTest/images/javascript_n.svg);} QPushButton:hover {border: none; image: url(:/QCefWidgetTest/images/javascript_h.svg);}");


  pushButtonDevTools_ = new QPushButton();
  pushButtonDevTools_->setFixedSize(20, 20);
  pushButtonDevTools_->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
  pushButtonDevTools_->setStyleSheet(
      "QPushButton {border: none;image: url(:/QCefWidgetTest/images/devtools_n.svg);} QPushButton:hover {border: none; image: url(:/QCefWidgetTest/images/devtools_h.svg);}");


  pushButtonClose_ = new QPushButton();
  pushButtonClose_->setFixedSize(18, 18);
  pushButtonClose_->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
  pushButtonClose_->setStyleSheet(
      "QPushButton {border: none;image: url(:/QCefWidgetTest/images/close_n.svg);} QPushButton:hover {border: none; image: url(:/QCefWidgetTest/images/close_h.svg);}");

  labelStatus_ = new QLabel("Running");
  labelStatus_->setFixedHeight(14);
  labelStatus_->setMaximumWidth(40);
  labelStatus_->setAlignment(Qt::AlignCenter);
  labelStatus_->setStyleSheet("font:normal normal 8px Arial;border:none; border-radius: 2px; color:white; background-color:green;");

  hlMain->addWidget(comboBoxUrl_);
  hlMain->addWidget(labelStatus_);
  hlMain->addWidget(pushButtonVisible_);
  hlMain->addWidget(pushButtonInvokeJS_);
  hlMain->addWidget(pushButtonDevTools_);
  hlMain->addWidget(pushButtonClose_);

  this->setLayout(hlMain);

  connect(pushButtonClose_, &QPushButton::clicked, pCefWnd_, &CefWnd::close);
  connect(pushButtonDevTools_, &QPushButton::clicked, pCefWnd_, &CefWnd::onShowDevTools);
  connect(pushButtonInvokeJS_, &QPushButton::clicked, pCefWnd_, &CefWnd::onTriggerEvent);
  connect(pushButtonVisible_, &QPushButton::clicked, [this]() {
    QString strStatus = pushButtonVisible_->property("status").toString();
    if (strStatus == "invisible") {
      pCefWnd_->show();
    }
    else {
      pCefWnd_->hide();
    }
  });

  connect(comboBoxUrl_->lineEdit(), &QLineEdit::returnPressed, [this]() {
    QString url = comboBoxUrl_->lineEdit()->text();
    if (pCefWnd_)
      pCefWnd_->onNavigateToUrl(url);
  });

  connect(comboBoxUrl_, QOverload<const QString &>::of(&QComboBox::currentIndexChanged), this, [this](const QString &text) {
    if (pCefWnd_)
      pCefWnd_->onNavigateToUrl(text);
  });
}
