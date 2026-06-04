#include <QFile>
#include <QFileDialog>
#include <nlohmann/json_fwd.hpp>
#include <qobject.h>
#include <qstringview.h>
#include <qwidget.h>

#include "import_dialog.hpp"

#include "api_client.hpp"

ImportDialog::ImportDialog(ApiClient *api, QWidget *parent) : QDialog(parent), api_(api) {
    ui_.setupUi(this);
    ui_.replyEdit->setReadOnly(true);
    ui_.importBtn->setEnabled(false);
}

static QByteArray readFile(const QString &path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return {};
    return file.readAll();
}

void ImportDialog::on_selectXmlBtn_clicked() {
    QString path = QFileDialog::getOpenFileName(this, "Select XML", {}, "XML(*.xml)");
    if (path.isEmpty())
        return;
    xml_ = readFile(path);
    ui_.importBtn->setEnabled(!xml_.isEmpty() && !json_.isEmpty());
}

void ImportDialog::on_selectJsonBtn_clicked() {
    QString path = QFileDialog::getOpenFileName(this, "Select JSON", {}, "JSON(*.json)");
    if (path.isEmpty())
        return;
    json_ = readFile(path);
    ui_.importBtn->setEnabled(!xml_.isEmpty() && !json_.isEmpty());
}

void ImportDialog::on_importBtn_clicked() {
    ui_.importBtn->setEnabled(false);
    ui_.replyEdit->setPlainText("Uploading...");

    api_->importFiles(xml_, json_, [this](bool ok, QString body, QString err) {
        if (ok)
            ui_.replyEdit->setPlainText(body);
        else
            ui_.replyEdit->setPlainText("Failed: " + err + "\n\n" + body);
        ui_.importBtn->setEnabled(true);
    });
}
