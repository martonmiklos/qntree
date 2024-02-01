#pragma once

#include <QStyledItemDelegate>

class SupplierAttachmentActionDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    SupplierAttachmentActionDelegate(QObject *parent = nullptr);
    ~SupplierAttachmentActionDelegate();

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
};

