#pragma once

#include <QDialog>
#include <QSettings>

#include "InvenTree_dialogs/inventreecategorymodel.h"

namespace Ui {
class DialogSelectInvenTreeCategory;
}

class DialogSelectInvenTreeCategory : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSelectInvenTreeCategory(InvenTree::PartApi *api, int selectedPk = 0, QWidget *parent = nullptr);
    ~DialogSelectInvenTreeCategory();
    
    void setFilterForNonStructural(bool newFilterForNonStructural);

private slots:
    void on_treeViewCategory_doubleClicked(const QModelIndex &index);

private:
    Ui::DialogSelectInvenTreeCategory *ui;
    bool m_filterForNonStructural = true;
    InvenTreeCategoryModel *m_model = nullptr;
    QSettings m_settings;

signals:
    void categorySelected(int pk, const QString &categoryName, const QString &categoryPath);
};
