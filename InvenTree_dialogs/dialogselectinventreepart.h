#pragma once

#include <QDialog>

#include "../wizard/models/partlistmodel.h"

namespace Ui {
class DialogSelectInvenTreePart;
}

class DialogSelectInvenTreePart : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSelectInvenTreePart(InvenTree::PartApi *api, QWidget *parent = nullptr);
    ~DialogSelectInvenTreePart();

private slots:
    void on_lineEditFilter_textChanged(const QString &arg1);
    void on_tableView_doubleClicked(const QModelIndex &index);

private:
    Ui::DialogSelectInvenTreePart *ui;
    PartListModel *m_model = nullptr;

signals:
    void partSelected(InvenTree::Part part);
};
