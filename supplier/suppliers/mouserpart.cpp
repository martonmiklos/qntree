#include "mouserpart.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QRegularExpression>

void MouserPart::parseFromSearchResult(const QJsonObject &partJson)
{
    reset();
    m_properties.clear();
    m_priceRanges.clear();

    m_name = partJson.value(QStringLiteral("ManufacturerPartNumber")).toString();
    m_mpn = m_name;
    m_sku = partJson.value(QStringLiteral("MouserPartNumber")).toString();
    m_description = partJson.value(QStringLiteral("Description")).toString();
    m_categoryName = partJson.value(QStringLiteral("Category")).toString().trimmed();
    m_categoryId = partJson.value(QStringLiteral("CategoryId")).toVariant().toString().trimmed();
    if (m_categoryId.isEmpty())
        m_categoryId = m_categoryName;
    m_manufacturerName = partJson.value(QStringLiteral("Manufacturer")).toString();
    m_supplierLink = partJson.value(QStringLiteral("ProductDetailUrl")).toString();

    const QString datasheetUrl = partJson.value(QStringLiteral("DataSheetUrl")).toString();
    if (!datasheetUrl.isEmpty()) {
        SupplierAttachment datasheetAttachment;
        datasheetAttachment.url = QUrl(datasheetUrl);
        datasheetAttachment.comment = QObject::tr("Datasheet");
        datasheetAttachment.upload = true;
        m_attachments.append(datasheetAttachment);
    }

    const QJsonArray attributes = partJson.value(QStringLiteral("ProductAttributes")).toArray();
    for (const auto &attributeValue : attributes) {
        const QJsonObject attribute = attributeValue.toObject();
        const QString attributeName = attribute.value(QStringLiteral("AttributeName")).toString();
        const QString attributeText = attribute.value(QStringLiteral("AttributeValue")).toString();
        if (!attributeName.isEmpty() && !attributeText.isEmpty())
            m_properties.append(SupplierPartProperty(attributeName, attributeText, QString()));
    }

    const QJsonArray priceBreaks = partJson.value(QStringLiteral("PriceBreaks")).toArray();
    for (const auto &priceBreakValue : priceBreaks) {
        const QJsonObject priceBreakObj = priceBreakValue.toObject();
        SupplierPartPricingRange pricingRange;
        pricingRange.currency = priceBreakObj.value(QStringLiteral("Currency")).toString();
        pricingRange.qtyMin = priceBreakObj.value(QStringLiteral("Quantity")).toDouble();

        QString priceString = priceBreakObj.value(QStringLiteral("Price")).toString();
        priceString.remove(QRegularExpression(QStringLiteral("[^0-9\\.,-]")));
        priceString.replace(',', '.');
        pricingRange.price = priceString.toDouble();
        m_priceRanges.append(pricingRange);
    }

    const QString rawAvailability = partJson.value(QStringLiteral("Availability")).toString();
    QRegularExpression qtyRe(QStringLiteral("(\\d+[\\d,\\.]*)"));
    const auto match = qtyRe.match(rawAvailability);
    if (match.hasMatch()) {
        QString quantityString = match.captured(1);
        quantityString.remove(',');
        m_quantity = quantityString.toDouble();
    } else {
        m_quantity = 0;
    }
}

void MouserPart::parseImageResponse(const QByteArray &imageData)
{
    m_image.loadFromData(imageData);
}
