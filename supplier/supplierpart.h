#pragma once

#include "qimage.h"
#include "qlist.h"
#include "supplier/supplierpartproperty.h"
#include "supplierattachment.h"

class SupplierPartPricingRange
{
public:
    QString currency;
    bool isGross = false;

    qreal qtyMin = 0.0;
    qreal price = 0.0;
};

class SupplierPart
{
public:
    SupplierPart();
    QString name() const;
    QString categoryName() const;
    QString categoryId() const;
    QString unit() const;
    QString description() const;
    QString manufacturer() const;
    QList<SupplierAttachment> attachments() const;
    QList<SupplierPartProperty> properties() const;
    QList<SupplierPartPricingRange> priceRanges() const;

    QImage image() const;

    qint32 existingPk() const;
    void setExistingPk(qint32 newExistingPk);

protected:
    QString m_name;
    QString m_categoryName, m_categoryId, m_unit, m_description;
    QString m_manufacturerName;
    QImage m_image;
    QList<SupplierAttachment> m_attachments;
    QList<SupplierPartProperty> m_properties;
    QList<SupplierPartPricingRange> m_priceRanges;
    qint32 m_existingPk = 0;
};
