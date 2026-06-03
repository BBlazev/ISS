#pragma once
#include <QDialog>

namespace Ui { class TEST; }

class TestDialog : public QDialog {
    Q_OBJECT
public:
    explicit TestDialog(QWidget* parent = nullptr);
    ~TestDialog();
private:
    Ui::TEST* ui;
};
