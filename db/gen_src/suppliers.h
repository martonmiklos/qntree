#ifndef SUPPLIERS_H
#define SUPPLIERS_H

#include <Table>
#include <TableSet>

class Suppliers : public Nut::Table
{
  Q_OBJECT

  NUT_PRIMARY_AUTO_INCREMENT(id)
  NUT_DECLARE_FIELD(int, id, id, setId, 0)

  NUT_DECLARE_FIELD(QString, name, name, setName, QString())

  public:
    Q_INVOKABLE Suppliers( QObject *parent = nullptr );

};

Q_DECLARE_METATYPE(Suppliers*)


#endif
