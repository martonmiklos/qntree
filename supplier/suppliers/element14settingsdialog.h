#pragma once

#include <QDialog>
#include <QSettings>

namespace Ui {
class Element14SettingsDialog;
}

class Element14SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit Element14SettingsDialog(QWidget *parent = nullptr);
    ~Element14SettingsDialog();

    static constexpr const char *SETTINGS_GROUP = "Suppliers/Element14";
    static constexpr const char *KEY_API_KEY = "api_key";
    static constexpr const char *KEY_STORE_ID = "store_id";
    static constexpr const char *KEY_INVENTREE_SUPPLIER_ID = "inventree_supplier_id";

private slots:
    void saveSettings();

private:
    void loadSettings();

    Ui::Element14SettingsDialog *ui;
    QSettings m_settings;
};
