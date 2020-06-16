#include "CefViewTestWnd.h"
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

CefViewTestWnd::CefViewTestWnd(QWidget *parent)
    : QMainWindow(parent)
    , cefWidget_(nullptr)
    , cefOpenGLWidget_(nullptr) {
  setupUi();

  connect(comboBoxUrl_->lineEdit(), &QLineEdit::returnPressed,
          [this]() { 
    cefWidget_->navigateToUrl(comboBoxUrl_->lineEdit()->text()); 
    cefOpenGLWidget_->navigateToUrl(comboBoxUrl_->lineEdit()->text());
  });

  connect(comboBoxUrl_, QOverload<const QString &>::of(&QComboBox::currentIndexChanged), this,
          [this](const QString &text) { 
    cefWidget_->navigateToUrl(text); 
    cefOpenGLWidget_->navigateToUrl(text);
  });

  connect(pushButtonBack_, &QPushButton::clicked, [this]() { 
    cefWidget_->browserGoBack(); 
    cefOpenGLWidget_->browserGoBack();
  });

  connect(pushButtonForward_, &QPushButton::clicked, [this]() { 
    cefWidget_->browserGoForward(); 
    cefOpenGLWidget_->browserGoForward();
  });

  connect(pushButtonReload_, &QPushButton::clicked, [this]() {
    if (cefWidget_->browserIsLoading())
      cefWidget_->browserStopLoad();
    else
      cefWidget_->browserReload();

    if (cefOpenGLWidget_->browserIsLoading())
      cefOpenGLWidget_->browserStopLoad();
    else
      cefOpenGLWidget_->browserReload();
  });

  connect(pushButtonApply_, &QPushButton::clicked, this, &CefViewTestWnd::onPushButtonApplyClicked);

  connect(
      cefWidget_, &QCefWidget::loadingStateChanged, this,
      [this](bool isLoading, bool canGoBack, bool canGoForward) {
        pushButtonBack_->setEnabled(canGoBack);
        pushButtonForward_->setEnabled(canGoForward);
        pushButtonReload_->setProperty("loading", isLoading ? "1" : "0");
        style()->unpolish(pushButtonReload_);
        style()->polish(pushButtonReload_);
      },
      Qt::QueuedConnection);

  connect(
      cefWidget_, &QCefWidget::invokeMethodNotify, this,
      [this](int browserId, int frameId, const QString &method, const QVariantList &arguments) {
        QString str =
            QString("[InvokeMethodNotify]\r\nbrowserId: %1\r\nframeId: %2\r\nmethod: %3\r\n")
                .arg(browserId)
                .arg(frameId)
                .arg(method);

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
        QString str =
            QString(
                "[CefQueryRequest]\r\nid: %1\r\nreqeust: %2\r\nresponse: %3\r\nresult: %4\r\nerror: %5\r\n")
                .arg(query.id())
                .arg(query.reqeust())
                .arg(query.response())
                .arg(query.result())
                .arg(query.error());

        plainTextEditLog_->appendPlainText(str);
      },
      Qt::QueuedConnection);

  connect(
      cefWidget_, &QCefWidget::titleChanged, this,
      [this](QString title) { this->setWindowTitle(title); }, Qt::QueuedConnection);

  checkboxHookTopLevelCloseMsg_->setChecked(QCefSetting::isHookTopLevelWindowCloseMessage());
  checkboxIgnoreTopLevelCloseMsg_->setChecked(QCefSetting::isIgnoreTopLevelCloseMessage());

  cefWidget_->setFPS(30);
  cefOpenGLWidget_->setFPS(30);
  if (comboBoxUrl_->itemText(0).length() > 0) {
    cefWidget_->navigateToUrl(comboBoxUrl_->itemText(0));
    cefOpenGLWidget_->navigateToUrl(comboBoxUrl_->itemText(0));
  }
}

CefViewTestWnd::~CefViewTestWnd() {}

void CefViewTestWnd::closeEvent(QCloseEvent *event) { event->accept(); }

QSize CefViewTestWnd::sizeHint() const { return kWindowDefaultSize; }

void CefViewTestWnd::resizeEvent(QResizeEvent *event) {}

void CefViewTestWnd::setupUi() {
  this->setWindowTitle("CefView Demo");
  this->setObjectName("QCefViewTestClass");

  actionShowTransparentCef_ = new QAction();
  actionShowTransparentCef_->setShortcut(QKeySequence("Ctrl+T"));
  actionShowTransparentCef_->setText("Show Transparent CEF");
  connect(actionShowTransparentCef_, &QAction::triggered, this,
          &CefViewTestWnd::onShowTransparentCefWnd);

  actionTriggerTestEvent_ = new QAction();
  actionTriggerTestEvent_->setShortcut(QKeySequence("Ctrl+E"));
  actionTriggerTestEvent_->setText("Trigger Test Event");
  connect(actionTriggerTestEvent_, &QAction::triggered, this, &CefViewTestWnd::onTriggerTestEvent);

  menuTest_ = new QMenu();
  menuTest_->setObjectName("menuTest");
  menuTest_->addAction(actionTriggerTestEvent_);
  menuTest_->addAction(actionShowTransparentCef_);

  pushButtonBack_ = new QPushButton();
  pushButtonBack_->setObjectName("pushButtonBack");
  pushButtonBack_->setFixedSize(30, 30);
  pushButtonBack_->setCursor(QCursor(Qt::PointingHandCursor));

  pushButtonForward_ = new QPushButton();
  pushButtonForward_->setObjectName("pushButtonForward");
  pushButtonForward_->setFixedSize(30, 30);
  pushButtonForward_->setCursor(QCursor(Qt::PointingHandCursor));

  pushButtonReload_ = new QPushButton();
  pushButtonReload_->setObjectName("pushButtonReload");
  pushButtonReload_->setFixedSize(30, 30);
  pushButtonReload_->setCursor(QCursor(Qt::PointingHandCursor));
  pushButtonReload_->setProperty("loading", "0");

  pushButtonTest_ = new QPushButton();
  pushButtonTest_->setObjectName("pushButtonTest");
  pushButtonTest_->setCursor(QCursor(Qt::PointingHandCursor));
  pushButtonTest_->setFixedSize(30, 30);
  pushButtonTest_->setText("");

  Q_ASSERT(menuTest_);
  pushButtonTest_->setMenu(menuTest_);

  comboBoxUrl_ = new QComboBox();
  comboBoxUrl_->setObjectName("comboBoxUrl");
  comboBoxUrl_->setFixedHeight(22);
  comboBoxUrl_->setEditable(true);
  comboBoxUrl_->addItems(
      QStringList() << QString("file:///%1")
                           .arg(QCoreApplication::applicationDirPath() + u8"/TestPage.html")
                    << "about:blank"
                    << "https://html5test.com/"
                    << "https://www.bing.com"
                    << "https://www.google.com"
                    << "https://map.baidu.com/"
                    << "https://ant.design/components/overview/");

  QHBoxLayout *hlTop = new QHBoxLayout();
  hlTop->setSpacing(6);
  hlTop->setContentsMargins(6, 0, 6, 0);
  hlTop->addWidget(pushButtonBack_);
  hlTop->addWidget(pushButtonForward_);
  hlTop->addWidget(pushButtonReload_);
  hlTop->addWidget(comboBoxUrl_);
  hlTop->addWidget(pushButtonTest_);

  cefWidget_ = new QCefWidget();
  cefWidget_->setObjectName("cefWidget");
  cefWidget_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  cefOpenGLWidget_ = new QCefOpenGLWidget();
  cefOpenGLWidget_->setObjectName("cefOpenGLWidget");
  cefOpenGLWidget_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  splitterCef_ = new QSplitter(Qt::Orientation::Horizontal);
  splitterCef_->setObjectName("splitterCef");
  splitterCef_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  splitterCef_->addWidget(cefWidget_);
  splitterCef_->addWidget(cefOpenGLWidget_);

  plainTextEditLog_ = new QPlainTextEdit();
  plainTextEditLog_->setObjectName("plainTextEditLog");
  plainTextEditLog_->setReadOnly(true);

  pushButtonApply_ = new QPushButton();
  pushButtonApply_->setObjectName("pushButtonApply");
  pushButtonApply_->setText("Apply");

  checkboxHookTopLevelCloseMsg_ = new QCheckBox();
  checkboxHookTopLevelCloseMsg_->setObjectName("checkboxHookTopLevelCloseMsg");
  checkboxHookTopLevelCloseMsg_->setText("Hook Top-level WM_CLOSE");
  checkboxHookTopLevelCloseMsg_->setToolTip("Hook Top-Level Window's WM_CLOSE message");

  checkboxIgnoreTopLevelCloseMsg_ = new QCheckBox();
  checkboxIgnoreTopLevelCloseMsg_->setObjectName("checkboxIgnoreTopLevelCloseMsg");
  checkboxIgnoreTopLevelCloseMsg_->setText("Ignore Top-level WM_CLOSE");
  checkboxIgnoreTopLevelCloseMsg_->setToolTip("Ignore Top-level Window's WM_CLOSE message");

  QVBoxLayout *vlBottomLeft = new QVBoxLayout();
  vlBottomLeft->setSpacing(6);
  vlBottomLeft->addWidget(checkboxHookTopLevelCloseMsg_);
  vlBottomLeft->addWidget(checkboxIgnoreTopLevelCloseMsg_);
  vlBottomLeft->addWidget(pushButtonApply_);
  vlBottomLeft->addStretch();

  QHBoxLayout *vlBottom = new QHBoxLayout();
  vlBottom->setSpacing(6);
  vlBottom->setContentsMargins(6, 0, 6, 6);
  vlBottom->addLayout(vlBottomLeft);
  vlBottom->addWidget(plainTextEditLog_);

  widgetBottom_ = new QWidget();
  widgetBottom_->setObjectName("widgetBottom");
  widgetBottom_->setLayout(vlBottom);
  widgetBottom_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  splitterMain_ = new QSplitter(Qt::Orientation::Vertical);
  splitterMain_->setObjectName("splitterMain");
  splitterMain_->addWidget(splitterCef_);
  splitterMain_->addWidget(widgetBottom_);
  // https://stackoverflow.com/questions/43831474/how-to-equally-distribute-the-width-of-qsplitter
  splitterMain_->setSizes(
      QList<int>({(int)(sizeHint().height() * 3.f / 4.f), (int)(sizeHint().height() * 1.f / 4.f)}));

  QVBoxLayout *vlMain = new QVBoxLayout();
  vlMain->setSpacing(0);
  vlMain->setContentsMargins(0, 0, 0, 0);
  vlMain->addSpacing(2);
  vlMain->addLayout(hlTop);
  vlMain->addWidget(splitterMain_);

  centralWidget_ = new QWidget();
  centralWidget_->setLayout(vlMain);

  this->setCentralWidget(centralWidget_);

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

void CefViewTestWnd::onShowTransparentCefWnd() {
  TransparentCefWnd *pTransparentCefWnd = new TransparentCefWnd();
  pTransparentCefWnd->show();
}

void CefViewTestWnd::onTriggerTestEvent() {
  static int eventIndex = 1;
  QCefEvent event(QString("TestEvent%1").arg(eventIndex));
  event.setStringProperty("StrProp", QString("String_%1").arg(eventIndex));
  event.setIntProperty("IntProp", 1000 + eventIndex);
  event.setDoubleProperty("DoubleProp", 3.1415926f + (double)eventIndex);
  event.setBoolProperty("BoolProp", eventIndex % 2 == 0);

  cefWidget_->broadcastEvent("eventTest", event);

  eventIndex++;
}

void CefViewTestWnd::onPushButtonApplyClicked() {}
