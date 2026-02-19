#pragma once

#include "supplier/supplierpart.h"

class QJsonArray;
class QJsonObject;
class TME;

class TMEPart : public SupplierPart
{
    friend class TME;
public:
    TMEPart() = default;
    TMEPart(const QJsonObject &productJson);
    void parseParametersResponse(const QJsonArray &paramsJson);
    void parseAttachmentsResponse(const QJsonArray &paramsJson);
    void parsePricingResponse(const QJsonArray &paramsJson, const QString &currency);
    void parseImageResponse(const QByteArray &replyData);
};
