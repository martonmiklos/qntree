#pragma once

#include "supplier/supplierpart.h"

class QJsonObject;

class MouserPart : public SupplierPart
{
    friend class Mouser;
public:
    MouserPart() = default;
    void parseFromSearchResult(const QJsonObject &partJson);
    void parseImageResponse(const QByteArray &imageData);
};
