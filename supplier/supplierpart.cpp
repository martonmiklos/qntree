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
