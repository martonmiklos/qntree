#include "inventreestocklocationmodel.h"

InvenTreeStockLocationModel::InvenTreeStockLocationModel(InvenTree::StockApi *api, QObject *parent)
    : QAbstractItemModel(parent), m_api(api)
{
    InvenTree::Location location;
    location.setPk(0);
    m_rootItem = new InvenTreeStockLocationItem(api, location);
    connect(m_rootItem, &InvenTreeStockLocationItem::childsFetched, this, [=](int childCount) {
        beginInsertRows(QModelIndex(), 0, childCount - 1);
        endInsertRows();
        emit dataFetched();
        m_topLevelLocationsFetched = true;
        if (m_populatetreeToSelectedLocation)
            populateParentsRecursivelyToTop(m_preSelectedPk);
    });
    m_rootItem->fetchChilds();
}

InvenTreeStockLocationModel::~InvenTreeStockLocationModel()
{
    delete m_rootItem;
}

QVariant InvenTreeStockLocationModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section)
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        return tr("Stock location name");
    }
    return QVariant();
}

QModelIndex InvenTreeStockLocationModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    InvenTreeStockLocationItem *parentItem;
    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<InvenTreeStockLocationItem*>(parent.internalPointer());

    InvenTreeStockLocationItem *childItem = parentItem->child(row);
    if (childItem) {
        auto index = createIndex(row, column, childItem);
        childItem->setIndex(index);
        return index;
    }
    return QModelIndex();
}

QModelIndex InvenTreeStockLocationModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    InvenTreeStockLocationItem *childItem = static_cast<InvenTreeStockLocationItem*>(index.internalPointer());
    InvenTreeStockLocationItem *parentItem = childItem->parentItem();

    if (parentItem == m_rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int InvenTreeStockLocationModel::rowCount(const QModelIndex &parent) const
{
    InvenTreeStockLocationItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<InvenTreeStockLocationItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int InvenTreeStockLocationModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 1;
}

QVariant InvenTreeStockLocationModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        InvenTreeStockLocationItem *item = static_cast<InvenTreeStockLocationItem*>(index.internalPointer());
        if (item->locationData.getDescription().isEmpty())
            return item->locationData.getName();
        return QString("%1 (%2)").arg(item->locationData.getName(), item->locationData.getDescription());
    } else if (role == IsStructuralRole) {
        InvenTreeStockLocationItem *item = static_cast<InvenTreeStockLocationItem*>(index.internalPointer());
        return item->locationData.isStructural();
    } else if (role == PkRole) {
        InvenTreeStockLocationItem *item = static_cast<InvenTreeStockLocationItem*>(index.internalPointer());
        return item->locationData.getPk();
    } else if (role == LocationPathRole) {
        InvenTreeStockLocationItem *item = static_cast<InvenTreeStockLocationItem*>(index.internalPointer());
        return item->locationData.getPathstring();
    }
    return QVariant();
}

bool InvenTreeStockLocationModel::hasChildren(const QModelIndex &parent) const
{
    auto item = static_cast<const InvenTreeStockLocationItem*>(parent.internalPointer());
    if (item) {
        if (!item->areChildsFetched())
            return true;
        return item->childCount() != 0;
    }
    return true;
}

bool InvenTreeStockLocationModel::canFetchMore(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return false;

    InvenTreeStockLocationItem *item = static_cast<InvenTreeStockLocationItem*>(parent.internalPointer());
    if (item)
        return !item->areChildsFetched();
    return false;
}

void InvenTreeStockLocationModel::fetchMore(const QModelIndex &parent)
{
    auto upper = static_cast<InvenTreeStockLocationItem*>(parent.internalPointer());
    if (upper) {
        connect(upper, &InvenTreeStockLocationItem::childsFetched, this, &InvenTreeStockLocationModel::itemsChildsFetched);
        upper->fetchChilds();
    }
}

void InvenTreeStockLocationModel::itemsChildsFetched(int childCount)
{
    auto parentItem = static_cast<InvenTreeStockLocationItem*>(this->sender());
    disconnect(parentItem, &InvenTreeStockLocationItem::childsFetched, this, &InvenTreeStockLocationModel::itemsChildsFetched);
    beginInsertRows(parentItem->index(), 0, childCount - 1);
    endInsertRows();
    if (!m_locationListToPopulate.isEmpty()) {
        for (int i = 0; i < parentItem->childCount(); ++i) {
            if (parentItem->child(i)->locationData.getPk() == m_locationListToPopulate.last()) {
                m_locationListToPopulate.takeLast();
                const QModelIndex childIndex = index(i, 0, parentItem->index());
                emit requestExpand(childIndex);
                if (m_locationListToPopulate.isEmpty())
                    emit requestSelection(childIndex);
                break;
            }
        }
    }
    emit dataFetched();
}

void InvenTreeStockLocationModel::setSelectedPk(int pk)
{
    m_preSelectedPk = pk;
    if (m_rootItem->hasChildPk(pk)) {
        emit requestSelection(m_rootItem->childByPk(pk)->index());
    } else if (m_topLevelLocationsFetched) {
        populateParentsRecursivelyToTop(pk);
    } else {
        m_populatetreeToSelectedLocation = true;
    }
}

void InvenTreeStockLocationModel::populateParentsRecursivelyToTop(int pk)
{
    m_populatetreeToSelectedLocation = false;
    connect(m_api, &InvenTree::StockApi::stockLocationRetrieveSignal,
            this, &InvenTreeStockLocationModel::parentLocationFetchedForSelection);
    m_locationListToPopulate.clear();
    fetchParentLocation(pk);
}

void InvenTreeStockLocationModel::fetchParentLocation(int pk)
{
    m_api->stockLocationRetrieve(pk);
}

void InvenTreeStockLocationModel::parentLocationFetchedForSelection(InvenTree::Location locationData)
{
    if (m_rootItem->hasChildPk(locationData.getPk())) {
        disconnect(m_api, &InvenTree::StockApi::stockLocationRetrieveSignal,
                   this, &InvenTreeStockLocationModel::parentLocationFetchedForSelection);
        emit requestExpand(m_rootItem->childByPk(locationData.getPk())->index());
    } else if (!locationData.is_parent_Set()) {
        disconnect(m_api, &InvenTree::StockApi::stockLocationRetrieveSignal,
                   this, &InvenTreeStockLocationModel::parentLocationFetchedForSelection);
        beginInsertRows(QModelIndex(), m_rootItem->childCount(), m_rootItem->childCount());
        auto newTopLevelItem = new InvenTreeStockLocationItem(m_api, locationData, m_rootItem);
        m_rootItem->appendChild(newTopLevelItem);
        endInsertRows();
        emit requestExpand(index(m_rootItem->childCount() - 1, 0));
    } else {
        m_locationListToPopulate.append(locationData.getPk());
        fetchParentLocation(locationData.getParent());
    }
}

InvenTreeStockLocationItem::InvenTreeStockLocationItem(InvenTree::StockApi *api, InvenTree::Location location, InvenTreeStockLocationItem *parentItem) :
    QObject(parentItem),
    m_parentItem(parentItem),
    m_api(api)
{
    locationData = location;
}

int InvenTreeStockLocationItem::row() const
{
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<InvenTreeStockLocationItem*>(this));
    return 0;
}

void InvenTreeStockLocationItem::appendChild(InvenTreeStockLocationItem *child)
{
    child->setParentItem(this);
    m_childItems.append(child);
}

InvenTreeStockLocationItem *InvenTreeStockLocationItem::childByPk(int pk)
{
    for (auto child : std::as_const(m_childItems)) {
        if (child->locationData.getPk() == pk)
            return child;
    }
    return nullptr;
}

bool InvenTreeStockLocationItem::hasChildPk(int pk) const
{
    for (const auto *child : m_childItems) {
        if (child->locationData.getPk() == pk)
            return true;
    }
    return false;
}

void InvenTreeStockLocationItem::setParentItem(InvenTreeStockLocationItem *newParentItem)
{
    m_parentItem = newParentItem;
    setParent(newParentItem);
}

void InvenTreeStockLocationItem::fetchChilds()
{
    if (m_childsFetched || m_fetchInProgress)
        return;
    m_fetchInProgress = true;
    InvenTree::OptionalParam<qint32> parent(locationData.getPk(), locationData.getPk() == 0);
    connect(m_api, &InvenTree::StockApi::stockLocationListSignal, this, &InvenTreeStockLocationItem::subLocationsReceieved);
    //  const ::InvenTree::OptionalParam<QString> &ordering, const ::InvenTree::OptionalParam<QString> &search
    /*    void stockLocationList(
     *    const ::InvenTree::OptionalParam<bool> &cascade = ::InvenTree::OptionalParam<bool>(),
     *    const ::InvenTree::OptionalParam<double> &depth = ::InvenTree::OptionalParam<double>(),
     *    const ::InvenTree::OptionalParam<bool> &external = ::InvenTree::OptionalParam<bool>(),
     *    const ::InvenTree::OptionalParam<bool> &has_location_type = ::InvenTree::OptionalParam<bool>(),
     *    const ::InvenTree::OptionalParam<qint32> &limit = ::InvenTree::OptionalParam<qint32>(),
     *    const ::InvenTree::OptionalParam<qint32> &location_type = ::InvenTree::OptionalParam<qint32>(),
     *    const ::InvenTree::OptionalParam<QString> &name = ::InvenTree::OptionalParam<QString>(),
     *    const ::InvenTree::OptionalParam<qint32> &offset = ::InvenTree::OptionalParam<qint32>(),
     *    const ::InvenTree::OptionalParam<QString> &ordering = ::InvenTree::OptionalParam<QString>(),
     *    const ::InvenTree::OptionalParam<qint32> &parent = ::InvenTree::OptionalParam<qint32>(),
     *    const ::InvenTree::OptionalParam<QString> &search = ::InvenTree::OptionalParam<QString>(),
     *    const ::InvenTree::OptionalParam<bool> &structural = ::InvenTree::OptionalParam<bool>()); */

    m_api->stockLocationList(
        65535, // limit
        false, //cascade
        1, //depth
        InvenTree::OptionalParam<bool>(), //external
        InvenTree::OptionalParam<bool>(), //has_location_type
        InvenTree::OptionalParam<qint32>(), //location_type
        InvenTree::OptionalParam<QString>(), //name
        InvenTree::OptionalParam<qint32>(), //offset
        InvenTree::OptionalParam<QString>(), //ordering
        parent, //parent
        InvenTree::OptionalParam<bool>(), //path_detail
        InvenTree::OptionalParam<QString>(), //search
        InvenTree::OptionalParam<bool>(), //structural
        InvenTree::OptionalParam<bool>() //top_level
        );
}

bool InvenTreeStockLocationItem::areChildsFetched() const
{
    return m_childsFetched;
}

QModelIndex InvenTreeStockLocationItem::index() const
{
    return m_index;
}

void InvenTreeStockLocationItem::setIndex(const QModelIndex &newIndex)
{
    m_index = newIndex;
}

QModelIndex InvenTreeStockLocationItem::findIndexOfLocation(int pk, bool *found) const
{
    if (locationData.getPk() == pk)
        return m_index;

    for (const auto &child : m_childItems) {
        bool found_ = false;
        auto index = child->findIndexOfLocation(pk, &found_);
        if (found_) {
            if (found)
                *found = found_;
            return index;
        }
    }
    return QModelIndex();
}

void InvenTreeStockLocationItem::subLocationsReceieved(InvenTree::PaginatedLocationList summary)
{
    disconnect(m_api, &InvenTree::StockApi::stockLocationListSignal, this, &InvenTreeStockLocationItem::subLocationsReceieved);
    m_fetchInProgress = false;

    auto locations = summary.getResults();
    for (const auto &location : locations) {
        if ((locationData.getPk() == 0 && !location.is_parent_Set()) || location.getParent() == locationData.getPk()) {
            auto newChild = new InvenTreeStockLocationItem(m_api, location, this);
            m_childItems.append(newChild);
        } else {
            return;
        }
    }
    m_childsFetched = true;
    emit childsFetched(summary.getCount());
}
