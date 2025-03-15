#pragma once

#include <QAbstractTableModel>

#include "gen_src/client/CompanyApi.h"


class InvenTreeCompanyModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Column {
        Id,
        CompanyName,
        Website,
        Column_Invalid,
    };

    class CompanyDetails {
    public:
        CompanyDetails(InvenTree::Company c);
        int pk;
        QString name;
        QString website;
    };

    explicit InvenTreeCompanyModel(InvenTree::CompanyApi *api, QObject *parent = nullptr);

    void update();

    // Header:
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    void setListOnlyManufacturers(bool only);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    InvenTree::CompanyApi *m_api = nullptr;

    // TODO make filterable by each mfr role...
    bool m_listOnlyManufacturers = true;
    QList<CompanyDetails> m_companyList;

private slots:
    void companyListReceived(InvenTree::PaginatedCompanyList result);
};


