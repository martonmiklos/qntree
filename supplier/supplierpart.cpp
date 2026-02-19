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

void SupplierPart::reset()
{
    m_attachments.clear();
}

void SupplierPart::setAttachmentComment(int index, const QString &comment)
{
    Q_ASSERT(index < m_attachments.count());
    m_attachments[index].comment = comment;
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

QString SupplierPart::mpn() const
{
    return m_mpn;
}

QString SupplierPart::sku() const
{
    return m_sku;
}

QString SupplierPart::supplierLink() const
{
    return m_supplierLink;
}

SupplierAttachment *SupplierPart::attachmentAt(int index)
{
    return &m_attachments[index];
}

qreal SupplierPart::quantity() const
{
    return m_quantity;
}

