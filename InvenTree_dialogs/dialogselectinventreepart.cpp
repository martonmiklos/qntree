#include "dialogselectinventreepart.h"
#include "ui_dialogselectinventreepart.h"

DialogSelectInvenTreePart::DialogSelectInvenTreePart(InvenTree::PartApi *api, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogSelectInvenTreePart)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    m_model = new PartListModel(api, this);
    m_model->setFilter(QString());
    ui->tableView->setModel(m_model);
    ui->tableView->horizontalHeader()->setSectionResizeMode(PartListModel::Col_IPN, QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setSectionResizeMode(PartListModel::Col_Name, QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setSectionResizeMode(PartListModel::Col_Description, QHeaderView::Stretch);
}

DialogSelectInvenTreePart::~DialogSelectInvenTreePart()
{
    delete ui;
}

void DialogSelectInvenTreePart::on_lineEditFilter_textChanged(const QString &arg1)
{
    m_model->setFilter(arg1);
}


void DialogSelectInvenTreePart::on_tableView_doubleClicked(const QModelIndex &index)
{
    emit partSelected(m_model->partAt(index.row()));
    close();
}

