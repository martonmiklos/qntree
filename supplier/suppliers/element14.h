#pragma once

#include "../abstractsupplier.h"
#include "element14part.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>

class Element14 : public AbstractSupplier
{
    Q_OBJECT

public:
    explicit Element14(QObject *parent = nullptr);
    void retrivePart(const QString &partNumber) override;
    int invenTreeId() const override;

private slots:
    void networkReplyFinished();

private:
    QNetworkAccessManager *m_manager = nullptr;
    QNetworkReply *m_partReply = nullptr;
    QNetworkReply *m_imageReply = nullptr;
    Element14Part m_part;
};
