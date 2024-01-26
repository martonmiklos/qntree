#pragma once

#include "qimage.h"
#include "qlist.h"
#include "supplier/supplierpartproperty.h"
#include "supplierattachment.h"

class SupplierPart
{
public:
    SupplierPart();

    QString name() const;

    QString categoryName() const;

    QString categoryId() const;

    QString unit() const;

    QString description() const;

    QList<SupplierAttachment> attachments() const;

    QList<SupplierPartProperty> properties() const;

protected:
    QString m_name;
    QString m_categoryName, m_categoryId, m_unit, m_description;
    QImage image;
    QList<SupplierAttachment> m_attachments;
    QList<SupplierPartProperty> m_properties;
};
