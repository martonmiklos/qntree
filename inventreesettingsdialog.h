#pragma once

#include <QDialog>
#include <QSettings>
#include <QCompleter>
#include <QStringListModel>

namespace Ui {
class InventreeSettingsDialog;
}

class InventreeSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InventreeSettingsDialog(QWidget *parent = nullptr);
    ~InventreeSettingsDialog();

    QString serverUrl() const;
    QString apiToken() const;

    static constexpr const char* KEY_SERVER = "server_url";
    static constexpr const char* KEY_TOKEN  = "api_token";
    static constexpr const char* KEY_URL_LIST = "server_url_history";

private slots:
    void saveSettings();

private:
    void loadSettings();
    void setupCompleter();

private:
    Ui::InventreeSettingsDialog *ui;

    QSettings m_settings;
    QStringListModel *m_urlModel;
    QCompleter *urlCompleter;
};
