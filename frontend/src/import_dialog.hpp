#pragma once

#include <QByteArray>
#include <QDialog>
#include <qdialog.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qwidget.h>

#include "api_client.hpp"
#include "ui_import_dialog.h"

class ApiClient;

class ImportDialog : public QDialog {
    Q_OBJECT

  public:
    explicit ImportDialog(ApiClient *api, QWidget *parent = nullptr);

  private slots:
    void on_selectXmlBtn_clicked();
    void on_selectJsonBtn_clicked();
    void on_importBtn_clicked();

  private:
    Ui::Dialog ui_;

    ApiClient *api_;
    QByteArray xml_;
    QByteArray json_;
};
