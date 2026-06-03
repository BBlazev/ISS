#include <QApplication>
#include "testdialog.hpp"

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    TestDialog dlg;
    dlg.show();
    return app.exec();
}
