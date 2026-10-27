#pragma once

#include "suppliersettingsdialog.h"

namespace Ui {
class Element14SettingsDialog;
}

class Element14SettingsDialog : public SupplierSettingsDialog
{
    Q_OBJECT

public:
    explicit Element14SettingsDialog(QWidget *parent = nullptr);
    ~Element14SettingsDialog();

    void loadSettings() override;

private slots:
    void saveSettings() override;

private:
    Ui::Element14SettingsDialog *ui;
};
