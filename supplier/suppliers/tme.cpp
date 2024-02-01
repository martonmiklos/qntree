#include "tme.h"
#include "qjsonarray.h"
#include "qjsondocument.h"
#include "qjsonobject.h"
#include "qnetworkaccessmanager.h"
#include "qnetworkrequest.h"
#include "supplier/suppliers/tmepart.h"
#include "qjsonobject.h"

#include <QMessageAuthenticationCode>

TME::TME(QObject *parent)
    : AbstractSupplier{parent}
{
    m_name = tr("TME");
    m_uid = QStringLiteral("ac01ab15-a375-492c-94c2-f5c4fa16d6a3");

    m_secret ="99b2608090e382f814f7";
    m_token = "e643dd91ccf2b11f07ccdfe892f8fd8c6bf95bbdfcb2c";
    m_country = m_language = "HU";

    m_manager = new QNetworkAccessManager(this);
    connect(m_manager, &QNetworkAccessManager::finished, this, &TME::networkReplyFinished);
}

void TME::retrivePart(const QString &partNumber)
{
    QList<QPair<QString, QString>> params;
    params.append(QPair<QString,QString>(QUrl::toPercentEncoding("SymbolList[0]"), partNumber));
    m_partDetailQueryReply = apiCall("Products/GetProducts", params);
}

int TME::pnLengthLimit() const
{
    return 16;
}

QNetworkReply *TME::apiCall(const QString &action, QList<QPair<QString, QString>> params)
{
    QString apiUrl = "https://api.tme.eu/" + action + ".json";
    QNetworkRequest req =  QNetworkRequest(QUrl(apiUrl));

    params.append(QPair<QString,QString>("Country", m_country));
    params.append(QPair<QString,QString>("Language", m_language));
    params.append(QPair<QString,QString>("Token", m_token));

    std::sort(params.begin(), params.end(),
          [](const QPair<QString,QString> &a, const QPair<QString,QString> &b) -> bool { return a.first < b.first; });
    QString encodedParams;
    for (const auto & param : params) {
        if (!encodedParams.isEmpty())
            encodedParams.append("%26");
        encodedParams.append(QUrl::toPercentEncoding(param.first + "=" + param.second));
    }
    QString signature_base = QString("POST&" + QUrl::toPercentEncoding(apiUrl) + "&" + encodedParams);

    QMessageAuthenticationCode code(QCryptographicHash::Sha1);
    code.setKey(m_secret.toLocal8Bit());
    code.addData(signature_base.toLocal8Bit());

    auto apiSignature = QUrl::toPercentEncoding(code.result().toBase64());
    encodedParams.append("%26" + QUrl::toPercentEncoding("ApiSignature") + "%3D" + apiSignature);

    params.append(QPair<QString,QString>("ApiSignature", apiSignature));

    QString paramString;
    for (const auto & param : params) {
        if (!paramString.isEmpty())
            paramString.append("&");
        paramString.append(param.first + "=" + param.second);
    }
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    return m_manager->post(req, paramString.toLocal8Bit());
}

bool TME::grossPrices() const
{
    return m_grossPrices;
}

void TME::setGrossPrices(bool newGrossPrices)
{
    m_grossPrices = newGrossPrices;
}

QString TME::currency() const
{
    return m_currency;
}

void TME::setCurrency(const QString &newCurrency)
{
    m_currency = newCurrency;
}

void TME::networkReplyFinished(QNetworkReply *reply)
{
    // handle non-JSON responses first
    if (reply == m_imageReply) {
        qWarning() << reply;
        m_part.parseImageResponse(reply->readAll());

        QList<QPair<QString, QString>> params;
        params.append(QPair<QString,QString>(QUrl::toPercentEncoding("SymbolList[0]"), m_part.name()));
        m_partPropertiesQueryReply = apiCall("Products/GetParameters", params);
        return;
    }

    auto json = QJsonDocument::fromJson(reply->readAll());
    qWarning().noquote() << json.toJson(QJsonDocument::Indented);

    if (reply == m_partDetailQueryReply) {
        if (!reply->error()) {
            QJsonObject ob = json.object()["Data"].toObject();
            ob = ob["ProductList"].toArray().first().toObject();
            m_part = TMEPart(ob);

            QNetworkRequest req =  QNetworkRequest(QUrl("http:" + ob["Photo"].toString()));
            m_imageReply = m_manager->get(req);
        }
    } else if (reply == m_partPropertiesQueryReply) {
        if (!reply->error()) {
            QJsonObject ob = json.object()["Data"].toObject();
            ob = ob["ProductList"].toArray().first().toObject();
            m_part.parseParametersResponse(ob["ParameterList"].toArray());

            QList<QPair<QString, QString>> params;
            params.append(QPair<QString,QString>(QUrl::toPercentEncoding("SymbolList[0]"), m_part.name()));
            m_partAttachementsQueryReply = apiCall("Products/GetProductsFiles", params);
        }
    } else if (reply == m_partAttachementsQueryReply) {
        if (!reply->error()) {
            QJsonObject ob = json.object()["Data"].toObject();
            ob = ob["ProductList"].toArray().first().toObject();
            ob = ob["Files"].toObject();
            m_part.parseAttachmentsResponse(ob["DocumentList"].toArray());

            QList<QPair<QString, QString>> params;
            params.append(QPair<QString,QString>(QUrl::toPercentEncoding("SymbolList[0]"), m_part.name()));
            params.append(QPair<QString,QString>("Currency", m_currency));
            params.append(QPair<QString,QString>("GrossPrices", m_grossPrices ? "true" : "false"));
            m_partPricingQueryReply = apiCall("Products/GetPrices", params);
        }
    } else if (reply == m_partPricingQueryReply) {
        if (!reply->error()) {
            QJsonObject ob = json.object()["Data"].toObject();
            auto prodList = ob["ProductList"].toArray().first().toObject();
            m_part.parsePricingResponse(prodList["PriceList"].toArray(), ob["Currency"].toString());
            emit supplierPartRetrived(m_part);
        }
    }
}
