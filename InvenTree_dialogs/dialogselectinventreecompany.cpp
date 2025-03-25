#include "dialogselectinventreecompany.h"
#include "ui_dialogselectinventreecompany.h"

DialogSelectInvenTreeCompany::DialogSelectInvenTreeCompany(InvenTree::CompanyApi *api, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogSelectInvenTreeCompany)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);

    m_model = new InvenTreeCompanyModel(api, this);
    ui->tableViewCompanyList->setModel(m_model);
    ui->tableViewCompanyList->horizontalHeader()->setSectionResizeMode(InvenTreeCompanyModel::Id, QHeaderView::ResizeToContents);
    ui->tableViewCompanyList->horizontalHeader()->setSectionResizeMode(InvenTreeCompanyModel::CompanyName, QHeaderView::Stretch);
    ui->tableViewCompanyList->horizontalHeader()->setSectionResizeMode(InvenTreeCompanyModel::Website, QHeaderView::ResizeToContents);

    connect(api, &InvenTree::CompanyApi::companyListSignalError, this, [=](InvenTree::PaginatedCompanyList, QNetworkReply::NetworkError, const QString &error_str) {
        ui->labelError->setText(error_str);
    });
}

DialogSelectInvenTreeCompany::~DialogSelectInvenTreeCompany()
{
    delete ui;
}

void DialogSelectInvenTreeCompany::listOnlyManufacturers(bool filter)
{
    m_model->setListOnlyManufacturers(filter);
}

void DialogSelectInvenTreeCompany::update()
{
    m_model->update();
}

void DialogSelectInvenTreeCompany::on_tableView_doubleClicked(const QModelIndex &index)
{
    emit companySelected(m_model->data(m_model->index(index.row(), InvenTreeCompanyModel::Id)).toInt(),
                         m_model->data(m_model->index(index.row(), InvenTreeCompanyModel::CompanyName)).toString());
}


void DialogSelectInvenTreeCompany::on_tableViewCompanyList_doubleClicked(const QModelIndex &index)
{
    emit companySelected(m_model->data(m_model->index(index.row(), InvenTreeCompanyModel::Id)).toInt(),
                         m_model->data(m_model->index(index.row(), InvenTreeCompanyModel::CompanyName)).toString());
    close();
}

