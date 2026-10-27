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

    void loadSettings() override;
    void saveSettings() override;

    QString apiKey() const;
    void setApiKey(const QString &newApiKey);

    QString storeId() const;
    void setStoreId(const QString &newStoreId);

    static Element14* instance(QObject *parent = nullptr);

private slots:
    void networkReplyFinished();

private:
    QString imageUrlFromProduct(const QJsonObject &product);
    static constexpr const char *KEY_STORE_ID = "store_id";
    QNetworkReply *m_partReply = nullptr;
    QNetworkReply *m_imageReply = nullptr;
    Element14Part m_part;
    QString m_apiKey, m_storeId;
    static Element14 *m_instance;
};
