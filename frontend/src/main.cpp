#include <QApplication>

#include "api_client.hpp"
#include "import_dialog.hpp"

int main(int argc, char **argv) {

    QApplication app(argc, argv);

    ApiClient api;

    ImportDialog dlg(&api);
    dlg.show();
    return app.exec();
}
