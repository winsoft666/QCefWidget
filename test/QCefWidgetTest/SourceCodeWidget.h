#pragma once
#include <QtWidgets>

class QNetworkAccessManager;
class QNetworkReply;
class SourceCodeWidget : public QDialog {
  Q_OBJECT
public:
  SourceCodeWidget(QWidget *parent = Q_NULLPTR);
  ~SourceCodeWidget();
protected:
  QNetworkAccessManager* manager_;
  QNetworkReply* reply_;
  QWidget* weiXinQRCodeWidget_;
  QLineEdit* lineEditEmail_;
  QLineEdit* lineEditGithub_;
  QLineEdit* lineEditGitee_;
};