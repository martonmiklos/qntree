#include "tmepart.h"
#include "qjsonarray.h"
#include "qjsonobject.h"

TMEPart::TMEPart(const QJsonObject &productJson)
{
    m_name = productJson["Symbol"].toString();
    m_categoryName = productJson["Category"].toString();
    m_categoryId = QString::number(productJson["CategoryId"].toInt());
    m_unit = productJson["Unit"].toString();
    m_description = productJson["Description"].toString();
    m_properties.append(SupplierPartProperty("Weight", QString::number(productJson["Weight"].toDouble()), productJson["WeightUnit"].toString()));
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
