#include "supplierpartproperty.h"
#include "qregularexpression.h"

SupplierPartProperty::SupplierPartProperty(const QString &name, const QString &value, const QString &unit)
    : m_name(name), m_value(value), m_unit(unit)
{

}

QString SupplierPartProperty::valueString() const
{
    if (m_unit.isEmpty())
        return m_value;
    return m_value + " " + m_unit;
}

bool SupplierPartProperty::splittable() const
{
    if (!m_unit.isEmpty())
        return false;
    static QRegularExpression re("^[\\-]?[\\+]?[0-9]+.*(~[0-9])*$");
    return re.match(m_value).hasMatch();
}

void SupplierPartProperty::splitUnit()
{
    int firstNum = -1;
    for (int i = m_value.length(); i>0; i--) {
        if (m_value.at(i - 1).isNumber()) {
            firstNum = (i-1);
            break;
        }
    }

    if (firstNum == -1)
        return;
    m_unit = m_value.mid(firstNum + 1);
    m_value = m_value.mid(0, firstNum + 1);
}
