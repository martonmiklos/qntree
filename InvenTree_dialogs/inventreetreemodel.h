#pragma once

#include <QAbstractItemModel>



class InvenTreeTreeItem : public QObject
{
    Q_OBJECT
public:
    InvenTreeTreeItem(InvenTreeTreeItem *parentItem = nullptr);

    virtual qint32 getPk() const = 0;
    virtual void fetchChilds() = 0;

    void appendChild(InvenTreeTreeItem *child)
    {
        child->setParentItem(this);
        m_childItems.append(child);
    }

    InvenTreeTreeItem *child(int row)
    {
        return m_childItems.at(row);
    }

    InvenTreeTreeItem *childByPk(int pk)
    {
        for (auto child : std::as_const(m_childItems)) {
            if (child->getPk() == pk)
                return child;
        }

        return nullptr;
    }

    int childCount() const
    {
        return m_childItems.count();
    }

    int row() const
    {
        if (m_parentItem)
            return m_parentItem->m_childItems.indexOf(const_cast<InvenTreeTreeItem*>(this));
        return 0;
    }

    InvenTreeTreeItem *parentItem()
    {
        return m_parentItem;
    }

    bool areChildsFetched() const
    {
        return m_childsFetched;
    }

    QModelIndex index() const
    {
        return m_index;
    }
    void setIndex(const QModelIndex &newIndex)
    {
        m_index = newIndex;
    }

    QModelIndex findIndexOfItem(int pk, bool *found = nullptr) const
    {
        if (getPk() == pk)
            return m_index;

        for (const auto &child : m_childItems) {
            bool found_ = false;
            auto index = child->findIndexOfItem(pk, &found_);
            if (found_) {
                if (found)
                    *found = found_;
                return index;
            }
        }
        return QModelIndex();
    }

    bool hasChildPk(const int pk) const
    {
        for (const auto &child : m_childItems) {
            if (child->getPk() == pk)
                return true;
        }
        return false;
    }

    void setParentItem(InvenTreeTreeItem *newParentItem)
    {
        setParent(newParentItem);
        m_parentItem = newParentItem;
    }

protected:
    QList<InvenTreeTreeItem*> m_childItems;
    InvenTreeTreeItem *m_parentItem = nullptr;
    bool m_childsFetched = false;
    bool m_fetchInProgress = false;
    QModelIndex m_index;

signals:
    void childsFetched(int childCount);
};

/**
 * @brief The InvenTreeTreeModel class
 * Provides abstract class to populate QTreeViews dynamically
 * It is capable to show a specific tree item and open recursively the model upwards
 */
class InvenTreeTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit InvenTreeTreeModel(QObject *parent = nullptr);

    // Basic functionality:
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override
    {
        if (!hasIndex(row, column, parent))
            return QModelIndex();

        InvenTreeTreeItem *parentItem;
        if (!parent.isValid())
            parentItem = m_rootItem;
        else
            parentItem = static_cast<InvenTreeTreeItem*>(parent.internalPointer());

        InvenTreeTreeItem *childItem = parentItem->child(row);
        if (childItem) {
            auto index = createIndex(row, column, childItem);
            childItem->setIndex(index);
            return index;
        }
        return QModelIndex();
    }

    QModelIndex parent(const QModelIndex &index) const override
    {
        if (!index.isValid())
            return QModelIndex();

        InvenTreeTreeItem *childItem = static_cast<InvenTreeTreeItem*>(index.internalPointer());
        InvenTreeTreeItem *parentItem = childItem->parentItem();

        if (parentItem == m_rootItem)
            return QModelIndex();

        return createIndex(parentItem->row(), 0, parentItem);
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override
    {
        InvenTreeTreeItem *parentItem;
        if (parent.column() > 0)
            return 0;

        if (!parent.isValid())
            parentItem = m_rootItem;
        else
            parentItem = static_cast<InvenTreeTreeItem*>(parent.internalPointer());

        return parentItem->childCount();
    }

    bool hasChildren(const QModelIndex &parent) const override
    {
        auto item = static_cast<const InvenTreeTreeItem*>(parent.internalPointer());
        if (item) {
            if (!item->areChildsFetched())
                return true;
            return item->childCount() != 0;
        }
        return true;
    }

    bool canFetchMore(const QModelIndex &parent) const override
    {
        if (!parent.isValid())
            return false;

        InvenTreeTreeItem *item = static_cast<InvenTreeTreeItem*>(parent.internalPointer());
        if (item)
            return !item->areChildsFetched();
        return false;
    }

    void fetchMore(const QModelIndex &parent) override
    {
        auto upper = static_cast<InvenTreeTreeItem*>(parent.internalPointer());
        if (upper) {
            connect(upper, &InvenTreeTreeItem::childsFetched, this, &InvenTreeTreeModel::childItemsFetched);
            upper->fetchChilds();
        }
    }

    void setVisiblePk(int pk)
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

private slots:
    void childItemsFetched(int childCount)
    {
        auto parentItem = static_cast<InvenTreeTreeItem*>(this->sender());
        disconnect(parentItem, &InvenTreeTreeItem::childsFetched, this, &InvenTreeTreeModel::childItemsFetched);
        beginInsertRows(parentItem->index(), 0, childCount - 1);
        endInsertRows();
        if (m_listToPopulate.length()) {
            for (int i = 0; i<parentItem->childCount(); i++) {
                if (parentItem->child(i)->getPk() == m_listToPopulate.last()) {
                    m_listToPopulate.takeLast();
                    emit requestExpand(index(i, 0, parentItem->index()));
                    if (m_listToPopulate.length() == 0)
                        emit requestSelection(index(i, 0, parentItem->index()));
                    break;
                }
            }
        }
        emit dataFetched();
    }

private:
    InvenTreeTreeItem *m_rootItem = nullptr;
    bool m_topLevelCategoriesFetched = false;
    bool m_populatetreeToSelectedCategory = false;

    int m_preSelectedPk = -1;

    void populateParentsRecursivelyToTop(int pk);
    QList<int> m_listToPopulate;

signals:
    void dataFetched();
    void requestExpand(const QModelIndex & index);
    void requestSelection(const QModelIndex & index);

};
