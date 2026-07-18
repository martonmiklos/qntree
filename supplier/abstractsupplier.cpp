#include "abstractsupplier.h"
#include "db/gen_src/suppliers.h"

#include "db/config_db.h"

AbstractSupplier::AbstractSupplier(QObject *parent)
    : QObject{parent}
{
    auto dbEntry = ConfigDb::instance()->suppliers()->query()
    ->where(Suppliers::uuidField() == m_uid)
        ->first();
    if (!dbEntry) {
        dbEntry = Nut::create<Suppliers>();
        dbEntry->setUuid(m_uid);
        dbEntry->setName(m_name);
        dbEntry->save(ConfigDb::instance());
    }
    m_id = dbEntry->id();
}

QString AbstractSupplier::name() const
{
    return m_name;
}

QString AbstractSupplier::uid() const
{
    return m_uid;
}

int AbstractSupplier::getId() const
{
    return m_id;
}
