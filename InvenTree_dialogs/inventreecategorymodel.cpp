#include "inventreecategorymodel.h"

InvenTreeCategoryModel::InvenTreeCategoryModel(InvenTree::PartApi *api, QObject *parent)
    : QAbstractItemModel(parent)
{
    InvenTree::Category category;
    category.setPk(0);
    m_rootItem = new InvenTreeCategoryItem(api, category);
    m_rootItem->fetchChilds();
    connect(m_rootItem, &InvenTreeCategoryItem::childsFetched, this, [=](int childCount) {
        beginInsertRows(QModelIndex(), 0, childCount - 1);
        endInsertRows();
    });
}

InvenTreeCategoryModel::~InvenTreeCategoryModel()
{
    delete m_rootItem;
}

void InvenTreeCategoryModel::addCategory(int pk)
{
    bool found = false;
    m_rootItem->findIndexOfCategory(pk, &found);
    if (!found) {
        // TODO recursive query upwards until a parent category found
    }
}

QVariant InvenTreeCategoryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section)
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        return tr("Category name");
    }
    return QVariant();
}

QModelIndex InvenTreeCategoryModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    InvenTreeCategoryItem *parentItem;
    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<InvenTreeCategoryItem*>(parent.internalPointer());

    InvenTreeCategoryItem *childItem = parentItem->child(row);
    if (childItem) {
        auto index = createIndex(row, column, childItem);
        childItem->setIndex(index);
        return index;
    }
    return QModelIndex();
}

QModelIndex InvenTreeCategoryModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    InvenTreeCategoryItem *childItem = static_cast<InvenTreeCategoryItem*>(index.internalPointer());
    InvenTreeCategoryItem *parentItem = childItem->parentItem();

    if (parentItem == m_rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int InvenTreeCategoryModel::rowCount(const QModelIndex &parent) const
{
    InvenTreeCategoryItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<InvenTreeCategoryItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int InvenTreeCategoryModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 1;
}

QVariant InvenTreeCategoryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        InvenTreeCategoryItem *item = static_cast<InvenTreeCategoryItem*>(index.internalPointer());
        return item->categoryData.getName();
    } else if (role == IsStructuralRole) {
        InvenTreeCategoryItem *item = static_cast<InvenTreeCategoryItem*>(index.internalPointer());
        return item->categoryData.isStructural();
    } else if (role == PkRole) {
        InvenTreeCategoryItem *item = static_cast<InvenTreeCategoryItem*>(index.internalPointer());
        return item->categoryData.getPk();
    } else if (role == CategoryPathRole) {
        InvenTreeCategoryItem *item = static_cast<InvenTreeCategoryItem*>(index.internalPointer());
        return item->categoryData.getPathstring();
    }
    return QVariant();
}

bool InvenTreeCategoryModel::hasChildren(const QModelIndex &parent) const
{
    auto item = static_cast<const InvenTreeCategoryItem*>(parent.internalPointer());
    if (item) {
        if (!item->areChildsFetched())
            return true;
        return item->childCount() != 0;
    }
    return true;
}

bool InvenTreeCategoryModel::canFetchMore(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return false;

    InvenTreeCategoryItem *item = static_cast<InvenTreeCategoryItem*>(parent.internalPointer());
    if (item)
        return !item->areChildsFetched();
    return false;
}

void InvenTreeCategoryModel::fetchMore(const QModelIndex &parent)
{
    auto upper = static_cast<InvenTreeCategoryItem*>(parent.internalPointer());
    if (upper) {
        connect(upper, &InvenTreeCategoryItem::childsFetched, this, &InvenTreeCategoryModel::itemsChildsFetched);
        upper->fetchChilds();
    }
}

void InvenTreeCategoryModel::addPk(int pk)
{
    // recursively add the category upwards in the category tree
}

void InvenTreeCategoryModel::itemsChildsFetched(int childCount)
{
    auto parentItem = static_cast<InvenTreeCategoryItem*>(this->sender());
    disconnect(parentItem, &InvenTreeCategoryItem::childsFetched, this, &InvenTreeCategoryModel::itemsChildsFetched);
    beginInsertRows(parentItem->index(), 0, childCount - 1);
    endInsertRows();
}

InvenTreeCategoryItem::InvenTreeCategoryItem(InvenTree::PartApi *api, InvenTree::Category category, InvenTreeCategoryItem *parentItem) :
    QObject(parentItem),
    m_parentItem(parentItem),
    m_api(api)
{
    categoryData = category;
}

int InvenTreeCategoryItem::row() const
{
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<InvenTreeCategoryItem*>(this));
    return 0;
}

void InvenTreeCategoryItem::fetchChilds()
{
    if (m_childsFetched || m_fetchInProgress)
        return;
    m_fetchInProgress = true;
    InvenTree::OptionalParam<qint32> parent(categoryData.getPk(), categoryData.getPk() == 0);
    connect(m_api, &InvenTree::PartApi::partCategoryListSignal, this, &InvenTreeCategoryItem::subCategoriesReceieved);
    /*m_api->partCategoryList(InvenTree::OptionalParam<QString>(), //description
                            InvenTree::OptionalParam<qint32>(65535), //limit,
                            InvenTree::OptionalParam<QString>(), //name,
                            InvenTree::OptionalParam<qint32>(0), //offset,
                            InvenTree::OptionalParam<QString>(), //ordering,
                            parent,
                            InvenTree::OptionalParam<QString>(), // search,
                            InvenTree::OptionalParam<bool>() //structural
                            );*/
}

bool InvenTreeCategoryItem::areChildsFetched() const
{
    return m_childsFetched;
}

QModelIndex InvenTreeCategoryItem::index() const
{
    return m_index;
}

void InvenTreeCategoryItem::setIndex(const QModelIndex &newIndex)
{
    m_index = newIndex;
}

QModelIndex InvenTreeCategoryItem::findIndexOfCategory(int pk, bool *found) const
{
    if (categoryData.getPk() == pk)
        return m_index;

    for (const auto &child : m_childItems) {
        bool found_ = false;
        auto index = child->findIndexOfCategory(pk, &found_);
        if (found_) {
            if (found)
                *found = found_;
            return index;
        }
    }
    return QModelIndex();
}

void InvenTreeCategoryItem::subCategoriesReceieved(InvenTree::PaginatedCategoryList summary)
{
    disconnect(m_api, &InvenTree::PartApi::partCategoryListSignal, this, &InvenTreeCategoryItem::subCategoriesReceieved);

    auto categories = summary.getResults();
    for (const auto &category : categories) {
        if ((categoryData.getPk() == 0 && !category.is_parent_Set()) || category.getParent() == categoryData.getPk()) {
            auto newChild = new InvenTreeCategoryItem(m_api, category, this);
            m_childItems.append(newChild);
        } else {
            return;
        }
    }
    m_childsFetched = true;
    emit childsFetched(summary.getCount());
}
