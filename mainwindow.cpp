#include "mainwindow.h"
#include "gen_src/client/StockApi.h"
#include "ui_mainwindow.h"
#include "supplier/supplierregistry.h"

using namespace InvenTree;

#include "inventreepartimportwizard.h"

#include "db/config_db.h"

#include "inventreesettingsdialog.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ConfigDb::instance()->init();

    ui->setupUi(this);

    m_partApi = new InvenTree::PartApi();

    m_stockApi = new InvenTree::StockApi();
    m_currencyApi = new InvenTree::CurrencyApi();
    m_companyApi = new InvenTree::CompanyApi();
    m_companyApi = new InvenTree::CompanyApi();
    m_attachmentApi = new InvenTree::AttachmentApi();
    updateInvenTreeToken();

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
    InvenTreePartImportWizard *wizard = new InvenTreePartImportWizard(m_partApi, m_stockApi, m_currencyApi, m_companyApi, m_attachmentApi, this);
    wizard->show();
}

void MainWindow::on_actionInvenTree_access_triggered()
{
    if (!m_inventreeSettingsDlg) {
        m_inventreeSettingsDlg = new InventreeSettingsDialog(this);
        connect(m_inventreeSettingsDlg, &InventreeSettingsDialog::accepted, this, &MainWindow::updateInvenTreeToken);
    }
    m_inventreeSettingsDlg->show();
}

void MainWindow::updateInvenTreeToken()
{
    m_settings.beginGroup("InventTree");
    auto token = m_settings.value(InventreeSettingsDialog::KEY_SERVER).toString();
    m_settings.endGroup();

    m_partApi->addHeaders("Authorization", "Token " + token);
    m_stockApi->addHeaders("Authorization", "Token " + token);
    m_currencyApi->addHeaders("Authorization", "Token " + token);
    m_companyApi->addHeaders("Authorization", "Token " + token);
    m_companyApi->addHeaders("Authorization", "Token " + token);
    m_attachmentApi->addHeaders("Authorization", "Token " + token);
}

