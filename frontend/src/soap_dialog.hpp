#pragma once

#include <qdialog.h>
#include <qwidget.h>

#include "api_client.hpp"
#include "ui_soap_dialog.h"

class ApiClient;

class SoapDialog : public QDialog {
    Q_OBJECT
  public:
    explicit SoapDialog(ApiClient *api, QWidget *parent = nullptr);

  private slots:

    void onSearchClicked();

  private:
    ApiClient *api_;
    Ui::SoapDialog_UI ui_;
};
