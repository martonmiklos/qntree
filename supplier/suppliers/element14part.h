#pragma once

#include "supplier/supplierpart.h"

class QJsonObject;

class Element14Part : public SupplierPart
{
    friend class Element14;

public:
    Element14Part() = default;
    void parseFromProductJson(const QJsonObject &productJson);
    void parseImageResponse(const QByteArray &imageData);
};
