#include "dialogselectinventreecompany.h"
#include "ui_dialogselectinventreecompany.h"

#include <QHeaderView>

DialogSelectInvenTreeCompany::DialogSelectInvenTreeCompany(InvenTree::CompanyApi *api, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogSelectInvenTreeCompany)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);

    // Source model
    m_model = new InvenTreeCompanyModel(api, this);

    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_model);

    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    m_proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    ui->tableViewCompanyList->setSortingEnabled(true);

    m_proxyModel->setFilterKeyColumn(-1);

    ui->tableViewCompanyList->setModel(m_proxyModel);

    ui->tableViewCompanyList->horizontalHeader()->setSectionResizeMode(InvenTreeCompanyModel::Id, QHeaderView::ResizeToContents);
    ui->tableViewCompanyList->horizontalHeader()->setSectionResizeMode(InvenTreeCompanyModel::CompanyName, QHeaderView::Stretch);
    ui->tableViewCompanyList->horizontalHeader()->setSectionResizeMode(InvenTreeCompanyModel::Website, QHeaderView::ResizeToContents);

    connect(api, &InvenTree::CompanyApi::companyListSignalError, this,
            [=](InvenTree::PaginatedCompanyList, QNetworkReply::NetworkError, const QString &error_str) {
                ui->labelError->setText(error_str);
            });

    ui->lineEditFilter->clear();
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

void DialogSelectInvenTreeCompany::on_tableViewCompanyList_doubleClicked(const QModelIndex &index)
{
    const QModelIndex srcIndex = m_proxyModel->mapToSource(index);

    emit companySelected(
        m_model->data(m_model->index(srcIndex.row(), InvenTreeCompanyModel::Id)).toInt(),
        m_model->data(m_model->index(srcIndex.row(), InvenTreeCompanyModel::CompanyName)).toString()
        );

    close();
}

void DialogSelectInvenTreeCompany::on_lineEditFilter_textChanged(const QString &arg1)
{
    // Treat input as plain text (escape regex metacharacters)
    // and do a "contains" match.
    QRegularExpression re(QRegularExpression::escape(arg1), QRegularExpression::CaseInsensitiveOption);
    m_proxyModel->setFilterRegularExpression(re);
}
