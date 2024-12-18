#include "BrowserListItem.h"
#include "WebViewWnd.h"
#include "BuiltInUrl.h"

BrowserListItem::BrowserListItem(WebViewWnd* w, QWidget* parent) :
    QWidget(parent),
    webViewWnd_(w) {
  setupUi();

  connect(w, &WebViewWnd::webViewWndVisibleChanged, this, [this](bool visible) {
    if (visible) {
      pushButtonVisible_->setProperty("status", "visible");
    }
    else {
      pushButtonVisible_->setProperty("status", "invisible");
    }
    this->style()->unpolish(pushButtonVisible_);
    this->style()->polish(pushButtonVisible_);
  });

  comboBoxUrl_->lineEdit()->setText(webViewWnd_->initUrl());

  updateStatus();
}

BrowserListItem::~BrowserListItem() {}

WebViewWnd* BrowserListItem::webViewWnd() {
  return webViewWnd_;
}

void BrowserListItem::setupUi() {
  QHBoxLayout* hlMain = new QHBoxLayout();
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
      "QPushButton[status=\"visible\"] {border: none;image: url(:/Sample/images/visible.svg);} QPushButton[status=\"invisible\"] {border: none;image: url(:/Sample/images/invisible.svg);}");
  pushButtonVisible_->setProperty("status", "invisible");

  pushButtonInvokeJS_ = new QPushButton();
  pushButtonInvokeJS_->setFixedSize(18, 18);
  pushButtonInvokeJS_->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
  pushButtonInvokeJS_->setStyleSheet(
      "QPushButton {border: none;image: url(:/Sample/images/javascript_n.svg);} QPushButton:hover {border: none; image: url(:/Sample/images/javascript_h.svg);}");

  pushButtonDevTools_ = new QPushButton();
  pushButtonDevTools_->setFixedSize(20, 20);
  pushButtonDevTools_->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
  pushButtonDevTools_->setStyleSheet(
      "QPushButton {border: none;image: url(:/Sample/images/devtools_n.svg);} QPushButton:hover {border: none; image: url(:/Sample/images/devtools_h.svg);}");

  pushButtonClose_ = new QPushButton();
  pushButtonClose_->setFixedSize(18, 18);
  pushButtonClose_->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
  pushButtonClose_->setStyleSheet(
      "QPushButton {border: none;image: url(:/Sample/images/close_n.svg);} QPushButton:hover {border: none; image: url(:/Sample/images/close_h.svg);}");

  labelStatus_ = new QLabel();
  labelStatus_->setFixedHeight(14);
  labelStatus_->setMinimumWidth(40);
  labelStatus_->setAlignment(Qt::AlignCenter);
  labelStatus_->setStyleSheet(
      "font:normal normal 12px Arial;border:none; border-radius: 2px; color:green;");

  hlMain->addWidget(comboBoxUrl_);
  hlMain->addStretch();
  hlMain->addWidget(labelStatus_);
  hlMain->addWidget(pushButtonVisible_);
  hlMain->addWidget(pushButtonInvokeJS_);
  hlMain->addWidget(pushButtonDevTools_);
  hlMain->addWidget(pushButtonClose_);

  this->setLayout(hlMain);

  connect(pushButtonClose_, &QPushButton::clicked, webViewWnd_, &WebViewWnd::forceClose);
  connect(pushButtonDevTools_,
          &QPushButton::clicked,
          webViewWnd_,
          &WebViewWnd::onShowDevTools);
  connect(pushButtonInvokeJS_,
          &QPushButton::clicked,
          webViewWnd_,
          &WebViewWnd::onTriggerEvent);
  connect(pushButtonVisible_, &QPushButton::clicked, [this]() {
    QString strStatus = pushButtonVisible_->property("status").toString();
    if (strStatus == "invisible") {
      webViewWnd_->show();
    }
    else {
      webViewWnd_->hide();
    }
  });

  connect(comboBoxUrl_->lineEdit(), &QLineEdit::returnPressed, [this]() {
    QString url = comboBoxUrl_->lineEdit()->text();
    if (webViewWnd_)
      webViewWnd_->onNavigateToUrl(url);
  });

  connect(comboBoxUrl_,
          QOverload<const QString&>::of(&QComboBox::currentIndexChanged),
          this,
          [this](const QString& text) {
            if (webViewWnd_ && text != "None")
              webViewWnd_->onNavigateToUrl(text);
          });
}

void BrowserListItem::updateStatus() {
  QString strEngine;
  if (webViewWnd_->browserEngine() == QWebView::BrowserEngine::WebView2)
    strEngine = "WebView2";
  else if (webViewWnd_->browserEngine() == QWebView::BrowserEngine::CEF)
    strEngine = "CEF";

  QString str = QString("%1 %2 %3")
                    .arg(strEngine)
                    .arg(webViewWnd_->framelessWindow() ? "Frameless" : "Non-Frameless")
                    .arg(webViewWnd_->osrEnabled() ? "OSR" : "Non-OSR");
  labelStatus_->setText(str);
}
