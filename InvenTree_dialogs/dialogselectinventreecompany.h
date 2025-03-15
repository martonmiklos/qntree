#pragma once

#include "gen_src/client/CompanyApi.h"
#include "wizard/models/companymodel.h"
#include <QDialog>

namespace Ui {
class DialogSelectInvenTreeCompany;
}

class DialogSelectInvenTreeCompany : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSelectInvenTreeCompany(InvenTree::CompanyApi *api, QWidget *parent = nullptr);
    ~DialogSelectInvenTreeCompany();

    void listOnlyManufacturers(bool filter);
    void update();

private slots:
    void on_tableView_doubleClicked(const QModelIndex &index);
    void on_tableViewCompanyList_doubleClicked(const QModelIndex &index);

private:
    Ui::DialogSelectInvenTreeCompany *ui;
    InvenTreeCompanyModel *m_model = nullptr;

signals:
    void companySelected(int pk, const QString &companyName);
};
