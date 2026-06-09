#include <qwidget.h>

#include "api_client.hpp"
#include "ui_java_dialog.h"

class JavaDialog : public QDialog {
    Q_OBJECT

  public:
    JavaDialog(ApiClient *api, QWidget *parent = nullptr);

  public slots:

    void onPushButton_clicked();

  private:
    Ui::JavaDialog ui_;
    ApiClient *api_;
};
