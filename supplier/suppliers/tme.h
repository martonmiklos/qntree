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

    void loadSettings() override;
    void saveSettings() override;

    QString currency() const;
    void setCurrency(const QString &newCurrency);

    bool grossPrices() const;
    void setGrossPrices(bool newGrossPrices);

    QString secret() const;
    void setSecret(const QString &newSecret);

    QString token() const;
    void setToken(const QString &newToken);

    QString country() const;
    void setCountry(const QString &newCountry);

    QString language() const;
    void setLanguage(const QString &newLanguage);

    static TME *instance(QObject *parent = nullptr);

private:
    static constexpr const char *KEY_SECRET = "secret";
    static constexpr const char *KEY_TOKEN = "token";
    static constexpr const char *KEY_COUNTRY = "country";
    static constexpr const char *KEY_LANGUAGE = "language";
    static constexpr const char *KEY_CURRENCY = "currency";
    static constexpr const char *KEY_GROSS_PRICES = "gross_prices";

    QString m_secret, m_token, m_country, m_language;
    QNetworkReply* apiCall(const QString &action, QList<QPair<QString, QString> > params);
    QNetworkReply *m_partDetailQueryReply = nullptr;
    QNetworkReply *m_partPropertiesQueryReply = nullptr;
    QNetworkReply *m_partAttachementsQueryReply = nullptr;
    QNetworkReply *m_partPricingQueryReply = nullptr;
    QNetworkReply *m_imageReply = nullptr;
    TMEPart m_part;

    QString m_currency = QStringLiteral("EUR");
    bool m_useGrossPrices = true;

    bool m_isQrDataBeingScanned = false;
    qreal m_qtyFromQr = 0;
    QString m_poFromQr;
    mutable QSettings m_settings;

    static TME* m_instance;

private slots:
    void networkReplyFinished(QNetworkReply *reply);
};




