#include "supplierregistry.h"
#include "suppliers/element14.h"
#include "suppliers/mouser.h"
#include "suppliers/tme.h"

SupplierRegistry::SupplierRegistry(QObject *parent) :
    QObjectSingleton<SupplierRegistry>(parent)
{
    m_suppliers.append(new TME(this));
    m_suppliers.append(new Mouser(this));
    m_suppliers.append(new Element14(this));
}

QList<QPair<QString, QString> > SupplierRegistry::getSuppliersDisplayInfo()
{
    QList<QPair<QString, QString> > ret;
    for (const auto supplier : m_suppliers)
        ret.append(QPair<QString, QString>(supplier->name(), supplier->uid()));
    return ret;
}

AbstractSupplier *SupplierRegistry::getSupplierByUId(const QString &uid)
{
    for (auto supplier : m_suppliers)
        if (supplier->uid() == uid)
            return supplier;
    return nullptr;
}
