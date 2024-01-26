#pragma once

#include <QString>

class SupplierPartProperty
{
public:
    SupplierPartProperty() = default;
    SupplierPartProperty(const QString &name, const QString &value, const QString &unit);

    QString valueString() const;

    QString m_name;
    QString m_value;
    QString m_unit;
    QString m_paramTypeId;

    bool splittable() const;
    void splitUnit();
};
