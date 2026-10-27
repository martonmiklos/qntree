#pragma once

#include "suppliersettingsdialog.h"

namespace Ui {
class TMESettingsDialog;
}

class TMESettingsDialog : public SupplierSettingsDialog
{
    Q_OBJECT

public:
    explicit TMESettingsDialog(QWidget *parent = nullptr);
    ~TMESettingsDialog();

    void loadSettings() override;

private slots:
    void saveSettings() override;

private:

    Ui::TMESettingsDialog *ui;
};
