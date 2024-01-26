#pragma once

#include "../qt_goodies/qobjectsingleton.h"
#include "supplier/abstractsupplier.h"
class SupplierRegistry : public QObjectSingleton<SupplierRegistry>
{
public:
    SupplierRegistry(QObject *parent = nullptr);

    AbstractSupplier *getSupplierByUId(const QString &uid);
    QList<QPair<QString, QString>> getSuppliersDisplayInfo();

private:
    QList<AbstractSupplier *> m_suppliers;
};


