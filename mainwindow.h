#pragma once

#include "gen_src/client/AttachmentApi.h"
#include "gen_src/client/CompanyApi.h"
#include <QMainWindow>
#include <QSettings>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#include "gen_src/client/PartApi.h"
#include "gen_src/client/StockApi.h"
#include "gen_src/client/CurrencyApi.h"

class InventreeSettingsDialog;
class MouserSettingsDialog;
class TMESettingsDialog;
class Element14SettingsDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButtonConnect_clicked();
    void on_pushButtonImportParts_clicked();
    void on_actionInvenTree_access_triggered();
    void on_actionMouser_triggered();
    void on_actionTME_triggered();
    void on_actionElement14_triggered();

    void updateInvenTreeToken();

private:
    Ui::MainWindow *ui;
    InvenTree::PartApi *m_partApi = nullptr;
    InvenTree::StockApi *m_stockApi = nullptr;
    InvenTree::CurrencyApi *m_currencyApi = nullptr;
    InvenTree::CompanyApi *m_companyApi = nullptr;
    InvenTree::AttachmentApi *m_attachmentApi = nullptr;
    QSettings m_settings;
    InventreeSettingsDialog *m_inventreeSettingsDlg = nullptr;
    MouserSettingsDialog *m_mouserSettingsDialog = nullptr;
    TMESettingsDialog *m_tmeSettingsDialog = nullptr;
    Element14SettingsDialog *m_element14SettingsDialog = nullptr;
};
