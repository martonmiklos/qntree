#ifndef SUPPLIER_PARAMETER_MAP_H
#define SUPPLIER_PARAMETER_MAP_H

#include <Table>
#include <TableSet>

class SupplierParameterMap : public Nut::Table
{
  Q_OBJECT

  NUT_PRIMARY_AUTO_INCREMENT(id)
  NUT_DECLARE_FIELD(int, id, id, setId, 0)

  NUT_DECLARE_FIELD(QString, supplier_paramtype_identifier, supplier_paramtype_identifier, setSupplier_paramtype_identifier, QString())

  NUT_DECLARE_FIELD(int, invenetree_part_category_parameter_type_id, invenetree_part_category_parameter_type_id, setInvenetree_part_category_parameter_type_id, 0)

  public:
    Q_INVOKABLE SupplierParameterMap( QObject *parent = nullptr );

};

Q_DECLARE_METATYPE(SupplierParameterMap*)


#endif
