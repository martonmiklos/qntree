#include "companymodel.h"


InvenTreeCompanyModel::InvenTreeCompanyModel(InvenTree::CompanyApi *api, QObject *parent)
    : QAbstractTableModel(parent),
    m_api(api)
{

}

void InvenTreeCompanyModel::update()
{
    InvenTree::OptionalParam<bool> isActive;
    InvenTree::OptionalParam<bool> isManufacturer;
    InvenTree::OptionalParam<bool> isSupplier;
    InvenTree::OptionalParam<bool> isCustomer;
    if (m_listOnlyManufacturers) {
        isManufacturer.m_Value = true;
        isManufacturer.m_hasValue = true;
    }
    connect(m_api, &InvenTree::CompanyApi::companyListSignal, this, &InvenTreeCompanyModel::companyListReceived);
    m_api->companyList(isActive,
                       isCustomer,
                       isManufacturer,
                       isSupplier,
                       InvenTree::OptionalParam<qint32>(std::numeric_limits<qint32>().max()),
                       InvenTree::OptionalParam<QString>(),
                       InvenTree::OptionalParam<qint32>(0));

    /*  const ::InvenTree::OptionalParam<bool> &active = ::InvenTree::OptionalParam<bool>(),
        const ::InvenTree::OptionalParam<bool> &is_customer = ::InvenTree::OptionalParam<bool>(),
        const ::InvenTree::OptionalParam<bool> &is_manufacturer = ::InvenTree::OptionalParam<bool>(),
        const ::InvenTree::OptionalParam<bool> &is_supplier = ::InvenTree::OptionalParam<bool>(),
        const ::InvenTree::OptionalParam<qint32> &limit = ::InvenTree::OptionalParam<qint32>(),
        const ::InvenTree::OptionalParam<QString> &name = ::InvenTree::OptionalParam<QString>(),
        const ::InvenTree::OptionalParam<qint32> &offset = ::InvenTree::OptionalParam<qint32>(),
        const ::InvenTree::OptionalParam<QString> &ordering = ::InvenTree::OptionalParam<QString>(),
        const ::InvenTree::OptionalParam<QString> &search = ::InvenTree::OptionalParam<QString>()*/
}

QVariant InvenTreeCompanyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (static_cast<Column>(section)) {
        case Id:
            return tr("#");
        case CompanyName:
            return tr("Name");
        case Website:
            return tr("Website");
        case Column_Invalid:
            break;
        }
    }
    return QVariant();
}

int InvenTreeCompanyModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_companyList.count();
}

int InvenTreeCompanyModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return Column_Invalid;
}

void InvenTreeCompanyModel::setListOnlyManufacturers(bool only)
{
    m_listOnlyManufacturers = only;
}

QVariant InvenTreeCompanyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (role == Qt::DisplayRole) {
        switch (static_cast<Column>(index.column())) {
        case Id:
            return m_companyList.at(index.row()).pk;
        case CompanyName:
            return m_companyList.at(index.row()).name;
        case Website:
            return m_companyList.at(index.row()).website;
        case Column_Invalid:
            break;
        }
    }
    return QVariant();
}

void InvenTreeCompanyModel::companyListReceived(InvenTree::PaginatedCompanyList result)
{
    disconnect(m_api, &InvenTree::CompanyApi::companyListSignal, this, &InvenTreeCompanyModel::companyListReceived);
    beginResetModel();
    m_companyList.clear();
    for (const auto &company : result.getResults()) {
        m_companyList.append(CompanyDetails(company));
    }
    endResetModel();
}

InvenTreeCompanyModel::CompanyDetails::CompanyDetails(InvenTree::Company c)
{
    pk = c.getPk();
    name = c.getName();
    website = c.getWebsite();
}
