#pragma once

#include "supplier/supplierpart.h"

class TMEPart : public SupplierPart
{
public:
    TMEPart() = default;
    TMEPart(const QJsonObject &productJson);
    void parseParametersResponse(const QJsonArray &paramsJson);
};
