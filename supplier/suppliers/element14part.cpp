#include "element14part.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QRegularExpression>
#include <QStringList>
#include <QUrl>

namespace {
QString firstNonEmptyValue(const QJsonObject &object, const QStringList &keys)
{
    for (const QString &key : keys) {
        const QString value = object.value(key).toString().trimmed();
        if (!value.isEmpty())
            return value;
    }

    return QString();
}
}

void Element14Part::parseFromProductJson(const QJsonObject &productJson)
{
    reset();
    m_properties.clear();
    m_priceRanges.clear();

    m_name = productJson.value(QStringLiteral("translatedManufacturerPartNumber")).toString();
    if (m_name.isEmpty())
        m_name = productJson.value(QStringLiteral("displayName")).toString();

    m_mpn = productJson.value(QStringLiteral("translatedManufacturerPartNumber")).toString();
    if (m_mpn.isEmpty())
        m_mpn = m_name;

    m_sku = productJson.value(QStringLiteral("sku")).toString();
    m_description = productJson.value(QStringLiteral("displayName")).toString();
    m_categoryName = firstNonEmptyValue(productJson, {
        QStringLiteral("translatedDisplayCategory"),
        QStringLiteral("displayCategory"),
        QStringLiteral("translatedCategoryName"),
        QStringLiteral("categoryName"),
        QStringLiteral("taxonomyName")
    });
    if (m_categoryName.isEmpty())
        m_categoryName = productJson.value(QStringLiteral("category")).toObject().value(QStringLiteral("name")).toString().trimmed();

    m_manufacturerName = productJson.value(QStringLiteral("brandName")).toString();
    m_supplierLink = productJson.value(QStringLiteral("datasheets")).toArray().value(0).toObject().value(QStringLiteral("url")).toString();

    const QJsonArray datasheets = productJson.value(QStringLiteral("datasheets")).toArray();
    for (const auto &datasheetValue : datasheets) {
        const QJsonObject datasheetObj = datasheetValue.toObject();
        const QString datasheetUrl = datasheetObj.value(QStringLiteral("url")).toString();
        if (datasheetUrl.isEmpty())
            continue;

        SupplierAttachment datasheetAttachment;
        datasheetAttachment.url = QUrl(datasheetUrl);
        datasheetAttachment.comment = QObject::tr("Datasheet");
        datasheetAttachment.upload = true;
        m_attachments.append(datasheetAttachment);
    }

    const QJsonArray attributes = productJson.value(QStringLiteral("attributes")).toArray();
    for (const auto &attributeValue : attributes) {
        const QJsonObject attributeObj = attributeValue.toObject();
        const QString attributeName = attributeObj.value(QStringLiteral("attributeLabel")).toString();
        const QString attributeText = attributeObj.value(QStringLiteral("attributeValue")).toString();
        if (!attributeName.isEmpty() && !attributeText.isEmpty())
            m_properties.append(SupplierPartProperty(attributeName, attributeText, QString()));
    }

    const QJsonArray prices = productJson.value(QStringLiteral("prices")).toArray();
    for (const auto &priceValue : prices) {
        const QJsonObject priceObj = priceValue.toObject();
        SupplierPartPricingRange pricingRange;
        pricingRange.currency = priceObj.value(QStringLiteral("currency")).toString();
        pricingRange.qtyMin = priceObj.value(QStringLiteral("from")).toDouble();
        pricingRange.price = priceObj.value(QStringLiteral("cost")).toDouble();
        if (pricingRange.price > 0)
            m_priceRanges.append(pricingRange);
    }

    if (m_priceRanges.isEmpty()) {
        const QJsonArray priceBands = productJson.value(QStringLiteral("prices")).toArray();
        for (const auto &priceBand : priceBands) {
            const QJsonObject bandObj = priceBand.toObject();
            SupplierPartPricingRange pricingRange;
            pricingRange.currency = bandObj.value(QStringLiteral("currency")).toString();
            pricingRange.qtyMin = bandObj.value(QStringLiteral("from")).toDouble();

            QString costString = bandObj.value(QStringLiteral("cost")).toString();
            costString.remove(QRegularExpression(QStringLiteral("[^0-9\\.,-]")));
            costString.replace(',', '.');
            pricingRange.price = costString.toDouble();
            if (pricingRange.price > 0)
                m_priceRanges.append(pricingRange);
        }
    }

    m_quantity = productJson.value(QStringLiteral("inv")).toDouble();
}

void Element14Part::parseImageResponse(const QByteArray &imageData)
{
    m_image.loadFromData(imageData);
}
