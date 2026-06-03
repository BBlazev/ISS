#include "testdialog.hpp"
#include "ui_test.h"

TestDialog::TestDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::TEST) 
{
    ui->setupUi(this);
}

TestDialog::~TestDialog() { delete ui; }
