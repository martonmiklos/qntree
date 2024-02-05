#ifndef SUPPLIER_CATEGORY_MAP_H
#define SUPPLIER_CATEGORY_MAP_H

#include <Table>
#include <TableSet>

class SupplierCategoryMap : public Nut::Table
{
  Q_OBJECT

  NUT_PRIMARY_AUTO_INCREMENT(id)
  NUT_DECLARE_FIELD(int, id, id, setId, 0)

  NUT_DECLARE_FIELD(int, supplier_id, supplier_id, setSupplier_id, 0)

  NUT_DECLARE_FIELD(QString, supplier_category_name, supplier_category_name, setSupplier_category_name, QString())

  NUT_DECLARE_FIELD(QString, supplier_category_id, supplier_category_id, setSupplier_category_id, QString())

  NUT_DECLARE_FIELD(int, inventree_category_id, inventree_category_id, setInventree_category_id, 0)

  public:
    Q_INVOKABLE SupplierCategoryMap( QObject *parent = nullptr );

};

Q_DECLARE_METATYPE(SupplierCategoryMap*)


#endif
