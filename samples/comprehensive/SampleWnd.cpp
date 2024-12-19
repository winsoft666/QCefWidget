#include "SampleWnd.h"
#include "QWebView/Core.h"
#include <QDebug>
#include <QStyle>
#include <QCloseEvent>
#include "WebViewWnd.h"
#include "BrowserListItem.h"
#include "LayoutHelper.hpp"
#include "BuiltInUrl.h"
#include "resource.h"
#include "QWebView/Core.h"
#include "QWebView/Manager.h"

#pragma execution_character_set("utf-8")

namespace {
QSize kWindowDefaultSize = QSize(900, 600);
}

SampleWnd::SampleWnd(QWidget* parent) :
    QWidget(parent) {
  //QCefSetting::setFlashPlugin(
  //    "TestResource\\pepperflash\\26.0.0.126\\pepflashplayer.dll", "26.0.0.126");

  //QCefSetting::setResourceMap({{"test.html", {IDR_TEST_PAGE, "PAGE"}},
  //                             {"tree.html", {IDR_TREE_PAGE, "PAGE"}},
  //                             {"javascript_h.svg", {IDR_SVG1, "SVG"}},
  //                             {"test.swf", {IDR_SWF1, "SWF"}}});

  setupUi();

  connect(QWebViewManager::Get(), &QWebViewManager::allWebViewsClosed, [this]() {
    if (exitFlag_)
      qApp->quit();
  });
}

SampleWnd::~SampleWnd() {
}

void SampleWnd::closeEvent(QCloseEvent* event) {
  if (listBrowser_->count() > 0) {
    exitFlag_ = true;

    for (int i = 0; i < listBrowser_->count(); i++) {
      BrowserListItem* blt = (BrowserListItem*)listBrowser_->itemWidget(listBrowser_->item(i));
      Q_ASSERT(blt && blt->webViewWnd());
      if (blt && blt->webViewWnd()) {
        blt->webViewWnd()->forceClose();
      }
    }
    event->ignore();
  }
  else {
    event->accept();
    qApp->quit();
  }
}

QSize SampleWnd::sizeHint() const {
  return kWindowDefaultSize;
}

void SampleWnd::setupUi() {
  using namespace LayoutHelper;
  this->setWindowTitle("QWebView Sample");
  this->setObjectName("QControlPanel");
  this->setWindowIcon(QIcon(":/Sample/images/logo.svg"));

  QHBoxLayout* hlEngine = new QHBoxLayout();
  hlEngine->addWidget(new QLabel("浏览器引擎: "));
  comboBoxEngine_ = new QComboBox();
  comboBoxEngine_->setMinimumWidth(80);
  comboBoxEngine_->addItem("WebView2", (int)QWebView::BrowserEngine::WebView2);
  comboBoxEngine_->addItem("CEF", (int)QWebView::BrowserEngine::CEF);
  hlEngine->addWidget(comboBoxEngine_);
  hlEngine->addStretch();

  checkboxFramelessWindow_ = new QCheckBox("无边框窗口");

  checkboxTranslucentWindowBackground_ = new QCheckBox("窗口背景透明");

  QRegExp rxColor(
      "((25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9][0-9]|[0-9]).){3}(25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9][0-9]|[0-9])");
  QRegExpValidator* regValidatorColor = new QRegExpValidator(rxColor);

  QHBoxLayout* hlWindowBkColor = new QHBoxLayout();
  hlWindowBkColor->addWidget(new QLabel("窗口背景色(ARGB): "));
  lineEditWindowBkColor_ = new QLineEdit("255,255,255,255");
  lineEditWindowBkColor_->setMaximumWidth(110);
  lineEditWindowBkColor_->setValidator(regValidatorColor);
  hlWindowBkColor->addWidget(lineEditWindowBkColor_);
  hlWindowBkColor->addStretch();

  QHBoxLayout* hlBrowserBkColor = new QHBoxLayout();
  hlBrowserBkColor->addWidget(new QLabel("浏览器背景色(ARGB): "));
  lineEditBrowserBkColor_ = new QLineEdit("255,255,255,255");
  lineEditBrowserBkColor_->setMaximumWidth(110);
  lineEditBrowserBkColor_->setValidator(regValidatorColor);
  hlBrowserBkColor->addWidget(lineEditBrowserBkColor_);
  hlBrowserBkColor->addStretch();

  checkboxOsrEnabled_ = new QCheckBox("离屏渲染(OSR)");

  checkboxInitHide_ = new QCheckBox("创建隐藏的窗口");

  QHBoxLayout* hlFPS = new QHBoxLayout();
  hlFPS->addWidget(new QLabel("最大FPS (1 ~ 60，仅OSR模式下可用): "));
  lineEditFPS_ = new QLineEdit("25");
  lineEditFPS_->setMaximumWidth(32);
  QIntValidator* intValidator = new QIntValidator(1, 60);
  lineEditFPS_->setValidator(intValidator);
  hlFPS->addWidget(lineEditFPS_);
  hlFPS->addStretch();

  QHBoxLayout* hlInitSize = new QHBoxLayout();
  hlInitSize->addWidget(new QLabel("窗口尺寸: "));
  lineEditInitSize_ = new QLineEdit("800*600");
  lineEditInitSize_->setMaximumWidth(80);
  QRegExp rx("[0-9]+\\*[0-9]+");
  QRegExpValidator* regValidator = new QRegExpValidator(rx);
  lineEditInitSize_->setValidator(regValidator);
  hlInitSize->addWidget(lineEditInitSize_);
  hlInitSize->addStretch();

  checkboxUsingHideInsteadCloseWhenClickCloseButton_ = new QCheckBox("点击窗口关闭按钮时，不关闭窗口，而是隐藏窗口");

  checkboxAllowExecuteUnknownProtocolViaOS_ = new QCheckBox("Allow Execute Unknown Protocol via OS");
  checkboxAllowExecuteUnknownProtocolViaOS_->setChecked(true);

  QHBoxLayout* hlInitUrl = new QHBoxLayout();
  hlInitUrl->addWidget(new QLabel("初始链接: "));
  comboBoxUrl_ = new QComboBox();
  comboBoxUrl_->setObjectName("comboBoxUrl");
  comboBoxUrl_->setMinimumWidth(300);
  comboBoxUrl_->setEditable(true);
  comboBoxUrl_->addItems(getBuiltInUrl());
  hlInitUrl->addWidget(comboBoxUrl_);
  hlInitUrl->addStretch();

  pushButtonNewBrowser_ = newPushButton("pushButtonNewBrowser", "使用上面配置创建新的浏览器窗口");
  connect(pushButtonNewBrowser_, &QPushButton::clicked, this, &SampleWnd::onPushButtonNewBrowserClicked);

  QHBoxLayout* hlQuickSetting = new QHBoxLayout();
  hlQuickSetting->addWidget(new QLabel("快速设置: "));
  pushButtonQuickSettingForIrregularWnd_ = newPushButton("", "不规则窗口");
  connect(pushButtonQuickSettingForIrregularWnd_, &QPushButton::clicked, this, &SampleWnd::onPushButtonQuickSettingForIrregularWndClicked);
  hlQuickSetting->addWidget(pushButtonQuickSettingForIrregularWnd_);

  pushButtonQuickSettingForElectron_ = newPushButton("", "模拟Electron");
  connect(pushButtonQuickSettingForElectron_, &QPushButton::clicked, this, &SampleWnd::onPushButtonQuickSettingForElectronClicked);
  hlQuickSetting->addWidget(pushButtonQuickSettingForElectron_);
  hlQuickSetting->addStretch();

  QVBoxLayout* vlOption = new QVBoxLayout();
  vlOption->addLayout(hlEngine);
  vlOption->addWidget(checkboxInitHide_);
  vlOption->addWidget(checkboxFramelessWindow_);
  vlOption->addWidget(checkboxTranslucentWindowBackground_);
  vlOption->addWidget(checkboxOsrEnabled_);
  vlOption->addWidget(checkboxUsingHideInsteadCloseWhenClickCloseButton_);
  vlOption->addWidget(checkboxAllowExecuteUnknownProtocolViaOS_);
  vlOption->addLayout(hlFPS);
  vlOption->addLayout(hlInitSize);
  vlOption->addLayout(hlWindowBkColor);
  vlOption->addLayout(hlBrowserBkColor);
  vlOption->addLayout(hlInitUrl);
  vlOption->addLayout(hlQuickSetting);
  vlOption->addWidget(pushButtonNewBrowser_);

  QHBoxLayout* hlBottom = new QHBoxLayout();
  labelVersion_ = new QLabel();
  //labelVersion_->setText(QString("QCefWidget: %1  CEF: %2").arg(QCefWidget::QCefWidgetVersion()).arg(QCefWidget::CefVersion()));
  hlBottom->addWidget(labelVersion_);
  hlBottom->addStretch();
  pushButtonClose_ = newPushButton("", "调用QWidget::close()方法");
  connect(pushButtonClose_, &QPushButton::clicked, [this]() { close(); });
  hlBottom->addWidget(pushButtonClose_);

  listBrowser_ = new QListWidget();
  listBrowser_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  QVBoxLayout* vlMain = new QVBoxLayout();
  vlMain->addLayout(vlOption);
  vlMain->addWidget(listBrowser_);
  vlMain->addLayout(hlBottom);

  this->setLayout(vlMain);

  QString qss;
  QFile qssFile(":/Sample/ControlPanel.qss");
  qssFile.open(QFile::ReadOnly);
  if (qssFile.isOpen()) {
    qss = QString::fromUtf8(qssFile.readAll());
    this->setStyleSheet(qss);
    qssFile.close();
  }

  this->resize(sizeHint());
}

void SampleWnd::onPushButtonNewBrowserClicked() {
  WebViewWnd* wmd = new WebViewWnd(
      checkboxFramelessWindow_->isChecked(),
      checkboxTranslucentWindowBackground_->isChecked(),
      (QWebView::BrowserEngine)comboBoxEngine_->currentData().toInt());
  connect(wmd, &WebViewWnd::webViewWndClosed, this, &SampleWnd::onWebViewWndClosed);
  wmd->setOsrEnabled(checkboxOsrEnabled_->isChecked());
  wmd->setUsingHideInsteadClose(checkboxUsingHideInsteadCloseWhenClickCloseButton_->isChecked());
  wmd->setAllowExecuteUnknownProtocolViaOS(checkboxAllowExecuteUnknownProtocolViaOS_->isChecked());
  wmd->setMaximumFps(lineEditFPS_->text().toInt());

  QColor winBkColor;
  if (stringToColor(lineEditWindowBkColor_->text(), winBkColor))
    wmd->setWindowBkColor(winBkColor);

  QColor browserBkColor;
  if (stringToColor(lineEditBrowserBkColor_->text(), browserBkColor))
    wmd->setBrowserBkColor(browserBkColor);

  QString strInitSize = lineEditInitSize_->text();
  if (strInitSize.contains("*")) {
    QSize initSize = {strInitSize.mid(0, strInitSize.indexOf("*")).toInt(),
                      strInitSize.mid(strInitSize.indexOf("*") + 1).toInt()};
    wmd->setInitSize(initSize);
  }

  wmd->setInitUrl(comboBoxUrl_->currentText());

  BrowserListItem* blt = new BrowserListItem(wmd);
  QListWidgetItem* item = new QListWidgetItem();
  item->setSizeHint(QSize(1, 32));
  listBrowser_->addItem(item);
  listBrowser_->setItemWidget(item, blt);

  wmd->setupUi();

  if (!checkboxInitHide_->isChecked())
    wmd->show();
}

void SampleWnd::onPushButtonQuickSettingForIrregularWndClicked() {
  checkboxOsrEnabled_->setChecked(true);
  checkboxFramelessWindow_->setChecked(true);
  checkboxTranslucentWindowBackground_->setChecked(true);
  QString old = lineEditWindowBkColor_->text();
  QString newSetting = "0.255.255.255";
  if (old.indexOf(",") != -1) {
    newSetting = "0" + old.mid(old.indexOf(","));
  }
  lineEditWindowBkColor_->setText(newSetting);

  old = lineEditBrowserBkColor_->text();
  newSetting = "0.255.255.255";
  if (old.indexOf(",") != -1) {
    newSetting = "0" + old.mid(old.indexOf(","));
  }
  lineEditBrowserBkColor_->setText(newSetting);

  comboBoxUrl_->lineEdit()->setText("http://qcefwidget/tree.html");
}

void SampleWnd::onPushButtonQuickSettingForElectronClicked() {
  checkboxFramelessWindow_->setChecked(true);
  comboBoxUrl_->lineEdit()->setText("http://qcefwidget/test.html");
}

void SampleWnd::onWebViewWndClosed() {
  WebViewWnd* wnd = (WebViewWnd*)sender();
  for (int i = 0; i < listBrowser_->count(); i++) {
    BrowserListItem* blt =
        (BrowserListItem*)listBrowser_->itemWidget(listBrowser_->item(i));
    Q_ASSERT(blt && blt->webViewWnd());
    if (blt && blt->webViewWnd() == wnd) {
      delete listBrowser_->takeItem(i);
    }
  }
}

bool SampleWnd::stringToColor(QString s, QColor& c) {
  int lastPos = 0;
  for (int i = 0; i < 3; i++) {
    int pos = s.indexOf(",", lastPos);
    if (pos == -1)
      return false;
    QString v = s.mid(lastPos, pos - lastPos);
    if (i == 0)
      c.setAlpha(v.toInt());
    else if (i == 1)
      c.setRed(v.toInt());
    else if (i == 2) {
      c.setGreen(v.toInt());

      v = s.mid(pos + 1);
      c.setBlue(v.toInt());
    }
    lastPos = pos + 1;
  }
  return true;
}
