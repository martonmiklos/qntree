#pragma once

#include "../abstractsupplier.h"
#include "qnetworkreply.h"
#include "supplier/suppliers/tmepart.h"

class TME : public AbstractSupplier
{
    Q_OBJECT
public:
    explicit TME(QObject *parent = nullptr);
    void retrivePart(const QString &partNumber) override;

    int pnLengthLimit() const override;
private:
    QString m_secret, m_token, m_country, m_language;
    QNetworkReply* apiCall(const QString &action, QList<QPair<QString, QString> > params);
    QNetworkAccessManager * m_manager = nullptr;
    QNetworkReply *m_partDetailQueryReply = nullptr, *m_partPropertiesQueryReply = nullptr;
    TMEPart m_part;

private slots:
    void networkReplyFinished(QNetworkReply *reply);
};






