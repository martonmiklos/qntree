#pragma once

#include "gen_src/client/CompanyApi.h"
#include <QMainWindow>
#include <QSettings>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#include "gen_src/client/PartApi.h"
#include "gen_src/client/StockApi.h"
#include "gen_src/client/CurrencyApi.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButtonConnect_clicked();
    void on_pushButtonImportParts_clicked();

private:
    Ui::MainWindow *ui;
    InvenTree::PartApi *m_partApi = nullptr;
    InvenTree::StockApi *m_stockApi = nullptr;
    InvenTree::CurrencyApi *m_currencyApi = nullptr;
    InvenTree::CompanyApi *m_companyApi = nullptr;
    QSettings m_settings;
};
