#pragma once

class AbstractInvenTreeFetchableTreeModel : public QObject
{
    Q_OBJECT
public:
    InvenTreeCategoryItem(InvenTree::PartApi *api, InvenTree::Category category, InvenTreeCategoryItem *parentItem = nullptr);
    InvenTree::Category categoryData;

    void appendChild(InvenTreeCategoryItem *child) {m_childItems.append(child);}

    InvenTreeCategoryItem *child(int row) {return m_childItems.at(row);}
    int childCount() const {return m_childItems.count();}
    int row() const;
    InvenTreeCategoryItem *parentItem() {return m_parentItem;}

    void fetchChilds();
    bool areChildsFetched() const;

    QModelIndex index() const;
    void setIndex(const QModelIndex &newIndex);

    QModelIndex findIndexOfCategory(int pk, bool *found = nullptr) const;

private:
    QList<InvenTreeCategoryItem *> m_childItems;
    InvenTreeCategoryItem *m_parentItem = nullptr;
    InvenTree::PartApi *m_api = nullptr;
    bool m_childsFetched = false;
    bool m_fetchInProgress = false;
    QModelIndex m_index;

private slots:
    void subCategoriesReceieved(InvenTree::PaginatedCategoryList summary);

signals:
    void childsFetched(int childCount);
};
