#ifndef MANUFACTURER_MAP_H
#define MANUFACTURER_MAP_H

#include <Table>
#include <TableSet>

class ManufacturerMap : public Nut::Table
{
  Q_OBJECT

  NUT_PRIMARY_AUTO_INCREMENT(id)
  NUT_DECLARE_FIELD(int, id, id, setId, 0)

  NUT_DECLARE_FIELD(int, inventree_company_pk, inventree_company_pk, setInventree_company_pk, 0)

  NUT_DECLARE_FIELD(int, supplier_id, supplier_id, setSupplier_id, 0)

  NUT_DECLARE_FIELD(QString, manufacturer_name, manufacturer_name, setManufacturer_name, QString())

  public:
    Q_INVOKABLE ManufacturerMap( QObject *parent = nullptr );

};

Q_DECLARE_METATYPE(ManufacturerMap*)


#endif
