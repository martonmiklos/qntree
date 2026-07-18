#include "element14.h"

#include "element14settingsdialog.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QStringList>
#include <QUrlQuery>

namespace {
QString imageUrlFromProduct(const QJsonObject &product)
{
    const QJsonValue imageValue = product.value(QStringLiteral("image"));
    if (imageValue.isString()) {
        const QString directImage = imageValue.toString().trimmed();
        if (!directImage.isEmpty())
            return directImage;
    } else if (imageValue.isObject()) {
        const QJsonObject imageObject = imageValue.toObject();
        const QStringList imageKeys{
            QStringLiteral("vrntPath"),
            QStringLiteral("baseName"),
            QStringLiteral("largeImageUrl"),
            QStringLiteral("mediumImageUrl"),
            QStringLiteral("smallImageUrl"),
            QStringLiteral("url")
        };

        for (const QString &key : imageKeys) {
            const QString imagePath = imageObject.value(key).toString().trimmed();
            if (!imagePath.isEmpty())
                return imagePath;
        }
    }

    const QStringList directImageKeys{
        QStringLiteral("imageBaseName"),
        QStringLiteral("imageUrl"),
        QStringLiteral("largeImageUrl"),
        QStringLiteral("mediumImageUrl"),
        QStringLiteral("smallImageUrl")
    };
    for (const QString &key : directImageKeys) {
        const QString imagePath = product.value(key).toString().trimmed();
        if (!imagePath.isEmpty())
            return imagePath;
    }

    return QString();
}
}

Element14::Element14(QObject *parent)
    : AbstractSupplier(parent)
{
    m_name = tr("element14");
    m_uid = QStringLiteral("5888c32b-2de8-435f-a63b-2c39de5d6b1d");

    m_manager = new QNetworkAccessManager(this);
}

void Element14::retrivePart(const QString &partNumber)
{
    m_settings.beginGroup(Element14SettingsDialog::SETTINGS_GROUP);
    const QString apiKey = m_settings.value(Element14SettingsDialog::KEY_API_KEY).toString().trimmed();
    const QString storeId = m_settings.value(Element14SettingsDialog::KEY_STORE_ID, QStringLiteral("us.newark.com")).toString().trimmed();
    m_settings.endGroup();

    if (apiKey.isEmpty()) {
        emit error(tr("element14 API key is missing. Open settings and set it first."));
        return;
    }

    QUrl url(QStringLiteral("https://api.element14.com/catalog/products"));
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("term"), QStringLiteral("manuPartNum:%1").arg(partNumber.trimmed()));
    query.addQueryItem(QStringLiteral("storeInfo.id"), storeId);
    query.addQueryItem(QStringLiteral("resultsSettings.numberOfResults"), QStringLiteral("1"));
    query.addQueryItem(QStringLiteral("resultsSettings.offset"), QStringLiteral("0"));
    query.addQueryItem(QStringLiteral("callInfo.responseDataFormat"), QStringLiteral("JSON"));
    query.addQueryItem(QStringLiteral("callInfo.apiKey"), apiKey);
    url.setQuery(query);

    if (m_partReply) {
        m_partReply->disconnect(this);
        m_partReply->deleteLater();
        m_partReply = nullptr;
    }

    m_partReply = m_manager->get(QNetworkRequest(url));
    connect(m_partReply, &QNetworkReply::finished, this, &Element14::networkReplyFinished);
}

int Element14::invenTreeId() const
{
    m_settings.beginGroup(Element14SettingsDialog::SETTINGS_GROUP);
    const int invenTreeSupplierId = m_settings.value(Element14SettingsDialog::KEY_INVENTREE_SUPPLIER_ID, 0).toInt();
    m_settings.endGroup();
    return invenTreeSupplierId;
}

void Element14::networkReplyFinished()
{
    auto *reply = qobject_cast<QNetworkReply*>(sender());
    if (reply == m_imageReply) {
        if (reply->error() == QNetworkReply::NoError)
            m_part.parseImageResponse(reply->readAll());

        emit supplierPartRetrived(m_part);
        reply->deleteLater();
        m_imageReply = nullptr;
        return;
    } else if (reply != m_partReply) {
        return;
    }

    const QByteArray payload = reply->readAll();
    if (reply->error() != QNetworkReply::NoError) {
        emit error(reply->errorString());
        reply->deleteLater();
        m_partReply = nullptr;
        return;
    }

    const QJsonDocument json = QJsonDocument::fromJson(payload);
    const QJsonObject root = json.object();

    QJsonArray products = root.value(QStringLiteral("products")).toArray();
    if (products.isEmpty()) {
        products = root.value(QStringLiteral("manufacturerPartNumberSearchReturn"))
                       .toObject()
                       .value(QStringLiteral("products"))
                       .toArray();
    }
    if (products.isEmpty()) {
        emit partNotFound();
    } else {
        const QJsonObject product = products.first().toObject();
        m_part.parseFromProductJson(product);

        const QString imageUrlString = imageUrlFromProduct(product);

        if (!imageUrlString.isEmpty()) {
            QUrl imageUrl(imageUrlString);
            if (!imageUrl.isValid() || imageUrl.scheme().isEmpty())
                imageUrl = QUrl(QStringLiteral("https://www.newark.com") + imageUrlString);

            if (m_imageReply) {
                m_imageReply->disconnect(this);
                m_imageReply->deleteLater();
                m_imageReply = nullptr;
            }

            m_imageReply = m_manager->get(QNetworkRequest(imageUrl));
            connect(m_imageReply, &QNetworkReply::finished, this, &Element14::networkReplyFinished);
        } else {
            emit supplierPartRetrived(m_part);
        }
    }

    reply->deleteLater();
    m_partReply = nullptr;
}
