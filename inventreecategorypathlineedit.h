#pragma once

#include <QLineEdit>
#include <QCompleter>
#include <QStringListModel>

#include "gen_src/client/PartApi.h"

#include <QAbstractListModel>

struct InventreeCategoryItem
{
    int id = -1;
    QString path;
};

class InventreeCategoryListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit InventreeCategoryListModel(QObject *parent = nullptr);

    enum Roles {
        IdRole = Qt::UserRole + 1
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    void setCategories(const InvenTree::PaginatedCategoryList &list);

private:
    QList<InventreeCategoryItem> m_items;
};

class InventreeCategoryPathLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit InventreeCategoryPathLineEdit(QWidget *parent = nullptr);
    ~InventreeCategoryPathLineEdit();

private slots:
    void onTextEdited(const QString &text);
    void onCategoryRetrieved(InvenTree::PaginatedCategoryList category);

private:
    InvenTree::PartApi *m_api = nullptr;

    QStringList m_paths;
    InventreeCategoryListModel *m_model = nullptr;
    QCompleter *m_completer = nullptr;

    QString m_pendingText;

signals:
    void categorySelected(quint32 id);
};
