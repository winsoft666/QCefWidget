#include "TestWnd.h"
#include "QCefSetting.h"
#include "QCefWidget.h"
#include "QCefOpenGLWidget.h"
#include <QDebug>
#include <QStyle>
#include <QCloseEvent>
#include "CefWnd.h"
#include "SourceCodeWidget.h"
#include "BrowserListItem.h"
#include "BuiltInUrl.h"
#include "resource.h"

#pragma execution_character_set("utf-8")

namespace {
QSize kWindowDefaultSize = QSize(900, 600);
}

TestWnd::TestWnd(QWidget *parent)
    : QWidget(parent) {
  QCefSetting::setFlashPlugin("TestResource\\pepperflash\\26.0.0.126\\pepflashplayer.dll", "26.0.0.126");
  //QCefSetting::setFlashPlugin("TestResource/pepperflash/32.0.0.387/pepflashplayer32.dll", "32.0.0.387");
  //QCefSetting::setFlashPlugin("TestResource/pepperflash/32.0.0.192/pepflashplayer.dll", "32.0.0.192");

  QCefSetting::setResourceMap({{"test.html", {IDR_TEST_PAGE, "PAGE"}}});

  setupUi();
}

TestWnd::~TestWnd() { qDebug() << "TestWnd::~TestWnd"; }

void TestWnd::closeEvent(QCloseEvent *event) { 
  qDebug() << "TestWnd::closeEvent";

  for (int i = 0; i < listBrowser_->count(); i++) {
    BrowserListItem* blt = (BrowserListItem*)listBrowser_->itemWidget(listBrowser_->item(i));
    Q_ASSERT(blt && blt->cefWnd());
    if (blt && blt->cefWnd()) {
      blt->cefWnd()->close();
    }
  }
}

QSize TestWnd::sizeHint() const { return kWindowDefaultSize; }

void TestWnd::setupUi() {
  this->setWindowTitle("QCefWidget Tester");
  this->setObjectName("QControlPanel");
  this->setWindowIcon(QIcon(":/QCefWidgetTest/images/logo.svg"));

  cefWidgetGroup_ = new QButtonGroup(this);
  radioButtonCefWidget_ = new QRadioButton("QCefWidget");
  radioButtonCefOpenGLWidget_ = new QRadioButton("QCefOpenGLWidget");
  cefWidgetGroup_->addButton(radioButtonCefWidget_);
  cefWidgetGroup_->addButton(radioButtonCefOpenGLWidget_);
  radioButtonCefWidget_->setChecked(true);

  QHBoxLayout* hlCefWidget = new QHBoxLayout();
  hlCefWidget->addWidget(radioButtonCefWidget_);
  hlCefWidget->addWidget(radioButtonCefOpenGLWidget_);
  hlCefWidget->addStretch();

  checkboxFramelessWindow_ = new QCheckBox("Frameless Window");
  checkboxFramelessWindow_->setChecked(false);

  checkboxTranslucentWindowBackground_ = new QCheckBox("Translucent Window Background");
  checkboxTranslucentWindowBackground_->setChecked(false);


  QRegExp rxColor("((25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9][0-9]|[0-9]).){3}(25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9][0-9]|[0-9])");
  QRegExpValidator *regValidatorColor = new QRegExpValidator(rxColor);

  QHBoxLayout* hlWindowBkColor = new QHBoxLayout();
  hlWindowBkColor->addWidget(new QLabel("Window Background-Color(ARGB): "));
  lineEditWindowBkColor_ = new QLineEdit("255,255,255,255");
  lineEditWindowBkColor_->setMaximumWidth(110);
  lineEditWindowBkColor_->setValidator(regValidatorColor);
  hlWindowBkColor->addWidget(lineEditWindowBkColor_);
  hlWindowBkColor->addStretch();

  QHBoxLayout* hlBrowserBkColor = new QHBoxLayout();
  hlBrowserBkColor->addWidget(new QLabel("Browser Background-Color(ARGB): "));
  lineEditBrowserBkColor_ = new QLineEdit("255,255,255,255");
  lineEditBrowserBkColor_->setMaximumWidth(110);
  lineEditBrowserBkColor_->setValidator(regValidatorColor);
  hlBrowserBkColor->addWidget(lineEditBrowserBkColor_);
  hlBrowserBkColor->addStretch();

  checkboxOsrEnabled_ = new QCheckBox("OSR(Off-screen Render)");
  checkboxOsrEnabled_->setChecked(true);

  QHBoxLayout *hlFPS = new QHBoxLayout();
  hlFPS->addWidget(new QLabel("Browser Maximum FPS(1~60): "));
  lineEditFPS_ = new QLineEdit("25");
  lineEditFPS_->setMaximumWidth(32);
  QIntValidator *intValidator = new QIntValidator(1, 60);
  lineEditFPS_->setValidator(intValidator);
  hlFPS->addWidget(lineEditFPS_);
  hlFPS->addStretch();

  QHBoxLayout *hlInitSize = new QHBoxLayout();
  hlInitSize->addWidget(new QLabel("Window Size: "));
  lineEditInitSize_ = new QLineEdit("800*600");
  lineEditInitSize_->setMaximumWidth(80);
  QRegExp rx("[0-9]+\\*[0-9]+");
  QRegExpValidator *regValidator = new QRegExpValidator(rx);
  lineEditInitSize_->setValidator(regValidator);
  hlInitSize->addWidget(lineEditInitSize_);
  hlInitSize->addStretch();

  checkboxContextMenuEnabled_ = new QCheckBox("Browser with Context Menu");
  checkboxContextMenuEnabled_->setChecked(true);

  checkboxAutoShowDevToolsContextMenu_ = new QCheckBox("Browser with DevTools Context Menu");
  checkboxAutoShowDevToolsContextMenu_->setChecked(true);

  QHBoxLayout* hlInitUrl = new QHBoxLayout();
  hlInitUrl->addWidget(new QLabel("URL: "));
  comboBoxUrl_ = new QComboBox();
  comboBoxUrl_->setObjectName("comboBoxUrl");
  comboBoxUrl_->setFixedHeight(22);
  comboBoxUrl_->setEditable(true);
  comboBoxUrl_->addItems(getBuiltInUrl());
  hlInitUrl->addWidget(comboBoxUrl_);
  hlInitUrl->addStretch();

  pushButtonNewBrowser_ = new QPushButton("New Browser");
  pushButtonNewBrowser_->setObjectName("pushButtonNewBrowser");
  pushButtonNewBrowser_->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
  connect(pushButtonNewBrowser_, &QPushButton::clicked, this, &TestWnd::onPushButtonNewBrowserClicked);

  QVBoxLayout *vlOption = new QVBoxLayout();
  vlOption->addLayout(hlCefWidget);
  vlOption->addWidget(checkboxFramelessWindow_);
  vlOption->addWidget(checkboxTranslucentWindowBackground_);
  vlOption->addWidget(checkboxOsrEnabled_);
  vlOption->addWidget(checkboxContextMenuEnabled_);
  vlOption->addWidget(checkboxAutoShowDevToolsContextMenu_);
  vlOption->addLayout(hlFPS);
  vlOption->addLayout(hlInitSize);
  vlOption->addLayout(hlWindowBkColor);
  vlOption->addLayout(hlBrowserBkColor);
  vlOption->addLayout(hlInitUrl);
  //vlOption->addStretch();

  QVBoxLayout *vlButton = new QVBoxLayout();
  //vlButton->addStretch();
  vlButton->addWidget(pushButtonNewBrowser_);

  QHBoxLayout *hlTop = new QHBoxLayout();
  hlTop->addLayout(vlOption);
  hlTop->addLayout(vlButton);

  QHBoxLayout* hlBottom = new QHBoxLayout();
  pushButtonGetSourceCode_ = new QPushButton("Get source code");
  pushButtonGetSourceCode_->setObjectName("pushButtonGetSourceCode");
  pushButtonGetSourceCode_->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
  connect(pushButtonGetSourceCode_, &QPushButton::clicked, this, &TestWnd::onPushButtonGetSourceCodeClicked);
  hlBottom->addWidget(pushButtonGetSourceCode_);
  hlBottom->addStretch();

  listBrowser_ = new QListWidget();
  listBrowser_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  QVBoxLayout *vlMain = new QVBoxLayout();
  vlMain->addLayout(hlTop);
  vlMain->addWidget(listBrowser_);
  vlMain->addLayout(hlBottom);

  this->setLayout(vlMain);

  QString qss;
  QFile qssFile(":/QCefWidgetTest/ControlPanel.qss");
  qssFile.open(QFile::ReadOnly);
  if (qssFile.isOpen()) {
    qss = QLatin1String(qssFile.readAll());
    this->setStyleSheet(qss);
    qssFile.close();
  }

  this->resize(sizeHint());
}

void TestWnd::onPushButtonNewBrowserClicked() {
  CefWnd *pCefWnd = new CefWnd();
  connect(pCefWnd, &QWidget::destroyed, this, &TestWnd::onCefWndDestroyed);
  pCefWnd->setUsingGLWidget(radioButtonCefOpenGLWidget_->isChecked());
  pCefWnd->setFramelessWindow(checkboxFramelessWindow_->isChecked());
  pCefWnd->setTranslucentWindowBackground(checkboxTranslucentWindowBackground_->isChecked());
  pCefWnd->setOsrEnabled(checkboxOsrEnabled_->isChecked());
  pCefWnd->setContextMenuEnabled(checkboxContextMenuEnabled_->isChecked());
  pCefWnd->setAutoAddDevToolsContextMenu(checkboxAutoShowDevToolsContextMenu_->isChecked());
  pCefWnd->setMaximumFps(lineEditFPS_->text().toInt());

  QColor winBkColor;
  if (stringToColor(lineEditWindowBkColor_->text(), winBkColor))
    pCefWnd->setWindowBkColor(winBkColor);

  QColor browserBkColor;
  if (stringToColor(lineEditBrowserBkColor_->text(), browserBkColor))
    pCefWnd->setBrowserBkColor(browserBkColor);

  QString strInitSize = lineEditInitSize_->text();
  if (strInitSize.contains("*")) {
    QSize initSize = {strInitSize.mid(0, strInitSize.indexOf("*")).toInt(), strInitSize.mid(strInitSize.indexOf("*") + 1).toInt()};
    pCefWnd->setInitSize(initSize);
  }

  pCefWnd->setInitUrl(comboBoxUrl_->currentText());

  BrowserListItem *blt = new BrowserListItem(pCefWnd);
  QListWidgetItem *item = new QListWidgetItem();
  item->setSizeHint(QSize(1, 32));
  listBrowser_->addItem(item);
  listBrowser_->setItemWidget(item, blt);

  pCefWnd->setupUi();
  pCefWnd->show();
}

void TestWnd::onPushButtonGetSourceCodeClicked() {
  SourceCodeWidget* pWeiXin = new SourceCodeWidget(this);
  pWeiXin->show();
}

void TestWnd::onCefWndDestroyed(QObject *obj) {
  for (int i = 0; i < listBrowser_->count(); i++) {
    BrowserListItem *blt = (BrowserListItem *)listBrowser_->itemWidget(listBrowser_->item(i));
    Q_ASSERT(blt && blt->cefWnd());
    if (blt && blt->cefWnd() == obj) {
      delete listBrowser_->takeItem(i);
    }
  }
}

bool TestWnd::stringToColor(QString s, QColor& c) {
  int lastPos = 0;
  for (int i = 0; i < 3; i++) {
    int pos = s.indexOf(",", lastPos);
    if (pos == -1)
      return false;
    QString v = s.mid(lastPos, pos - lastPos);
    if (i == 0)
      c.setAlpha(v.toInt());
    else if(i == 1)
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
