#pragma once

#include "qurl.h"
class SupplierAttachment
{
public:
    SupplierAttachment() = default;

    QUrl url;
    quint64 sizeInBytes;
    QString comment;
    QString language;
    bool hasOtherLang = false;
    bool upload = false;

    QString filename() const
    {
        QString ret = url.fileName();
        if (hasOtherLang)
            ret.append(" (" + language + ")");
        return ret;
    }
};
