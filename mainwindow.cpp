#include "mainwindow.h"
#include "gen_src/client/StockApi.h"
#include "ui_mainwindow.h"
#include "supplier/supplierregistry.h"

using namespace InvenTree;

#include "inventreepartimportwizard.h"

#include "db/config_db.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ConfigDb::instance()->init();

    ui->setupUi(this);

    m_partApi = new InvenTree::PartApi();
    m_partApi->addHeaders("Authorization", "Token inv-d7e8312f7b53a59290faa71708264247f0c21c11-20231228");

    m_stockApi = new InvenTree::StockApi();
    m_stockApi->addHeaders("Authorization", "Token inv-d7e8312f7b53a59290faa71708264247f0c21c11-20231228");

    m_currencyApi = new InvenTree::CurrencyApi();
    m_currencyApi->addHeaders("Authorization", "Token inv-d7e8312f7b53a59290faa71708264247f0c21c11-20231228");

    m_companyApi = new InvenTree::CompanyApi();
    m_companyApi->addHeaders("Authorization", "Token inv-d7e8312f7b53a59290faa71708264247f0c21c11-20231228");

    SupplierRegistry::instance(this);

    on_pushButtonImportParts_clicked();

    m_settings.beginGroup("MainWindow");
    restoreGeometry(m_settings.value("geometry").toByteArray());
    m_settings.endGroup();
}

MainWindow::~MainWindow()
{
    m_settings.beginGroup("MainWindow");
    m_settings.setValue("geometry", saveGeometry());
    m_settings.endGroup();
    delete ui;
}


void MainWindow::on_pushButtonConnect_clicked()
{
    m_partApi->partCategoryParametersList(1000, 0);
}


void MainWindow::on_pushButtonImportParts_clicked()
{
    InvenTreePartImportWizard *wizard = new InvenTreePartImportWizard(m_partApi, m_stockApi, m_currencyApi, m_companyApi, this);
    wizard->show();
}

