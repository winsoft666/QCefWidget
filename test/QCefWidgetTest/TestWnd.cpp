#include "TestWnd.h"
#include "QCefSetting.h"
#include "QCefWidget.h"
#include "QCefOpenGLWidget.h"
#include "TransparentCefWnd.h"
#include <QDebug>
#include <QStyle>
#include <QCloseEvent>
#include "CefWnd.h"
#include "SourceCodeWidget.h"

#pragma execution_character_set("utf-8")

namespace {
QSize kWindowDefaultSize = QSize(800, 460);
}

TestWnd::TestWnd(QWidget *parent)
    : QWidget(parent) {
  QCefSetting::setFlashPlugin("TestResource\\pepperflash\\26.0.0.126\\pepflashplayer.dll", "26.0.0.126");
  //QCefSetting::setFlashPlugin("TestResource/pepperflash/32.0.0.387/pepflashplayer32.dll", "32.0.0.387");
  //QCefSetting::setFlashPlugin("TestResource/pepperflash/32.0.0.192/pepflashplayer.dll", "32.0.0.192");

  //QCefSetting::setResourceMap({{"TestPage.html", {IDR_TEST_PAGE, "PAGE"}}});

  setupUi();
}

TestWnd::~TestWnd() { qDebug() << "TestWnd::~TestWnd"; }

void TestWnd::closeEvent(QCloseEvent *event) { qDebug() << "TestWnd::closeEvent"; }

QSize TestWnd::sizeHint() const { return kWindowDefaultSize; }

void TestWnd::setupUi() {
  this->setWindowTitle("Control Panel");
  this->setObjectName("QControlPanel");
  this->setWindowIcon(QIcon(":/QCefWidgetTest/images/logo.png"));

  checkboxFramelessWindow_ = new QCheckBox("Frameless Window");
  checkboxFramelessWindow_->setChecked(false);

  checkboxTranslucentWindowBackground_ = new QCheckBox("Translucent Window Background");
  checkboxTranslucentWindowBackground_->setChecked(false);

  QHBoxLayout* hlWindow = new QHBoxLayout();
  hlWindow->addWidget(checkboxFramelessWindow_);
  hlWindow->addWidget(checkboxTranslucentWindowBackground_);
  hlWindow->addStretch();

  QRegExp rxColor("((25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9][0-9]|[0-9]).){3}(25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9][0-9]|[0-9])");
  QRegExpValidator *regValidatorColor = new QRegExpValidator(rxColor);

  QHBoxLayout* hlBkColor = new QHBoxLayout();
  hlBkColor->addWidget(new QLabel("Window Background-Color(ARGB): "));
  lineEditWindowBkColor_ = new QLineEdit("255,255,255,255");
  lineEditWindowBkColor_->setValidator(regValidatorColor);
  hlBkColor->addWidget(lineEditWindowBkColor_);
  hlBkColor->addSpacing(20);

  hlBkColor->addWidget(new QLabel("Browser Background-Color(ARGB): "));
  lineEditBrowserBkColor_ = new QLineEdit("255,255,255,255");
  lineEditBrowserBkColor_->setValidator(regValidatorColor);
  hlBkColor->addWidget(lineEditBrowserBkColor_);
  hlBkColor->addStretch();

  checkboxOsrEnabled_ = new QCheckBox("OSR(Off-screen Render)");
  checkboxOsrEnabled_->setChecked(true);

  QHBoxLayout *hlFPS = new QHBoxLayout();
  hlFPS->addWidget(new QLabel("Maximum FPS: "));
  lineEditFPS_ = new QLineEdit("25");
  lineEditFPS_->setMaximumWidth(32);
  QIntValidator *intValidator = new QIntValidator(1, 60);
  lineEditFPS_->setValidator(intValidator);
  hlFPS->addWidget(lineEditFPS_);
  hlFPS->addStretch();

  QHBoxLayout *hlInitSize = new QHBoxLayout();
  hlInitSize->addWidget(new QLabel("Initialize Size: "));
  lineEditInitSize_ = new QLineEdit("800*600");
  lineEditInitSize_->setMaximumWidth(100);
  QRegExp rx("[0-9]+\\*[0-9]+");
  QRegExpValidator *regValidator = new QRegExpValidator(rx);
  lineEditInitSize_->setValidator(regValidator);
  hlInitSize->addWidget(lineEditInitSize_);
  hlInitSize->addStretch();

  checkboxContextMenuEnabled_ = new QCheckBox("Context Menu");
  checkboxContextMenuEnabled_->setChecked(true);

  checkboxAutoShowDevToolsContextMenu_ = new QCheckBox("DevTools Context Menu");
  checkboxAutoShowDevToolsContextMenu_->setChecked(true);

  QHBoxLayout *hlContextMenu = new QHBoxLayout();
  hlContextMenu->addWidget(checkboxContextMenuEnabled_);
  hlContextMenu->addWidget(checkboxAutoShowDevToolsContextMenu_);
  hlContextMenu->addStretch();

  comboBoxUrl_ = new QComboBox();
  comboBoxUrl_->setObjectName("comboBoxUrl");
  comboBoxUrl_->setFixedHeight(22);
  comboBoxUrl_->setEditable(true);
  comboBoxUrl_->addItems(QStringList() << "about:blank"
                                       << "chrome://version"
                                       << "http://qcefwidget/TestPage.html" << QString("file:///%1").arg(QCoreApplication::applicationDirPath() + u8"/TestResource/TestPage.html")
                                       << QString("file:///%1").arg(QCoreApplication::applicationDirPath() + u8"/TestResource/FlashPlayerTest.html")
                                       << QString("file:///%1").arg(QCoreApplication::applicationDirPath() + u8"/TestResource/Tree.html")
                                       << QString("file:///%1").arg(QCoreApplication::applicationDirPath() + u8"/TestResource/PDF.html")
                                       << QString("file:///%1").arg(QCoreApplication::applicationDirPath() + u8"/TestResource/OsrTest.html")
                                       << QString("file:///%1").arg(QCoreApplication::applicationDirPath() + u8"/TestResource/Clock.html") << "https://html5test.com/"
                                       << "https://www.bing.com"
                                       << "https://www.google.com"
                                       << "https://map.baidu.com/"
                                       << "https://ant.design/components/overview/");

  pushButtonNewBrowser_ = new QPushButton("New Browser");
  connect(pushButtonNewBrowser_, &QPushButton::clicked, this, &TestWnd::onPushButtonNewBrowserClicked);

  QVBoxLayout *vlOption = new QVBoxLayout();
  vlOption->addLayout(hlWindow);
  vlOption->addLayout(hlBkColor);
  vlOption->addWidget(checkboxOsrEnabled_);
  vlOption->addLayout(hlContextMenu);
  vlOption->addLayout(hlFPS);
  vlOption->addLayout(hlInitSize);
  vlOption->addWidget(comboBoxUrl_);
  vlOption->addStretch();

  QVBoxLayout *vlButton = new QVBoxLayout();
  vlButton->addWidget(pushButtonNewBrowser_);
  vlButton->addStretch();

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

  pCefWnd->setupUi();
  pCefWnd->show();
}

void TestWnd::onPushButtonGetSourceCodeClicked() {
  SourceCodeWidget* pWeiXin = new SourceCodeWidget(this);
  pWeiXin->show();
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
