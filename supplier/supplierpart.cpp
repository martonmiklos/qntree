#include "supplierpart.h"

SupplierPart::SupplierPart()
{

}

QString SupplierPart::name() const
{
    return m_name;
}

QString SupplierPart::categoryName() const
{
    return m_categoryName;
}

QString SupplierPart::categoryId() const
{
    return m_categoryId;
}

QString SupplierPart::unit() const
{
    return m_unit;
}

QString SupplierPart::description() const
{
    return m_description;
}

QString SupplierPart::manufacturer() const
{
    return m_manufacturerName;
}

QList<SupplierAttachment> SupplierPart::attachments() const
{
    return m_attachments;
}

QList<SupplierPartProperty> SupplierPart::properties() const
{
    return m_properties;
}

QList<SupplierPartPricingRange> SupplierPart::priceRanges() const
{
    return m_priceRanges;
}

QImage SupplierPart::image() const
{
    return m_image;
}

qint32 SupplierPart::existingPk() const
{
    return m_existingPk;
}

void SupplierPart::setExistingPk(qint32 newExistingPk)
{
    m_existingPk = newExistingPk;
}

