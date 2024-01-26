#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace InvenTree;

#include "inventreepartimportwizard.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_apiInstance = new InvenTree::ActionApi();

    m_partApi = new InvenTree::PartApi();
    m_partApi->addHeaders("Authorization", "Token inv-d7e8312f7b53a59290faa71708264247f0c21c11-20231228");

    on_pushButtonImportParts_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButtonConnect_clicked()
{
    m_partApi->partCategoryParameters_List(1000, 0);
}


void MainWindow::on_pushButtonImportParts_clicked()
{
    InvenTreePartImportWizard *wizard = new InvenTreePartImportWizard(m_partApi, this);
    wizard->show();
}

