#include "mouser.h"

#include "db/gen_src/suppliers.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QSettings>


Mouser* Mouser::m_instance = nullptr;

Mouser::Mouser(QObject *parent)
    : AbstractSupplier("Mouser", "e2e50b1f-6ce0-4cb8-b2f2-6df9e8840116", parent)
{
    loadSettings();
}

void Mouser::loadSettings()
{
    AbstractSupplier::loadSettings();

    m_settings.beginGroup(QStringLiteral("Suppliers/Mouser"));
    m_apiKey = m_settings.value(KEY_API_KEY).toString();
    m_settings.endGroup();
}

void Mouser::saveSettings()
{
    m_settings.beginGroup(QStringLiteral("Suppliers/Mouser"));
    m_settings.setValue(KEY_API_KEY, m_apiKey);
    m_settings.endGroup();
}

void Mouser::retrivePart(const QString &partNumber)
{
    if (m_apiKey.isEmpty()) {
        emit error(tr("Mouser API key is missing. Open settings and set it first."));
        return;
    }

    QUrl url(QStringLiteral("https://api.mouser.com/api/v1.0/search/partnumber"));
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("apiKey"), m_apiKey);
    url.setQuery(query);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));

    QJsonObject requestRoot;
    QJsonObject requestData;
    requestData.insert(QStringLiteral("mouserPartNumber"), partNumber.trimmed());
    requestData.insert(QStringLiteral("partSearchOptions"), QStringLiteral("None"));
    requestRoot.insert(QStringLiteral("SearchByPartRequest"), requestData);

    if (m_partReply) {
        m_partReply->disconnect(this);
        m_partReply->deleteLater();
        m_partReply = nullptr;
    }

    m_partReply = m_manager->post(request, QJsonDocument(requestRoot).toJson(QJsonDocument::Compact));
    connect(m_partReply, &QNetworkReply::finished, this, &Mouser::networkReplyFinished);
}

void Mouser::networkReplyFinished()
{
    auto *reply = qobject_cast<QNetworkReply*>(sender());
    if (reply == m_imageReply) {
        if (reply->error() == QNetworkReply::NoError) {
            m_part.parseImageResponse(reply->readAll());
        }

        emit supplierPartRetrived(m_part);
        reply->deleteLater();
        m_imageReply = nullptr;
        return;
    } else if (reply != m_partReply) {
        return;
    }

    const QByteArray payload = reply->readAll();
    const QJsonDocument json = QJsonDocument::fromJson(payload);
    const QJsonObject root = json.object();

    if (reply->error() != QNetworkReply::NoError) {
        emit error(reply->errorString());
    } else {
        const QJsonObject searchResults = root.value(QStringLiteral("SearchResults")).toObject();
        const QJsonArray parts = searchResults.value(QStringLiteral("Parts")).toArray();

        if (parts.isEmpty()) {
            emit partNotFound();
        } else {
            const QJsonObject part = parts.first().toObject();
            m_part.parseFromSearchResult(part);

            QString imagePath = part.value(QStringLiteral("ImagePath")).toString().trimmed();
            if (!imagePath.isEmpty()) {
                if (imagePath.startsWith("//"))
                    imagePath = "https:" + imagePath;

                QUrl imageUrl(imagePath);
                if (!imageUrl.isValid() || imageUrl.scheme().isEmpty())
                    imageUrl = QUrl(QStringLiteral("https://www.mouser.com") + imagePath);

                if (m_imageReply) {
                    m_imageReply->disconnect(this);
                    m_imageReply->deleteLater();
                    m_imageReply = nullptr;
                }
                auto request = QNetworkRequest(imageUrl);
                request.setRawHeader(
                    "User-Agent",
                    "Mozilla/5.0 (X11; Linux x86_64; rv:140.0) "
                    "Gecko/20100101 Firefox/140.0");

                request.setRawHeader(
                    "Accept",
                    "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");

                request.setRawHeader("Sec-Fetch-Dest", "document");
                request.setRawHeader("Pragma", "no-cache");
                request.setRawHeader("Cache-Control", "no-cache");

                m_imageReply = m_manager->get(request);
                connect(m_imageReply, &QNetworkReply::finished, this, &Mouser::networkReplyFinished);
            } else {
                emit supplierPartRetrived(m_part);
            }
        }
    }

    reply->deleteLater();
    m_partReply = nullptr;
}

Mouser *Mouser::instance(QObject *parent)
{
    if (m_instance == nullptr) {
        m_instance = new Mouser(parent);
    }
    return m_instance;
}


QString Mouser::apiKey() const
{
    return m_apiKey;
}

void Mouser::setApiKey(const QString &newApiKey)
{
    m_apiKey = newApiKey;
}
