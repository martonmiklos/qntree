#pragma once

#include <QDialog>

#include "InvenTree_dialogs/inventreecategorymodel.h"

namespace Ui {
class DialogSelectInvenTreeLocation;
}

class DialogSelectInvenTreeLocation : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSelectInvenTreeLocation(InvenTree::PartApi *api, QWidget *parent = nullptr);
    ~DialogSelectInvenTreeLocation();
    
    void setFilterForNonStructural(bool newFilterForNonStructural);

private slots:
    void on_treeViewCategory_doubleClicked(const QModelIndex &index);

private:
    Ui::DialogSelectInvenTreeLocation *ui;
    bool m_filterForNonStructural = true;
    InvenTreeCategoryModel *m_model = nullptr;

signals:
    void categorySelected(int pk, const QString &categoryName, const QString &categoryPath);
};
