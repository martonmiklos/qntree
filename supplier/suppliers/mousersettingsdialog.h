#pragma once

#include <QDialog>
#include <QSettings>

namespace Ui {
class MouserSettingsDialog;
}

class MouserSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MouserSettingsDialog(QWidget *parent = nullptr);
    ~MouserSettingsDialog();

    static constexpr const char *SETTINGS_GROUP = "Suppliers/Mouser";
    static constexpr const char *KEY_API_KEY = "api_key";
    static constexpr const char *KEY_INVENTREE_SUPPLIER_ID = "inventree_supplier_id";

private slots:
    void saveSettings();

private:
    void loadSettings();

    Ui::MouserSettingsDialog *ui;
    QSettings m_settings;
};
