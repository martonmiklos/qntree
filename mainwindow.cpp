#include "mainwindow.h"
#include "InvenTree_dialogs/dialogselectinventreecategory.h"
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
    m_partApi->addHeaders("Authorization", "Token inv-cfdeb7eb1b8a30c013ecf21db9870816955dc557-20250325");

    m_stockApi = new InvenTree::StockApi();
    m_stockApi->addHeaders("Authorization", "Token inv-cfdeb7eb1b8a30c013ecf21db9870816955dc557-20250325");

    m_currencyApi = new InvenTree::CurrencyApi();
    m_currencyApi->addHeaders("Authorization", "Token inv-cfdeb7eb1b8a30c013ecf21db9870816955dc557-20250325");

    m_companyApi = new InvenTree::CompanyApi();
    m_companyApi->addHeaders("Authorization", "Token inv-cfdeb7eb1b8a30c013ecf21db9870816955dc557-20250325");

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

