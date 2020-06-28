#include "TestWnd.h"
#include "QCefSetting.h"
#include "QCefWidget.h"
#include "QCefOpenGLWidget.h"
#include "TransparentCefWnd.h"
#include <QDebug>
#include <QStyle>
#include <QCloseEvent>

#pragma execution_character_set("utf-8")

namespace {
QSize kWindowDefaultSize = QSize(1080, 600);
}

TestWnd::TestWnd(QWidget *parent)
    : QMainWindow(parent)
    , cefWidget_(nullptr)
    , cefOpenGLWidget_(nullptr)
    , transWidgetCefWnd_(nullptr)
    , transOpenGLWidgetCefWnd_(nullptr) {
  setupUi();

  transWidgetCefWnd_ = new TransparentCefWnd(false, nullptr);
  connect(transWidgetCefWnd_, &QWidget::destroyed, transWidgetCefWnd_, [this]() {
    qInfo() << "transWidgetCefWnd_::destroyed";
    transWidgetCefWnd_ = nullptr;
  });
  transWidgetCefWnd_->show();

  transOpenGLWidgetCefWnd_ = new TransparentCefWnd(true, nullptr);
  connect(transOpenGLWidgetCefWnd_, &QWidget::destroyed, transOpenGLWidgetCefWnd_, [this]() {
    qInfo() << "transOpenGLWidgetCefWnd_::destroyed";
    transOpenGLWidgetCefWnd_ = nullptr;
  });
  transOpenGLWidgetCefWnd_->show();

  bindUiEvent();

  // Set FPS
  if (cefWidget_)
    cefWidget_->setFPS(30);

  if (cefOpenGLWidget_)
    cefOpenGLWidget_->setFPS(30);

  QString url1 = QString("file:///%1").arg(QCoreApplication::applicationDirPath() + u8"/TestResource/TestPage.html");
  QString url2 = QString("file:///%1").arg(QCoreApplication::applicationDirPath() + u8"/TestResource/Tree.html");

  if (checkBoxOpacityCefWidget_->isChecked() && cefWidget_)
    cefWidget_->navigateToUrl(url1);

  if (checkBoxOpacityCefOpenGLWidget_->isChecked() && cefOpenGLWidget_)
    cefOpenGLWidget_->navigateToUrl(url1);

  if (checkBoxTransparentCefWidget_->isChecked() && transWidgetCefWnd_)
    transWidgetCefWnd_->navigateToUrl(url2);

  if (checkBoxTransparentCefOpenGLWidget_->isChecked() && transOpenGLWidgetCefWnd_)
    transOpenGLWidgetCefWnd_->navigateToUrl(url2);
}

TestWnd::~TestWnd() { qInfo() << "TestWnd::~TestWnd, this: " << this; }

void TestWnd::closeEvent(QCloseEvent *event) {
  qInfo() << "TestWnd::closeEvent";

  if (transWidgetCefWnd_) {
    transWidgetCefWnd_->close();
  }

  if (transOpenGLWidgetCefWnd_) {
    transOpenGLWidgetCefWnd_->close();
  }
}

QSize TestWnd::sizeHint() const { return kWindowDefaultSize; }

void TestWnd::resizeEvent(QResizeEvent *event) {}

void TestWnd::setupUi() {
  this->setWindowTitle("CefWidget Demo");
  this->setObjectName("QCefWidgetDemo");

#pragma region Top
  QLabel *lblTarget = new QLabel("Target browser: ");

  checkBoxOpacityCefOpenGLWidget_ = new QCheckBox("Opacity QCefOpenGLWidget");
  checkBoxOpacityCefOpenGLWidget_->setChecked(true);

  checkBoxOpacityCefWidget_ = new QCheckBox("Opacity QCefWidget");
  checkBoxOpacityCefWidget_->setChecked(true);

  checkBoxTransparentCefWidget_ = new QCheckBox("Transparent QCefWidget");
  checkBoxTransparentCefWidget_->setChecked(true);

  checkBoxTransparentCefOpenGLWidget_ = new QCheckBox("Transparent QCefOpenGLWidget");
  checkBoxTransparentCefOpenGLWidget_->setChecked(true);

  QHBoxLayout *hlTargetCefWidget = new QHBoxLayout();
  hlTargetCefWidget->addWidget(lblTarget);
  hlTargetCefWidget->addWidget(checkBoxOpacityCefWidget_);
  hlTargetCefWidget->addWidget(checkBoxOpacityCefOpenGLWidget_);
  hlTargetCefWidget->addWidget(checkBoxTransparentCefWidget_);
  hlTargetCefWidget->addWidget(checkBoxTransparentCefOpenGLWidget_);
  hlTargetCefWidget->addStretch();

  actionTriggerTestEvent_ = new QAction();
  actionTriggerTestEvent_->setShortcut(QKeySequence("Ctrl+E"));
  actionTriggerTestEvent_->setText("Trigger Test Event");
  connect(actionTriggerTestEvent_, &QAction::triggered, this, &TestWnd::onTriggerTestEvent);

  pushButtonBack_ = new QPushButton("Back");
  pushButtonBack_->setObjectName("pushButtonBack");
  pushButtonBack_->setCursor(QCursor(Qt::PointingHandCursor));

  pushButtonForward_ = new QPushButton("Forward");
  pushButtonForward_->setObjectName("pushButtonForward");
  pushButtonForward_->setCursor(QCursor(Qt::PointingHandCursor));

  pushButtonReload_ = new QPushButton("Reload");
  pushButtonReload_->setObjectName("pushButtonReload");
  pushButtonReload_->setCursor(QCursor(Qt::PointingHandCursor));

  pushButtonOpenDevTools_ = new QPushButton("Open DevTools");
  pushButtonOpenDevTools_->setCursor(QCursor(Qt::PointingHandCursor));

  pushButtonCloseDevTools_ = new QPushButton("Close DevTools");
  pushButtonOpenDevTools_->setCursor(QCursor(Qt::PointingHandCursor));

  pushButtonExit_ = new QPushButton("Exit");
  pushButtonExit_->setCursor(QCursor(Qt::PointingHandCursor));

  comboBoxUrl_ = new QComboBox();
  comboBoxUrl_->setObjectName("comboBoxUrl");
  comboBoxUrl_->setFixedHeight(22);
  comboBoxUrl_->setEditable(true);
  comboBoxUrl_->addItems(QStringList() << "about:blank" << QString("file:///%1").arg(QCoreApplication::applicationDirPath() + u8"/TestResource/TestPage.html")
                                       << QString("file:///%1").arg(QCoreApplication::applicationDirPath() + u8"/TestResource/Tree.html")
                                       << QString("file:///%1").arg(QCoreApplication::applicationDirPath() + u8"/TestResource/Clock.html") << "https://html5test.com/"
                                       << "https://www.bing.com"
                                       << "https://www.google.com"
                                       << "https://map.baidu.com/"
                                       << "https://ant.design/components/overview/");

  QHBoxLayout *hlFunction = new QHBoxLayout();
  hlFunction->setSpacing(6);
  hlFunction->setContentsMargins(6, 0, 6, 0);
  hlFunction->addWidget(pushButtonBack_);
  hlFunction->addWidget(pushButtonForward_);
  hlFunction->addWidget(pushButtonReload_);
  hlFunction->addWidget(pushButtonOpenDevTools_);
  hlFunction->addWidget(pushButtonCloseDevTools_);
  hlFunction->addStretch();
  hlFunction->addWidget(pushButtonExit_);

  QVBoxLayout *vlTop = new QVBoxLayout();
  vlTop->addLayout(hlTargetCefWidget);
  vlTop->addWidget(comboBoxUrl_);
  vlTop->addLayout(hlFunction);

  widgetTop_ = new QWidget();
  widgetTop_->setLayout(vlTop);

#pragma endregion

#pragma region CEF
  cefWidget_ = new QCefWidget();
  if (cefWidget_) {
    cefWidget_->setObjectName("cefWidget");
    cefWidget_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  }

  cefOpenGLWidget_ = new QCefOpenGLWidget();
  if (cefOpenGLWidget_) {
    cefOpenGLWidget_->setObjectName("cefOpenGLWidget");
    cefOpenGLWidget_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  }

  splitterCef_ = new QSplitter(Qt::Orientation::Horizontal);
  splitterCef_->setObjectName("splitterCef");
  splitterCef_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  if (cefWidget_)
    splitterCef_->addWidget(cefWidget_);
  if (cefOpenGLWidget_)
    splitterCef_->addWidget(cefOpenGLWidget_);

#pragma endregion

#pragma region Bottom
  plainTextEditLog_ = new QPlainTextEdit();
  plainTextEditLog_->setObjectName("plainTextEditLog");
  plainTextEditLog_->setReadOnly(true);
#pragma endregion

  splitterMain_ = new QSplitter(Qt::Orientation::Vertical);
  splitterMain_->setObjectName("splitterMain");
  splitterMain_->addWidget(widgetTop_);
  splitterMain_->addWidget(splitterCef_);
  splitterMain_->addWidget(plainTextEditLog_);
  // https://stackoverflow.com/questions/43831474/how-to-equally-distribute-the-width-of-qsplitter
  splitterMain_->setSizes(QList<int>({(int)(sizeHint().height() * 1.f / 5.f), (int)(sizeHint().height() * 3.f / 5.f), (int)(sizeHint().height() * 1.f / 5.f)}));

  this->setCentralWidget(splitterMain_);

  this->resize(sizeHint());

  QString qss;
  QFile qssFile(":/QCefWidgetTest/QCefWidgetTest.qss");
  qssFile.open(QFile::ReadOnly);
  if (qssFile.isOpen()) {
    qss = QLatin1String(qssFile.readAll());
    this->setStyleSheet(qss);
    qssFile.close();
  }
}

void TestWnd::bindUiEvent() {

  connect(comboBoxUrl_->lineEdit(), &QLineEdit::returnPressed, [this]() {
    QString url = comboBoxUrl_->lineEdit()->text();
    if (checkBoxOpacityCefWidget_->isChecked() && cefWidget_)
      cefWidget_->navigateToUrl(url);

    if (checkBoxOpacityCefOpenGLWidget_->isChecked() && cefOpenGLWidget_)
      cefOpenGLWidget_->navigateToUrl(url);

    if (checkBoxTransparentCefWidget_->isChecked() && transWidgetCefWnd_)
      transWidgetCefWnd_->navigateToUrl(url);

    if (checkBoxTransparentCefOpenGLWidget_->isChecked() && transOpenGLWidgetCefWnd_)
      transOpenGLWidgetCefWnd_->navigateToUrl(url);
  });

  connect(comboBoxUrl_, QOverload<const QString &>::of(&QComboBox::currentIndexChanged), this, [this](const QString &text) {
    if (checkBoxOpacityCefWidget_->isChecked() && cefWidget_)
      cefWidget_->navigateToUrl(text);

    if (checkBoxOpacityCefOpenGLWidget_->isChecked() && cefOpenGLWidget_)
      cefOpenGLWidget_->navigateToUrl(text);

    if (checkBoxTransparentCefWidget_->isChecked() && transWidgetCefWnd_)
      transWidgetCefWnd_->navigateToUrl(text);

    if (checkBoxTransparentCefOpenGLWidget_->isChecked() && transOpenGLWidgetCefWnd_)
      transOpenGLWidgetCefWnd_->navigateToUrl(text);
  });

  connect(pushButtonBack_, &QPushButton::clicked, [this]() {
    if (checkBoxOpacityCefWidget_->isChecked() && cefWidget_)
      cefWidget_->goBack();

    if (checkBoxOpacityCefOpenGLWidget_->isChecked() && cefOpenGLWidget_)
      cefOpenGLWidget_->goBack();
  });

  connect(pushButtonForward_, &QPushButton::clicked, [this]() {
    if (checkBoxOpacityCefWidget_->isChecked() && cefWidget_)
      cefWidget_->goForward();

    if (checkBoxOpacityCefOpenGLWidget_->isChecked() && cefOpenGLWidget_)
      cefOpenGLWidget_->goForward();
  });

  connect(pushButtonReload_, &QPushButton::clicked, [this]() {
    if (checkBoxOpacityCefWidget_->isChecked() && cefWidget_) {
      cefWidget_->reloadBrowser();
    }

    if (checkBoxOpacityCefOpenGLWidget_->isChecked() && cefOpenGLWidget_) {
      cefOpenGLWidget_->reloadBrowser();
    }
  });

  connect(pushButtonOpenDevTools_, &QPushButton::clicked, this, [this]() {
    if (checkBoxOpacityCefWidget_->isChecked() && cefWidget_) {
      cefWidget_->showDevTools();
    }

    if (checkBoxOpacityCefOpenGLWidget_->isChecked() && cefOpenGLWidget_) {
      cefOpenGLWidget_->showDevTools();
    }

    if (checkBoxTransparentCefWidget_->isChecked() && transWidgetCefWnd_)
      transWidgetCefWnd_->showDevTools();

    if (checkBoxTransparentCefOpenGLWidget_->isChecked() && transOpenGLWidgetCefWnd_)
      transOpenGLWidgetCefWnd_->showDevTools();
  });

  connect(pushButtonCloseDevTools_, &QPushButton::clicked, this, [this]() {
    if (checkBoxOpacityCefWidget_->isChecked() && cefWidget_)
      cefWidget_->closeDevTools();

    if (checkBoxOpacityCefOpenGLWidget_->isChecked() && cefOpenGLWidget_)
      cefOpenGLWidget_->closeDevTools();

    if (checkBoxTransparentCefWidget_->isChecked() && transWidgetCefWnd_)
      transWidgetCefWnd_->closeDevTools();

    if (checkBoxTransparentCefOpenGLWidget_->isChecked() && transOpenGLWidgetCefWnd_)
      transOpenGLWidgetCefWnd_->closeDevTools();
  });

  connect(pushButtonExit_, &QPushButton::clicked, this, [this]() { this->close(); });

  connect(
      cefWidget_, &QCefWidget::loadingStateChanged, this,
      [this](bool isLoadingBrowser, bool canGoBack, bool canGoForward) {
        plainTextEditLog_->appendPlainText(QString("QCefWidget isLoadingBrowser: %1, canGoBack: %2, canGoForward: %3").arg(isLoadingBrowser).arg(canGoBack).arg(canGoForward));
      },
      Qt::QueuedConnection);

  connect(
      cefOpenGLWidget_, &QCefOpenGLWidget::loadingStateChanged, this,
      [this](bool isLoadingBrowser, bool canGoBack, bool canGoForward) {
        plainTextEditLog_->appendPlainText(
            QString("QCefOpenGLWidget isLoadingBrowser: %1, canGoBack: %2, canGoForward: %3").arg(isLoadingBrowser).arg(canGoBack).arg(canGoForward));
      },
      Qt::QueuedConnection);

  connect(
      cefWidget_, &QCefWidget::invokeMethodNotify, this,
      [this](int browserId, int frameId, const QString &method, const QVariantList &arguments) {
        QString str = QString("[InvokeMethodNotify]\r\nbrowserId: %1\r\nframeId: %2\r\nmethod: %3\r\n").arg(browserId).arg(frameId).arg(method);

        plainTextEditLog_->appendPlainText(str);
      },
      Qt::QueuedConnection);

  connect(
      cefWidget_, &QCefWidget::cefUrlRequest, this,
      [this](const QString &url) {
        QString str = QString("[CefUrlRequest]\r\nurl: %1\r\n").arg(url);

        plainTextEditLog_->appendPlainText(str);
      },
      Qt::QueuedConnection);

  connect(
      cefWidget_, &QCefWidget::cefQueryRequest, this,
      [this](const QCefQuery &query) {
        QString str = QString("[CefQueryRequest]\r\nid: %1\r\nreqeust: %2\r\nresponse: %3\r\nresult: %4\r\nerror: %5\r\n")
                          .arg(query.id())
                          .arg(query.reqeust())
                          .arg(query.response())
                          .arg(query.result())
                          .arg(query.error());

        plainTextEditLog_->appendPlainText(str);
      },
      Qt::QueuedConnection);

  connect(
      cefWidget_, &QCefWidget::titleChanged, this, [this](QString title) { plainTextEditLog_->appendPlainText(QString("QCefWidget titleChanged: %1").arg(title)); },
      Qt::QueuedConnection);

  connect(
      cefWidget_, &QCefWidget::titleChanged, this, [this](QString title) { plainTextEditLog_->appendPlainText(QString("QCefOpenGLWidget titleChanged: %1").arg(title)); },
      Qt::QueuedConnection);

  connect(
      cefWidget_, &QCefWidget::urlChanged, this,
      [this](bool isMainFrame, QString url) { plainTextEditLog_->appendPlainText(QString("QCefWidget urlChanged, isMainFrame: %1, url: %2").arg(isMainFrame).arg(url)); },
      Qt::QueuedConnection);

  connect(
      cefWidget_, &QCefWidget::urlChanged, this,
      [this](bool isMainFrame, QString url) { plainTextEditLog_->appendPlainText(QString("QCefOpenGLWidget urlChanged, isMainFrame: %1, url: %2").arg(isMainFrame).arg(url)); },
      Qt::QueuedConnection);
}

void TestWnd::onTriggerTestEvent() {
  static int eventIndex = 1;
  QCefEvent event(QString("TestEvent%1").arg(eventIndex));
  event.setStringProperty("StrProp", QString("String_%1").arg(eventIndex));
  event.setIntProperty("IntProp", 1000 + eventIndex);
  event.setDoubleProperty("DoubleProp", 3.1415926f + (double)eventIndex);
  event.setBoolProperty("BoolProp", eventIndex % 2 == 0);

  cefWidget_->broadcastEvent("eventTest", event);

  eventIndex++;
}