#include "supplierattachmentactiondelegate.h"
#include <QComboBox>

SupplierAttachmentActionDelegate::SupplierAttachmentActionDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}


SupplierAttachmentActionDelegate::~SupplierAttachmentActionDelegate()
{
}


QWidget *SupplierAttachmentActionDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    // Create the combobox and populate it
    QComboBox *cb = new QComboBox(parent);
    cb->addItem(tr("Save"), "save");
    cb->addItem(tr("Skip"), "skip");
    return cb;
}


void SupplierAttachmentActionDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *cb = qobject_cast<QComboBox *>(editor);
    Q_ASSERT(cb);
    const QString action = index.data(Qt::EditRole).toString();
    const int cbIndex = cb->findData(action);
    if (cbIndex >= 0)
        cb->setCurrentIndex(cbIndex);
}


void SupplierAttachmentActionDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *cb = qobject_cast<QComboBox *>(editor);
    Q_ASSERT(cb);
    model->setData(index, cb->currentData(), Qt::EditRole);
}
