#pragma once

#include <QDialog>
#include <QSettings>

namespace Ui {
class TMESettingsDialog;
}

class TMESettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TMESettingsDialog(QWidget *parent = nullptr);
    ~TMESettingsDialog();

    static constexpr const char *SETTINGS_GROUP = "Suppliers/TME";
    static constexpr const char *KEY_SECRET = "secret";
    static constexpr const char *KEY_TOKEN = "token";
    static constexpr const char *KEY_COUNTRY = "country";
    static constexpr const char *KEY_LANGUAGE = "language";
    static constexpr const char *KEY_CURRENCY = "currency";
    static constexpr const char *KEY_GROSS_PRICES = "gross_prices";
    static constexpr const char *KEY_INVENTREE_SUPPLIER_ID = "inventree_supplier_id";

private slots:
    void saveSettings();

private:
    void loadSettings();

    Ui::TMESettingsDialog *ui;
    QSettings m_settings;
};
