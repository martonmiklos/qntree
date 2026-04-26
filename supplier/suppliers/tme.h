#pragma once

#include "../abstractsupplier.h"
#include "qnetworkreply.h"
#include "supplier/suppliers/tmepart.h"
#include <QSettings>

class TME : public AbstractSupplier
{
    Q_OBJECT
public:
    explicit TME(QObject *parent = nullptr);
    void retrivePart(const QString &partNumber) override;
    int pnLengthLimit() const override;

    QString currency() const;
    void setCurrency(const QString &newCurrency);

    bool grossPrices() const;
    void setGrossPrices(bool newGrossPrices);

    int invenTreeId() const override;

private:
    void loadSettings();

    QString m_secret, m_token, m_country, m_language;
    QNetworkReply* apiCall(const QString &action, QList<QPair<QString, QString> > params);
    QNetworkAccessManager * m_manager = nullptr;
    QNetworkReply *m_partDetailQueryReply = nullptr;
    QNetworkReply *m_partPropertiesQueryReply = nullptr;
    QNetworkReply *m_partAttachementsQueryReply = nullptr;
    QNetworkReply *m_partPricingQueryReply = nullptr;
    QNetworkReply *m_imageReply = nullptr;
    TMEPart m_part;

    QString m_currency = QStringLiteral("EUR");
    bool m_grossPrices = true;

    bool m_isQrDataBeingScanned = false;
    qreal m_qtyFromQr = 0;
    QString m_poFromQr;
    mutable QSettings m_settings;

private slots:
    void networkReplyFinished(QNetworkReply *reply);

};




