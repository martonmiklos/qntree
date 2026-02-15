#include "inventreecategorymodel.h"
#include "gen_src/client/Category.h"

InvenTreeCategoryModel::InvenTreeCategoryModel(InvenTree::PartApi *api, QObject *parent)
    : QAbstractItemModel(parent), m_api(api)
{
    InvenTree::Category category;
    category.setPk(0);
    m_rootItem = new InvenTreeCategoryItem(api, category);
    connect(m_rootItem, &InvenTreeCategoryItem::childsFetched, this, [=](int childCount) {
        beginInsertRows(QModelIndex(), 0, childCount - 1);
        endInsertRows();
        emit dataFetched();
        m_topLevelCategoriesFetched = true;
        if (m_populatetreeToSelectedCategory) {
            populateParentsRecursivelyToTop(m_preSelectedPk);
        }
    });
    m_rootItem->fetchChilds();
}

InvenTreeCategoryModel::~InvenTreeCategoryModel()
{
    delete m_rootItem;
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
        connect(upper, &InvenTreeCategoryItem::childsFetched, this, &InvenTreeCategoryModel::childItemsFetched);
        upper->fetchChilds();
    }
}

void InvenTreeCategoryModel::childItemsFetched(int childCount)
{
    auto parentItem = static_cast<InvenTreeCategoryItem*>(this->sender());
    disconnect(parentItem, &InvenTreeCategoryItem::childsFetched, this, &InvenTreeCategoryModel::childItemsFetched);
    beginInsertRows(parentItem->index(), 0, childCount - 1);
    endInsertRows();
    if (m_categoryListToPopulate.length()) {
        for (int i = 0; i<parentItem->childCount(); i++) {
            if (parentItem->child(i)->categoryData.getPk() == m_categoryListToPopulate.last()) {
                m_categoryListToPopulate.takeLast();
                emit requestExpand(index(i, 0, parentItem->index()));
                if (m_categoryListToPopulate.length() == 0)
                    emit requestSelection(index(i, 0, parentItem->index()));
                break;
            }
        }
    }
    emit dataFetched();
}

void InvenTreeCategoryModel::setVisiblePk(int pk)
{
    m_preSelectedPk = pk;
    if (m_rootItem->hasChildPk(pk)) {
        // the selected category is a topmost category -> do nothing
    } else {
        // the selected category is not a topmost category -> start recursively selecting the parent categories
        if (m_topLevelCategoriesFetched) {
            populateParentsRecursivelyToTop(m_preSelectedPk);
        } else {
            // postpone recursive tree build until the top level categories are populated
            m_populatetreeToSelectedCategory = true;
        }
    }
}

void InvenTreeCategoryModel::populateParentsRecursivelyToTop(int pk)
{
    // recursively add the category upwards in the category tree
    m_populatetreeToSelectedCategory = false;
    connect(m_api, &InvenTree::PartApi::partCategoryRetrieveSignal, this, &InvenTreeCategoryModel::parentCategoryFetchedForSelection);
    m_categoryListToPopulate.clear();
    fetchParentCategory(pk);
}

void InvenTreeCategoryModel::fetchParentCategory(int pk)
{
    m_api->partCategoryRetrieve(pk);
}

void InvenTreeCategoryModel::parentCategoryFetchedForSelection(InvenTree::Category categoryData)
{
    if (m_rootItem->hasChildPk(categoryData.getPk())) {
        // we reached top level -> finished
        disconnect(m_api, &InvenTree::PartApi::partCategoryRetrieveSignal, this, &InvenTreeCategoryModel::parentCategoryFetchedForSelection);
        emit requestExpand(m_rootItem->childByPk(categoryData.getPk())->index());
    } else {
        if (categoryData.getPk() == 0) {
            // for some reason we reached a top level item while it was not added to the rootitem
            disconnect(m_api, &InvenTree::PartApi::partCategoryRetrieveSignal, this, &InvenTreeCategoryModel::parentCategoryFetchedForSelection);
            beginInsertRows(QModelIndex(), m_rootItem->childCount(), 1);
            auto newTopLevelItem = new InvenTreeCategoryItem(m_api, categoryData, m_rootItem);
            m_rootItem->appendChild(newTopLevelItem);
            endInsertRows();
            emit requestExpand(index(m_rootItem->childCount() - 1, 0));
        } else {
            m_categoryListToPopulate.append(categoryData.getPk());
            fetchParentCategory(categoryData.getParent());
        }
    }
}

InvenTreeCategoryItem::InvenTreeCategoryItem(InvenTree::PartApi *api, InvenTree::Category category, InvenTreeCategoryItem *parentItem) :
    QObject(parentItem),
    m_parentItem(parentItem),
    m_api(api)
{
    categoryData = category;
}

void InvenTreeCategoryItem::appendChild(InvenTreeCategoryItem *child)
{
    child->setParentItem(this);
    m_childItems.append(child);
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
    bool cascade = parent.stringValue() != "";
    m_api->partCategoryList(
        65535, // limit
        InvenTree::OptionalParam<bool>(), //cascade
        1, //depth
        InvenTree::OptionalParam<qint32>(), //exclude_tree
        InvenTree::OptionalParam<QString>(), //name
        0, //offset
        InvenTree::OptionalParam<QString>(), //ordering
        parent, //parent
        InvenTree::OptionalParam<bool>(), //path_detail
        InvenTree::OptionalParam<QString>(), //search
        InvenTree::OptionalParam<bool>(), //starred
        InvenTree::OptionalParam<bool>(), //structural
        InvenTree::OptionalParam<bool>() //top_level
        );
}

void InvenTreeCategoryItem::subCategoriesReceieved(InvenTree::PaginatedCategoryList summary)
{
    disconnect(m_api, &InvenTree::PartApi::partCategoryListSignal, this, &InvenTreeCategoryItem::subCategoriesReceieved);
    m_fetchInProgress = false;
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

bool InvenTreeCategoryItem::hasChildPk(const int pk) const
{
    for (const auto &child : m_childItems) {
        if (child->categoryData.getPk() == pk)
            return true;
    }
    return false;
}

void InvenTreeCategoryItem::setParentItem(InvenTreeCategoryItem *newParentItem)
{
    setParent(newParentItem);
    m_parentItem = newParentItem;
}
