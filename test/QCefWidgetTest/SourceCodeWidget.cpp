#include "SourceCodeWidget.h"
#include <QNetworkReply>
#include <QNetworkProxy>
#include <QNetworkAccessManager>
#include <QCoreApplication>
#include <QFile>
#include <QIcon>

SourceCodeWidget::SourceCodeWidget(QWidget* parent)
  : QDialog(parent,
            Qt::WindowSystemMenuHint | Qt::WindowTitleHint |
              Qt::WindowCloseButtonHint)
  , reply_(nullptr) {
  this->setWindowIcon(QIcon(":/QCefWidgetTest/images/weixin.svg"));
  setAttribute(Qt::WA_DeleteOnClose, true);
  setWindowTitle("Get source code");

  weiXinQRCodeWidget_ = new QWidget();
  weiXinQRCodeWidget_->setObjectName("weiXinQRCodeWidget");
  weiXinQRCodeWidget_->setSizePolicy(QSizePolicy::Expanding,
                                     QSizePolicy::Expanding);

  QHBoxLayout* hlEmail = new QHBoxLayout();
  hlEmail->addWidget(new QLabel("Email:  "));
  lineEditEmail_ = new QLineEdit("winsoft666@outlook.com");
  lineEditEmail_->setReadOnly(true);
  lineEditEmail_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  lineEditEmail_->setStyleSheet("border:none;");
  hlEmail->addWidget(lineEditEmail_);

  QHBoxLayout* hlGithub = new QHBoxLayout();
  hlGithub->addWidget(new QLabel("Github: "));
  lineEditGithub_ = new QLineEdit("https://github.com/winsoft666");
  lineEditGithub_->setReadOnly(true);
  lineEditGithub_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  lineEditGithub_->setStyleSheet("border:none;");
  hlGithub->addWidget(lineEditGithub_);

  QHBoxLayout* hlGitee = new QHBoxLayout();
  hlGitee->addWidget(new QLabel("Gitee:  "));
  lineEditGitee_ = new QLineEdit("https://gitee.com/china_jeffery");
  lineEditGitee_->setReadOnly(true);
  lineEditGitee_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  lineEditGitee_->setStyleSheet("border:none;");
  hlGitee->addWidget(lineEditGitee_);

  QVBoxLayout* vlMain = new QVBoxLayout();
  vlMain->addWidget(weiXinQRCodeWidget_);
  vlMain->addLayout(hlEmail);
  vlMain->addLayout(hlGitee);
  vlMain->addLayout(hlGithub);


  this->setLayout(vlMain);

  setFixedSize(430, 430);

  manager_ = new QNetworkAccessManager(this);
  QNetworkProxy proxy;
  proxy.setType(QNetworkProxy::NoProxy);
  manager_->setProxy(proxy);

  QString weiXinImgSrc = QCoreApplication::applicationDirPath() + "/weixin.jpg";

  connect(manager_,
          &QNetworkAccessManager::finished,
          this,
          [this, weiXinImgSrc](QNetworkReply* reply) {
            if (reply->error() == QNetworkReply::NoError) {
              QFile f(weiXinImgSrc);
              f.open(QFile::WriteOnly);
              if (f.isOpen()) {
                f.write(reply->readAll());
                f.close();
                weiXinQRCodeWidget_->setStyleSheet(
                  QString("QWidget{image: url(%1);}").arg(weiXinImgSrc));
              }
            }
          });

  if (QFile::exists(weiXinImgSrc)) {
    weiXinQRCodeWidget_->setStyleSheet(
      QString("QWidget{image: url(%1);}").arg(weiXinImgSrc));
  }
  else {
    reply_ = manager_->get(QNetworkRequest(QString(
      "https://gitee.com/china_jeffery/myprofile/raw/master/weixin.jpg")));
  }
}

SourceCodeWidget::~SourceCodeWidget() {
  if (reply_) {
    if (reply_->isRunning())
      reply_->abort();
  }
}
