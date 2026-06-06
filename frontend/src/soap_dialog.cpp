

#include <QFile>
#include <qabstractsocket.h>
#include <qdialog.h>
#include <qevent.h>
#include <qstringview.h>
#include <qwidget.h>

#include "soap_dialog.hpp"

#include "api_client.hpp"

SoapDialog::SoapDialog(ApiClient *api, QWidget *parent) : QDialog(parent), api_(api) {
    ui_.setupUi(this);
    ui_.searchBtn->setEnabled(true);
    ui_.label->setText("Enter a word to find: ");

    connect(ui_.searchBtn, &QPushButton::clicked, this, &SoapDialog::onSearchClicked);
}

static QByteArray makeFile(const std::string &term) {
    QByteArray xml;
    xml.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    xml.append("<SOAP-ENV:Envelope\n");
    xml.append("    xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"\n");
    xml.append("    xmlns:fs=\"http://localhost:8081/fs.wsdl\">\n");
    xml.append("  <SOAP-ENV:Body>\n");
    xml.append("    <fs:searchFiles>\n");
    xml.append("      <term>");
    xml.append(QString::fromStdString(term).toHtmlEscaped().toUtf8());
    xml.append("</term>\n");
    xml.append("    </fs:searchFiles>\n");
    xml.append("  </SOAP-ENV:Body>\n");
    xml.append("</SOAP-ENV:Envelope>\n");
    return xml;
}

void SoapDialog::onSearchClicked() {
    std::string term = ui_.lineEdit->text().toStdString();

    QByteArray file = makeFile(term);
    //    ui_.searchBtn->setEnabled(true);
    api_->soapFile(file, [this](bool ok, QString data, QString err) {
        if (!ok) {
            ui_.fieldTxt->setPlainText("Error: " + err);
            return;
        }
        ui_.fieldTxt->setPlainText(data.toUtf8());
    });
}
