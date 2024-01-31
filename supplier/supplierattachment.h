#pragma once

#include "qurl.h"
class SupplierAttachment
{
public:
    SupplierAttachment();

    QUrl url;
    quint64 sizeInBytes;
};
