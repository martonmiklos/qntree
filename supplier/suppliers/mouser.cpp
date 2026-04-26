#include "mouser.h"

#include "mousersettingsdialog.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QUrlQuery>

Mouser::Mouser(QObject *parent)
    : AbstractSupplier(parent)
{
    m_name = tr("Mouser");
    m_uid = QStringLiteral("e2e50b1f-6ce0-4cb8-b2f2-6df9e8840116");
    m_id = -1;

    m_manager = new QNetworkAccessManager(this);
}

void Mouser::retrivePart(const QString &partNumber)
{
    m_settings.beginGroup(MouserSettingsDialog::SETTINGS_GROUP);
    const QString apiKey = m_settings.value(MouserSettingsDialog::KEY_API_KEY).toString().trimmed();
    m_settings.endGroup();

    if (apiKey.isEmpty()) {
        emit error(tr("Mouser API key is missing. Open settings and set it first."));
        return;
    }

    QUrl url(QStringLiteral("https://api.mouser.com/api/v1.0/search/partnumber"));
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("apiKey"), apiKey);
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

int Mouser::invenTreeId() const
{
    m_settings.beginGroup(MouserSettingsDialog::SETTINGS_GROUP);
    const int invenTreeSupplierId = m_settings.value(MouserSettingsDialog::KEY_INVENTREE_SUPPLIER_ID, 0).toInt();
    m_settings.endGroup();
    return invenTreeSupplierId;
}

void Mouser::networkReplyFinished()
{
    auto *reply = qobject_cast<QNetworkReply*>(sender());
    if (reply != m_partReply)
        return;

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
            emit supplierPartRetrived(m_part);
        }
    }

    reply->deleteLater();
    m_partReply = nullptr;
}
