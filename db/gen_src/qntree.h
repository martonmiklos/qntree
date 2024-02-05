#ifndef QNTREE_H
#define QNTREE_H

#include <Database>
#include <Query>
#include "supplier_parameter_map.h"
#include "suppliers.h"
#include "supplier_category_map.h"

class Database;
class TableSet;

class Qntree : public Nut::Database
{
  Q_OBJECT

  NUT_DB_VERSION(1)

  NUT_DECLARE_TABLE(SupplierParameterMap, supplier_parameter_map)
  NUT_DECLARE_TABLE(Suppliers, suppliers)
  NUT_DECLARE_TABLE(SupplierCategoryMap, supplier_category_map)
  public:
    Q_INVOKABLE Qntree();

};

#endif
