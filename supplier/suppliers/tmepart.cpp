#include "tmepart.h"
#include "qjsonarray.h"
#include "qjsonobject.h"

TMEPart::TMEPart(const QJsonObject &productJson)
{
    m_name = productJson["Symbol"].toString();
    m_sku = productJson["Symbol"].toString();
    m_categoryName = productJson["Category"].toString();
    m_categoryId = QString::number(productJson["CategoryId"].toInt());
    m_unit = productJson["Unit"].toString();
    m_description = productJson["Description"].toString();
    m_properties.append(SupplierPartProperty("Weight", QString::number(productJson["Weight"].toDouble()), productJson["WeightUnit"].toString()));
    m_manufacturerName = productJson["Producer"].toString();
    m_mpn = productJson["OriginalSymbol"].toString();
    m_supplierLink = "https:" + productJson["ProductInformationPage"].toString();
}

void TMEPart::parseParametersResponse(const QJsonArray &paramsJson)
{
    for (int i = 0; i<paramsJson.count(); i++) {
        auto obj = paramsJson.at(i).toObject();
        SupplierPartProperty prop;
        prop.m_name = obj["ParameterName"].toString();
        prop.m_value = obj["ParameterValue"].toString();
        prop.m_paramTypeId = obj["ParameterId"].toString();
        m_properties.append(prop);
    }
}

void TMEPart::parseAttachmentsResponse(const QJsonArray &documentListJson)
{
    for (int i = 0; i<documentListJson.count(); i++) {
        auto obj = documentListJson.at(i).toObject();
        SupplierAttachment attachment;
        attachment.url = QUrl("http:" + obj["DocumentUrl"].toString());
        attachment.sizeInBytes = obj["Filesize"].toInt();
        m_attachments.append(attachment);
    }
}

void TMEPart::parsePricingResponse(const QJsonArray &paramsJson, const QString &currency)
{
    for (int i = 0; i<paramsJson.count(); i++) {
        auto obj = paramsJson.at(i).toObject();
        SupplierPartPricingRange priceRange;
        priceRange.price = obj["PriceValue"].toDouble();
        priceRange.qtyMin = obj["Amount"].toDouble();
        priceRange.currency = currency;
        m_priceRanges.append(priceRange);
    }
}

void TMEPart::parseImageResponse(const QByteArray &replyData)
{
    m_image.loadFromData(replyData);
}
