#pragma once

#include <QAbstractListModel>
#include <QCompleter>
#include <QLineEdit>

#include "gen_src/client/StockApi.h"

struct InventreeStockLocationItem
{
    int id = -1;
    QString path;
};

class InventreeStockLocationListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit InventreeStockLocationListModel(QObject *parent = nullptr);

    enum Roles { IdRole = Qt::UserRole + 1 };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    void setLocations(const InvenTree::PaginatedLocationList &list);

private:
    QList<InventreeStockLocationItem> m_items;
};

class InventreeStockLocationPathLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit InventreeStockLocationPathLineEdit(QWidget *parent = nullptr);
    ~InventreeStockLocationPathLineEdit();

private slots:
    void onTextEdited(const QString &text);
    void onLocationRetrieved(InvenTree::PaginatedLocationList locations);

private:
    InvenTree::StockApi *m_api = nullptr;
    InventreeStockLocationListModel *m_model = nullptr;
    QCompleter *m_completer = nullptr;
    QString m_pendingText;

signals:
    void locationSelected(quint32 id);
};
