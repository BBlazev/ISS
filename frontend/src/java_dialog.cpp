#include <qstyle.h>
#include <qwidget.h>

#include "java_dialog.hpp"

JavaDialog::JavaDialog(ApiClient *api, QWidget *parent) : QDialog(parent), api_(api) {
    ui_.setupUi(this);

    connect(ui_.pushButton, &QPushButton::clicked, this, &JavaDialog::onPushButton_clicked);
}

void JavaDialog::onPushButton_clicked() {
    Result res;
    res = api_->java_validate();

    ui_.plainTextEdit->setPlainText(QString::fromStdString(res.output));
}
