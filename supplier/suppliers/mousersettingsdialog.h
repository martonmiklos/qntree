#pragma once

#include "suppliersettingsdialog.h"


namespace Ui {
class MouserSettingsDialog;
}

class MouserSettingsDialog : public SupplierSettingsDialog
{
    Q_OBJECT

public:
    explicit MouserSettingsDialog(QWidget *parent = nullptr);
    ~MouserSettingsDialog();

    void loadSettings() override;


private slots:
    void saveSettings() override;

private:
    Ui::MouserSettingsDialog *ui;
};
