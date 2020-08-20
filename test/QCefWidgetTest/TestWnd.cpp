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
#include "DashboardWidget.h"
#include "LayoutHelper.hpp"
#include "BuiltInUrl.h"
#include "resource.h"

#pragma execution_character_set("utf-8")

namespace {
QSize kWindowDefaultSize = QSize(920, 680);
}

TestWnd::TestWnd(QWidget* parent)
  : QWidget(parent)
  , query_(nullptr)
  , mainProcessCpuCounter_(nullptr)
  , mainProcessMemCounter_(nullptr)
  , renderProcessCpuCounter_(nullptr)
  , renderProcessMemCounter_(nullptr) {
  QCefSetting::setFlashPlugin(
    "TestResource\\pepperflash\\26.0.0.126\\pepflashplayer.dll", "26.0.0.126");

  QCefSetting::setResourceMap({{"test.html", {IDR_TEST_PAGE, "PAGE"}},
                               {"tree.html", {IDR_TREE_PAGE, "PAGE"}},
                               {"test.swf", {IDR_SWF1, "SWF"}}});

  setupUi();
  systemPerformanceMonitor();

  qDebug() << "devicePixelRatioF:" << this->devicePixelRatioF();
  QScreen* pScreen =
    QGuiApplication::screenAt(this->mapToGlobal({this->width() / 2, 0}));
  qDebug() << "devicePixelRatio:" << pScreen->devicePixelRatio();
}

TestWnd::~TestWnd() {
  PdhCloseQuery(query_);
  qDebug() << "TestWnd::~TestWnd";
}

void TestWnd::closeEvent(QCloseEvent* event) {
  qDebug() << "TestWnd::closeEvent";

  for (int i = 0; i < listBrowser_->count(); i++) {
    BrowserListItem* blt =
      (BrowserListItem*)listBrowser_->itemWidget(listBrowser_->item(i));
    Q_ASSERT(blt && blt->cefWnd());
    if (blt && blt->cefWnd()) {
      blt->cefWnd()->close();
    }
  }
}

QSize TestWnd::sizeHint() const {
  return kWindowDefaultSize;
}

void TestWnd::setupUi() {
  using namespace LayoutHelper;
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

  checkboxTranslucentWindowBackground_ =
    new QCheckBox("Translucent Window Background");
  checkboxTranslucentWindowBackground_->setChecked(false);

  checkboxPerformanceMonitor_ = new QCheckBox("Run Performance Monitor");
  checkboxPerformanceMonitor_->setChecked(true);

  connect(checkboxPerformanceMonitor_, &QCheckBox::stateChanged, this, [this](int state) {
    if (state == Qt::CheckState::Checked) {
      if (!performanceTimer_.isActive())
        performanceTimer_.start();
    }
    else {
      if (performanceTimer_.isActive())
        performanceTimer_.stop();
    }
  });

  QRegExp rxColor(
    "((25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9][0-9]|[0-9]).){3}(25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9][0-9]|[0-9])");
  QRegExpValidator* regValidatorColor = new QRegExpValidator(rxColor);

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
  checkboxOsrEnabled_->setChecked(false);

  checkboxInitHide_ = new QCheckBox("Hidden");
  checkboxInitHide_->setChecked(false);

  QHBoxLayout* hlFPS = new QHBoxLayout();
  hlFPS->addWidget(new QLabel("Browser Maximum FPS(1~60): "));
  lineEditFPS_ = new QLineEdit("25");
  lineEditFPS_->setMaximumWidth(32);
  QIntValidator* intValidator = new QIntValidator(1, 60);
  lineEditFPS_->setValidator(intValidator);
  hlFPS->addWidget(lineEditFPS_);
  hlFPS->addStretch();

  QHBoxLayout* hlInitSize = new QHBoxLayout();
  hlInitSize->addWidget(new QLabel("Window Size: "));
  lineEditInitSize_ = new QLineEdit("800*600");
  lineEditInitSize_->setMaximumWidth(80);
  QRegExp rx("[0-9]+\\*[0-9]+");
  QRegExpValidator* regValidator = new QRegExpValidator(rx);
  lineEditInitSize_->setValidator(regValidator);
  hlInitSize->addWidget(lineEditInitSize_);
  hlInitSize->addStretch();

  checkboxContextMenuEnabled_ = new QCheckBox("Browser with Context Menu");
  checkboxContextMenuEnabled_->setChecked(true);

  checkboxAutoShowDevToolsContextMenu_ =
    new QCheckBox("Browser with DevTools Context Menu");
  checkboxAutoShowDevToolsContextMenu_->setChecked(true);

  checkboxAllowExecuteUnknownProtocolViaOS_ =
    new QCheckBox("Allow Execute Unknown Protocol via OS");
  checkboxAllowExecuteUnknownProtocolViaOS_->setChecked(true);

  QHBoxLayout* hlInitUrl = new QHBoxLayout();
  hlInitUrl->addWidget(new QLabel("URL: "));
  comboBoxUrl_ = new QComboBox();
  comboBoxUrl_->setObjectName("comboBoxUrl");
  comboBoxUrl_->setFixedHeight(22);
  comboBoxUrl_->setMaximumWidth(300);
  comboBoxUrl_->setEditable(true);
  comboBoxUrl_->addItems(getBuiltInUrl());
  hlInitUrl->addWidget(comboBoxUrl_);
  hlInitUrl->addStretch();

  pushButtonNewBrowser_ =
    newPushButton("pushButtonNewBrowser", "New Browser with Options");
  connect(pushButtonNewBrowser_,
          &QPushButton::clicked,
          this,
          &TestWnd::onPushButtonNewBrowserClicked);

  QHBoxLayout* hlQuickSetting = new QHBoxLayout();
  hlQuickSetting->addWidget(new QLabel("Quick Setting: "));
  pushButtonQuickSettingForIrregularWnd_ =
    newPushButton("", "Irregular Window");
  connect(pushButtonQuickSettingForIrregularWnd_,
          &QPushButton::clicked,
          this,
          &TestWnd::onPushButtonQuickSettingForIrregularWndClicked);
  hlQuickSetting->addWidget(pushButtonQuickSettingForIrregularWnd_);

  pushButtonQuickSettingForElectron_ = newPushButton("", "Simulate Electron");
  connect(pushButtonQuickSettingForElectron_,
          &QPushButton::clicked,
          this,
          &TestWnd::onPushButtonQuickSettingForElectronClicked);
  hlQuickSetting->addWidget(pushButtonQuickSettingForElectron_);
  hlQuickSetting->addStretch();

  QVBoxLayout* vlOption = new QVBoxLayout();
  vlOption->addLayout(hlCefWidget);
  vlOption->addWidget(checkboxInitHide_);
  vlOption->addWidget(checkboxFramelessWindow_);
  vlOption->addWidget(checkboxTranslucentWindowBackground_);
  vlOption->addWidget(checkboxOsrEnabled_);
  vlOption->addWidget(checkboxContextMenuEnabled_);
  vlOption->addWidget(checkboxAutoShowDevToolsContextMenu_);
  vlOption->addWidget(checkboxAllowExecuteUnknownProtocolViaOS_);
  vlOption->addWidget(checkboxPerformanceMonitor_);
  vlOption->addLayout(hlFPS);
  vlOption->addLayout(hlInitSize);
  vlOption->addLayout(hlWindowBkColor);
  vlOption->addLayout(hlBrowserBkColor);
  vlOption->addLayout(hlInitUrl);
  vlOption->addLayout(hlQuickSetting);
  //vlOption->addStretch();

  QHBoxLayout* hlMainProcDashboard = new QHBoxLayout();
  QLabel* labelMainProcDashboard = new QLabel(
    QString("Main Process").split(" ", QString::SkipEmptyParts).join("\n"));
  labelMainProcDashboard->setFixedSize(60, 120);
  labelMainProcDashboard->setWordWrap(true);
  labelMainProcDashboard->setAlignment(Qt::AlignCenter);
  labelMainProcDashboard->setFont(QFont("Arial", 10, -1, true));
  hlMainProcDashboard->addWidget(labelMainProcDashboard);
  dashboardWidgetMainProcCPU_ = new QDashboardWidget();
  dashboardWidgetMainProcCPU_->setFixedSize(160, 160);
  dashboardWidgetMainProcCPU_->setFont(QFont("Arial", 6));
  dashboardWidgetMainProcCPU_->setDividingAngleOffset(120, 60);
  dashboardWidgetMainProcCPU_->setValuePrefix("CPU: ");
  dashboardWidgetMainProcCPU_->setValueSuffix("%");
  hlMainProcDashboard->addWidget(dashboardWidgetMainProcCPU_);
  dashboardWidgetMainProcMemory_ = new QDashboardWidget();
  dashboardWidgetMainProcMemory_->setFixedSize(160, 160);
  dashboardWidgetMainProcMemory_->setFont(QFont("Arial", 6));
  dashboardWidgetMainProcMemory_->setDividingAngleOffset(120, 60);
  dashboardWidgetMainProcMemory_->setValuePrefix("Mem: ");
  dashboardWidgetMainProcMemory_->setValueSuffix("MB");
  dashboardWidgetMainProcMemory_->setMinValue(0);
  dashboardWidgetMainProcMemory_->setMaxValue(1000);
  dashboardWidgetMainProcMemory_->setDividingStep(5);
  dashboardWidgetMainProcMemory_->setBiggerDividingMulriple(10);
  dashboardWidgetMainProcMemory_->setBiggestDividingMulriple(20);
  hlMainProcDashboard->addWidget(dashboardWidgetMainProcMemory_);

  QHBoxLayout* hlRenderProcDashboard = new QHBoxLayout();
  QLabel* labelRenderProcDashboard = new QLabel(
    QString("Render Process").split(" ", QString::SkipEmptyParts).join("\n"));
  labelRenderProcDashboard->setFixedSize(60, 120);
  labelRenderProcDashboard->setWordWrap(true);
  labelRenderProcDashboard->setAlignment(Qt::AlignCenter);
  labelRenderProcDashboard->setFont(QFont("Arial", 10, -1, true));
  hlRenderProcDashboard->addWidget(labelRenderProcDashboard);
  dashboardWidgetRenderProcCPU_ = new QDashboardWidget();
  dashboardWidgetRenderProcCPU_->setFixedSize(160, 160);
  dashboardWidgetRenderProcCPU_->setFont(QFont("Arial", 6));
  dashboardWidgetRenderProcCPU_->setDividingAngleOffset(120, 60);
  dashboardWidgetRenderProcCPU_->setValuePrefix("CPU: ");
  dashboardWidgetRenderProcCPU_->setValueSuffix("%");
  hlRenderProcDashboard->addWidget(dashboardWidgetRenderProcCPU_);
  dashboardWidgetRenderProcMemory_ = new QDashboardWidget();
  dashboardWidgetRenderProcMemory_->setDividingAngleOffset(120, 60);
  dashboardWidgetRenderProcMemory_->setFixedSize(160, 160);
  dashboardWidgetRenderProcMemory_->setFont(QFont("Arial", 6));
  dashboardWidgetRenderProcMemory_->setValuePrefix("Mem: ");
  dashboardWidgetRenderProcMemory_->setValueSuffix("MB");
  dashboardWidgetRenderProcMemory_->setMinValue(0);
  dashboardWidgetRenderProcMemory_->setMaxValue(1000);
  dashboardWidgetRenderProcMemory_->setDividingStep(5);
  dashboardWidgetRenderProcMemory_->setBiggerDividingMulriple(10);
  dashboardWidgetRenderProcMemory_->setBiggestDividingMulriple(20);
  hlRenderProcDashboard->addWidget(dashboardWidgetRenderProcMemory_);

  QVBoxLayout* vlButton = new QVBoxLayout();
  vlButton->addLayout(hlMainProcDashboard);
  vlButton->addLayout(hlRenderProcDashboard);
  vlButton->addWidget(pushButtonNewBrowser_);

  QHBoxLayout* hlTop = new QHBoxLayout();
  hlTop->addLayout(vlOption);
  hlTop->addLayout(vlButton);

  QHBoxLayout* hlBottom = new QHBoxLayout();
  pushButtonGetSourceCode_ =
    newPushButton("pushButtonGetSourceCode", "Get source code");
  connect(pushButtonGetSourceCode_,
          &QPushButton::clicked,
          this,
          &TestWnd::onPushButtonGetSourceCodeClicked);
  hlBottom->addWidget(pushButtonGetSourceCode_);
  hlBottom->addStretch();
  pushButtonClose_ = newPushButton("", "Close");
  pushButtonClose_->setToolTip("simulate exit with call close()");
  connect(pushButtonClose_, &QPushButton::clicked, [this]() { close(); });
  hlBottom->addWidget(pushButtonClose_);

  listBrowser_ = new QListWidget();
  listBrowser_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  QVBoxLayout* vlMain = new QVBoxLayout();
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
  CefWnd* pCefWnd = new CefWnd();
  connect(pCefWnd, &QWidget::destroyed, this, &TestWnd::onCefWndDestroyed);
  pCefWnd->setUsingGLWidget(radioButtonCefOpenGLWidget_->isChecked());
  pCefWnd->setFramelessWindow(checkboxFramelessWindow_->isChecked());
  pCefWnd->setTranslucentWindowBackground(
    checkboxTranslucentWindowBackground_->isChecked());
  pCefWnd->setOsrEnabled(checkboxOsrEnabled_->isChecked());
  pCefWnd->setContextMenuEnabled(checkboxContextMenuEnabled_->isChecked());
  pCefWnd->setAutoAddDevToolsContextMenu(
    checkboxAutoShowDevToolsContextMenu_->isChecked());
  pCefWnd->setAllowExecuteUnknownProtocolViaOS(
    checkboxAllowExecuteUnknownProtocolViaOS_->isChecked());
  pCefWnd->setMaximumFps(lineEditFPS_->text().toInt());

  QColor winBkColor;
  if (stringToColor(lineEditWindowBkColor_->text(), winBkColor))
    pCefWnd->setWindowBkColor(winBkColor);

  QColor browserBkColor;
  if (stringToColor(lineEditBrowserBkColor_->text(), browserBkColor))
    pCefWnd->setBrowserBkColor(browserBkColor);

  QString strInitSize = lineEditInitSize_->text();
  if (strInitSize.contains("*")) {
    QSize initSize = {strInitSize.mid(0, strInitSize.indexOf("*")).toInt(),
                      strInitSize.mid(strInitSize.indexOf("*") + 1).toInt()};
    pCefWnd->setInitSize(initSize);
  }

  pCefWnd->setInitUrl(comboBoxUrl_->currentText());

  BrowserListItem* blt = new BrowserListItem(pCefWnd);
  QListWidgetItem* item = new QListWidgetItem();
  item->setSizeHint(QSize(1, 32));
  listBrowser_->addItem(item);
  listBrowser_->setItemWidget(item, blt);

  pCefWnd->setupUi();

  if (!checkboxInitHide_->isChecked())
    pCefWnd->show();
}

void TestWnd::onPushButtonGetSourceCodeClicked() {
  SourceCodeWidget* pWeiXin = new SourceCodeWidget(this);
  pWeiXin->show();
}

void TestWnd::onPushButtonQuickSettingForIrregularWndClicked() {
  checkboxOsrEnabled_->setChecked(true);
  checkboxFramelessWindow_->setChecked(true);
  checkboxTranslucentWindowBackground_->setChecked(true);
  radioButtonCefWidget_->setChecked(true);
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

void TestWnd::onPushButtonQuickSettingForElectronClicked() {
  checkboxFramelessWindow_->setChecked(true);
  comboBoxUrl_->lineEdit()->setText("http://qcefwidget/test.html");
}

void TestWnd::onCefWndDestroyed(QObject* obj) {
  for (int i = 0; i < listBrowser_->count(); i++) {
    BrowserListItem* blt =
      (BrowserListItem*)listBrowser_->itemWidget(listBrowser_->item(i));
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

void TestWnd::systemPerformanceMonitor() {
  SYSTEM_INFO si;
  GetSystemInfo(&si);
  cpuProcessorCount_ = si.dwNumberOfProcessors;

  PDH_STATUS status = PdhOpenQuery(NULL, NULL, &query_);
  if (status == ERROR_SUCCESS) {
    status = PdhAddCounter(query_,
                           TEXT("\\Process(QCefWidgetTest)\\% Processor Time"),
                           NULL,
                           &mainProcessCpuCounter_);
    status = PdhAddCounter(query_,
                           TEXT("\\Process(QCefWidgetTest)\\Working Set"),
                           NULL,
                           &mainProcessMemCounter_);

    status = PdhAddCounter(query_,
                           TEXT("\\Process(QCefWing)\\% Processor Time"),
                           NULL,
                           &renderProcessCpuCounter_);
    status = PdhAddCounter(query_,
                           TEXT("\\Process(QCefWing)\\Working Set"),
                           NULL,
                           &renderProcessMemCounter_);
  }

  performanceTimer_.setInterval(200);
  connect(&performanceTimer_, &QTimer::timeout, [this]() {
    PdhCollectQueryData(query_);
    PDH_FMT_COUNTERVALUE pdhValue;
    DWORD dwValue = 0;

    PDH_STATUS status = PdhGetFormattedCounterValue(
      mainProcessCpuCounter_, PDH_FMT_DOUBLE, &dwValue, &pdhValue);
    if (status == ERROR_SUCCESS) {
      if (dashboardWidgetMainProcCPU_)
        dashboardWidgetMainProcCPU_->setValue(
          floor(pdhValue.doubleValue / cpuProcessorCount_));
    }

    status = PdhGetFormattedCounterValue(
      mainProcessMemCounter_, PDH_FMT_DOUBLE, &dwValue, &pdhValue);
    if (status == ERROR_SUCCESS) {
      if (dashboardWidgetMainProcMemory_)
        dashboardWidgetMainProcMemory_->setValue(pdhValue.doubleValue / 1024.f /
                                                 1024.f);
    }

    status = PdhGetFormattedCounterValue(
      renderProcessCpuCounter_, PDH_FMT_DOUBLE, &dwValue, &pdhValue);
    if (status == ERROR_SUCCESS) {
      if (dashboardWidgetRenderProcCPU_)
        dashboardWidgetRenderProcCPU_->setValue(
          floor(pdhValue.doubleValue / cpuProcessorCount_));
    }

    status = PdhGetFormattedCounterValue(
      renderProcessMemCounter_, PDH_FMT_DOUBLE, &dwValue, &pdhValue);
    if (status == ERROR_SUCCESS) {
      if (dashboardWidgetRenderProcMemory_)
        dashboardWidgetRenderProcMemory_->setValue(pdhValue.doubleValue /
                                                   1024.f / 1024.f);
    }
  });
  performanceTimer_.start();
}
