#include "TestWnd.h"
#include "resource.h"
#include "QCefSetting.h"
#include "QCefWidget.h"
#include "QCefOpenGLWidget.h"
#include "TransparentCefWnd.h"
#include <QDebug>
#include <QStyle>
#include <QCloseEvent>
#include "FramelessCEF.h"

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
  QCefSetting::setFlashPlugin("TestResource\\pepperflash\\26.0.0.126\\pepflashplayer.dll", "26.0.0.126");
  //QCefSetting::setFlashPlugin("TestResource/pepperflash/32.0.0.387/pepflashplayer32.dll", "32.0.0.387");
  //QCefSetting::setFlashPlugin("TestResource/pepperflash/32.0.0.192/pepflashplayer.dll", "32.0.0.192");

  QCefSetting::setResourceMap({ {"TestPage.html", {IDR_TEST_PAGE, "PAGE" } } });

  setupUi();

  transWidgetCefWnd_ = new TransparentCefWnd(false, nullptr);
  if (transWidgetCefWnd_) {
    connect(transWidgetCefWnd_, &QWidget::destroyed, transWidgetCefWnd_, [this]() {
      qInfo() << "transWidgetCefWnd_::destroyed";
      transWidgetCefWnd_ = nullptr;
    });
    transWidgetCefWnd_->setOsrEnabled(checkboxOsrEnabled_->isChecked());
    transWidgetCefWnd_->show();
  }

  transOpenGLWidgetCefWnd_ = new TransparentCefWnd(true, nullptr);
  if (transOpenGLWidgetCefWnd_) {
    connect(transOpenGLWidgetCefWnd_, &QWidget::destroyed, transOpenGLWidgetCefWnd_, [this]() {
      qInfo() << "transOpenGLWidgetCefWnd_::destroyed";
      transOpenGLWidgetCefWnd_ = nullptr;
    });
    transOpenGLWidgetCefWnd_->setOsrEnabled(checkboxOsrEnabled_->isChecked());
    transOpenGLWidgetCefWnd_->show();
  }

  bindUiEvent();

  if (cefWidget_) {
    cefWidget_->setFPS(30);
    cefWidget_->setOsrEnabled(checkboxOsrEnabled_->isChecked());
    cefWidget_->setContextMenuEnabled(checkboxContextMenuEnabled_->isChecked());
  }

  if (cefOpenGLWidget_) {
    cefOpenGLWidget_->setFPS(30);
    cefOpenGLWidget_->setOsrEnabled(checkboxOsrEnabled_->isChecked());
    cefOpenGLWidget_->setContextMenuEnabled(checkboxContextMenuEnabled_->isChecked());
  }

  QString url;
  if (comboBoxUrl_->count() > 0)
    url = comboBoxUrl_->itemText(0);

  if (checkBoxOpacityCefWidget_->isChecked() && cefWidget_) {
    if (url.length() > 0)
      cefWidget_->navigateToUrl(url);
  }

  if (checkBoxOpacityCefOpenGLWidget_->isChecked() && cefOpenGLWidget_) {
    if (url.length() > 0)
      cefOpenGLWidget_->navigateToUrl(url);
  }

  if (checkBoxTransparentCefWidget_->isChecked() && transWidgetCefWnd_) {
    if (url.length() > 0)
      transWidgetCefWnd_->navigateToUrl(url);
  }

  if (checkBoxTransparentCefOpenGLWidget_->isChecked() && transOpenGLWidgetCefWnd_) {
    if (url.length() > 0)
      transOpenGLWidgetCefWnd_->navigateToUrl(url);
  }
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
  QLabel *lblOption = new QLabel("Options: ");

  checkboxOsrEnabled_ = new QCheckBox("CEF OSR(off-screen render)");
  checkboxOsrEnabled_->setChecked(false);

  checkboxContextMenuEnabled_ = new QCheckBox("Enable Context Menu");
  checkboxContextMenuEnabled_->setChecked(true);

  QHBoxLayout *hlOptions = new QHBoxLayout();
  hlOptions->addWidget(lblOption);
  hlOptions->addWidget(checkboxOsrEnabled_);
  hlOptions->addWidget(checkboxContextMenuEnabled_);
  hlOptions->addStretch();

  QLabel *lblTarget = new QLabel("Target browser: ");

  checkBoxOpacityCefOpenGLWidget_ = new QCheckBox("Opacity QCefOpenGLWidget");
  checkBoxOpacityCefOpenGLWidget_->setChecked(true);

  checkBoxOpacityCefWidget_ = new QCheckBox("Opacity QCefWidget");
  checkBoxOpacityCefWidget_->setChecked(true);

  checkBoxTransparentCefWidget_ = new QCheckBox("Transparent QCefWidget");
  checkBoxTransparentCefWidget_->setChecked(false);

  checkBoxTransparentCefOpenGLWidget_ = new QCheckBox("Transparent QCefOpenGLWidget");
  checkBoxTransparentCefOpenGLWidget_->setChecked(false);

  QHBoxLayout *hlTargetCefWidget = new QHBoxLayout();
  hlTargetCefWidget->addWidget(lblTarget);
  hlTargetCefWidget->addWidget(checkBoxOpacityCefWidget_);
  hlTargetCefWidget->addWidget(checkBoxOpacityCefOpenGLWidget_);
  hlTargetCefWidget->addWidget(checkBoxTransparentCefWidget_);
  hlTargetCefWidget->addWidget(checkBoxTransparentCefOpenGLWidget_);
  hlTargetCefWidget->addStretch();

  pushButtonBack_ = new QPushButton("Back");
  pushButtonBack_->setObjectName("pushButtonBack");
  pushButtonBack_->setCursor(QCursor(Qt::PointingHandCursor));

  pushButtonForward_ = new QPushButton("Forward");
  pushButtonForward_->setObjectName("pushButtonForward");
  pushButtonForward_->setCursor(QCursor(Qt::PointingHandCursor));

  pushButtonReload_ = new QPushButton("Reload");
  pushButtonReload_->setObjectName("pushButtonReload");
  pushButtonReload_->setCursor(QCursor(Qt::PointingHandCursor));

  pushButtonTriggerAnEvent_ = new QPushButton("Trigger An Event");
  pushButtonTriggerAnEvent_->setObjectName("pushButtonTriggerAnEvent");
  pushButtonTriggerAnEvent_->setCursor(QCursor(Qt::PointingHandCursor));

  pushButtonTrigger100Event_ = new QPushButton("Trigger 100 Event");
  pushButtonTrigger100Event_->setObjectName("pushButtonTrigger100Event");
  pushButtonTrigger100Event_->setCursor(QCursor(Qt::PointingHandCursor));

  pushButtonOpenDevTools_ = new QPushButton("Open DevTools");
  pushButtonOpenDevTools_->setCursor(QCursor(Qt::PointingHandCursor));

  pushButtonCloseDevTools_ = new QPushButton("Close DevTools");
  pushButtonOpenDevTools_->setCursor(QCursor(Qt::PointingHandCursor));

  pushButtonShowFramelessCef_ = new QPushButton("Show Frameless CEF");
  pushButtonShowFramelessCef_->setCursor(QCursor(Qt::PointingHandCursor));

  pushButtonExit_ = new QPushButton("Exit");
  pushButtonExit_->setCursor(QCursor(Qt::PointingHandCursor));

  comboBoxUrl_ = new QComboBox();
  comboBoxUrl_->setObjectName("comboBoxUrl");
  comboBoxUrl_->setFixedHeight(22);
  comboBoxUrl_->setEditable(true);
  comboBoxUrl_->addItems(QStringList() << ""
                                       << "about:blank"
                                       << "chrome://version" 
                                       << "http://qcefwidget/TestPage.html"
                                       << QString("file:///%1").arg(QCoreApplication::applicationDirPath() + u8"/TestResource/TestPage.html")
                                       << QString("file:///%1").arg(QCoreApplication::applicationDirPath() + u8"/TestResource/FlashPlayerTest.html")
                                       << QString("file:///%1").arg(QCoreApplication::applicationDirPath() + u8"/TestResource/Tree.html")
                                       << QString("file:///%1").arg(QCoreApplication::applicationDirPath() + u8"/TestResource/PDF.html")
                                       << QString("file:///%1").arg(QCoreApplication::applicationDirPath() + u8"/TestResource/OsrTest.html")
                                       << QString("file:///%1").arg(QCoreApplication::applicationDirPath() + u8"/TestResource/Clock.html") << "https://html5test.com/"
                                       << "https://www.bing.com"
                                       << "https://www.google.com"
                                       << "https://map.baidu.com/"
                                       << "https://ant.design/components/overview/");

  QLineEdit *pLineEdit = comboBoxUrl_->lineEdit();
  if (pLineEdit) {
    pLineEdit->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
  }

  QHBoxLayout *hlFunction = new QHBoxLayout();
  hlFunction->setSpacing(6);
  hlFunction->setContentsMargins(6, 0, 6, 0);
  hlFunction->addWidget(pushButtonBack_);
  hlFunction->addWidget(pushButtonForward_);
  hlFunction->addWidget(pushButtonReload_);
  hlFunction->addWidget(pushButtonTriggerAnEvent_);
  hlFunction->addWidget(pushButtonTrigger100Event_);
  hlFunction->addWidget(pushButtonOpenDevTools_);
  hlFunction->addWidget(pushButtonCloseDevTools_);
  hlFunction->addWidget(pushButtonShowFramelessCef_);
  hlFunction->addStretch();
  hlFunction->addWidget(pushButtonExit_);

  QVBoxLayout *vlTop = new QVBoxLayout();
  vlTop->addLayout(hlOptions);
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

  connect(pushButtonTriggerAnEvent_, &QPushButton::clicked, [this]() {
    unsigned int random_index = time(nullptr);
    QCefEvent event(QString("TestEvent%1").arg(random_index));
    event.setStringProperty("StrProp1", QString("String_%1").arg(random_index));
    event.setStringProperty("StrProp2", QString("String_%1").arg(random_index));
    event.setIntProperty("IntProp1", 1000 + random_index);
    event.setIntProperty("IntProp2", 1000 + random_index);
    event.setDoubleProperty("DoubleProp1", 3.1415926f + (double)random_index);
    event.setDoubleProperty("DoubleProp2", 3.1415926f + (double)random_index);
    event.setBoolProperty("BoolProp1", random_index % 2 == 0);
    event.setBoolProperty("BoolProp2", random_index % 2 == 0);

    if (checkBoxOpacityCefWidget_->isChecked() && cefWidget_) {
      cefWidget_->triggerEvent("eventTest", event);
    }

    if (checkBoxOpacityCefOpenGLWidget_->isChecked() && cefOpenGLWidget_) {
      cefOpenGLWidget_->triggerEvent("eventTest", event);
    }
  });

  connect(pushButtonTrigger100Event_, &QPushButton::clicked, [this]() {
    unsigned int base_index = 1;
    for (int i = 0; i < 100; i++) {
      unsigned int index = base_index + i;
      QCefEvent event(QString("TestEvent%1").arg(index));
      event.setStringProperty("StrProp", QString("String_%1").arg(index));
      event.setIntProperty("IntProp", 1000 + index);
      event.setDoubleProperty("DoubleProp", 3.1415926f + (double)index);
      event.setBoolProperty("BoolProp", index % 2 == 0);

      if (checkBoxOpacityCefWidget_->isChecked() && cefWidget_) {
        cefWidget_->triggerEvent("eventTest", event);
      }

      if (checkBoxOpacityCefOpenGLWidget_->isChecked() && cefOpenGLWidget_) {
        cefOpenGLWidget_->triggerEvent("eventTest", event);
      }
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

  connect(pushButtonShowFramelessCef_, &QPushButton::clicked, this, [this]() {
    FramelessCEF* pFramelessCEF = new FramelessCEF();
    pFramelessCEF->show();
  });

  connect(cefWidget_, &QCefWidget::loadingStateChanged, this, [this](bool isLoadingBrowser, bool canGoBack, bool canGoForward) {
    plainTextEditLog_->appendPlainText(QString("[QCefWidget] isLoadingBrowser: %1, canGoBack: %2, canGoForward: %3\r\n").arg(isLoadingBrowser).arg(canGoBack).arg(canGoForward));
  });

  connect(cefOpenGLWidget_, &QCefOpenGLWidget::loadingStateChanged, this, [this](bool isLoadingBrowser, bool canGoBack, bool canGoForward) {
    plainTextEditLog_->appendPlainText(
        QString("[QCefOpenGLWidget] isLoadingBrowser: %1, canGoBack: %2, canGoForward: %3\r\n").arg(isLoadingBrowser).arg(canGoBack).arg(canGoForward));
  });

  connect(cefWidget_, &QCefWidget::invokeMethodNotify, this, &TestWnd::onInvokeMethodNotify, Qt::QueuedConnection);

  connect(cefOpenGLWidget_, &QCefOpenGLWidget::invokeMethodNotify, this, &TestWnd::onInvokeMethodNotify, Qt::QueuedConnection);

  connect(cefWidget_, &QCefWidget::cefQueryRequest, this, [this](const QCefQuery &query) {
    QString str = QString("[QCefWidget CefQueryRequest] id: %1, reqeust: %2\r\n").arg(query.id()).arg(query.reqeust());
    plainTextEditLog_->appendPlainText(str);

    QCefQuery rsp = query;
    rsp.setResponseResult(true, "this is a query response message from c++.", 0);
    cefWidget_->responseCefQuery(rsp);
  });

  connect(cefOpenGLWidget_, &QCefOpenGLWidget::cefQueryRequest, this, [this](const QCefQuery &query) {
    QString str = QString("[QCefOpenGLWidget CefQueryRequest] id: %1, reqeust: %2\r\n").arg(query.id()).arg(query.reqeust());
    plainTextEditLog_->appendPlainText(str);

    QCefQuery rsp = query;
    rsp.setResponseResult(false, "this is a query response message from c++.", 123);
    cefOpenGLWidget_->responseCefQuery(rsp);
  });

  connect(cefWidget_, &QCefWidget::titleChanged, this, [this](QString title) { plainTextEditLog_->appendPlainText(QString("[QCefWidget] titleChanged: %1\r\n").arg(title)); });

  connect(cefOpenGLWidget_, &QCefOpenGLWidget::titleChanged, this,
          [this](QString title) { plainTextEditLog_->appendPlainText(QString("[QCefOpenGLWidget] titleChanged: %1\r\n").arg(title)); });

  connect(cefWidget_, &QCefWidget::urlChanged, this, [this](bool isMainFrame, QString url) {
    plainTextEditLog_->appendPlainText(QString("[QCefWidget] urlChanged, isMainFrame: %1, url: %2\r\n").arg(isMainFrame).arg(url));
  });

  connect(cefOpenGLWidget_, &QCefOpenGLWidget::urlChanged, this, [this](bool isMainFrame, QString url) {
    plainTextEditLog_->appendPlainText(QString("[QCefOpenGLWidget] urlChanged, isMainFrame: %1, url: %2\r\n").arg(isMainFrame).arg(url));
  });

  connect(checkboxOsrEnabled_, &QCheckBox::stateChanged, this, [this](int state) {
    if (cefWidget_)
      cefWidget_->setOsrEnabled(state == Qt::Checked);
    if (cefOpenGLWidget_)
      cefOpenGLWidget_->setOsrEnabled(state == Qt::Checked);
    if (transWidgetCefWnd_)
      transWidgetCefWnd_->setOsrEnabled(state == Qt::Checked);
    if (transOpenGLWidgetCefWnd_)
      transOpenGLWidgetCefWnd_->setOsrEnabled(state == Qt::Checked);
  });

  connect(checkboxContextMenuEnabled_, &QCheckBox::stateChanged, this, [this](int state) {
    if (cefWidget_)
      cefWidget_->setContextMenuEnabled(state == Qt::Checked);
    if (cefOpenGLWidget_)
      cefOpenGLWidget_->setContextMenuEnabled(state == Qt::Checked);
  });
}

void TestWnd::onInvokeMethodNotify(int browserId, int frameId, const QString &method, const QVariantList &arguments) {
  QString str = QString("[InvokeMethodNotify] browserId: %1, frameId: %2, method: %3\r\n").arg(browserId).arg(frameId).arg(method);
  QString strArgs;
  for (int i = 0; i < arguments.size(); i++) {
    strArgs += QString("[%1]: %2\r\n").arg(i).arg(arguments[i].toString());
  }
  plainTextEditLog_->appendPlainText(str + strArgs);
}
