#include "tme.h"
#include "qjsonarray.h"
#include "qjsondocument.h"
#include "qjsonobject.h"
#include "qnetworkaccessmanager.h"
#include "qnetworkrequest.h"
#include "supplier/suppliers/tmepart.h"
#include "qjsonobject.h"
#include "db/config_db.h"

#include <QRegularExpression>
#include <QMessageAuthenticationCode>

TME::TME(QObject *parent)
    : AbstractSupplier{parent}
{
    m_name = tr("TME");
    m_uid = QStringLiteral("ac01ab15-a375-492c-94c2-f5c4fa16d6a3");

    m_secret ="20948d40191f45adfebc";
    m_token = "22ecd8bff75788af5b46130c83ec68f0326b0d53979ee4c9f1";
    m_country = m_language = "HU";

    auto dbEntry = ConfigDb::instance()->suppliers()->query()
        ->where(Suppliers::uuidField() == m_uid)
        ->first();
    if (!dbEntry) {
        dbEntry = Nut::create<Suppliers>();
        dbEntry->setUuid(m_uid);
        dbEntry->setName(m_name);
        dbEntry->save(ConfigDb::instance());
    }
    m_id = dbEntry->id();

    m_manager = new QNetworkAccessManager(this);
    connect(m_manager, &QNetworkAccessManager::finished, this, &TME::networkReplyFinished);
}

void TME::retrivePart(const QString &userData)
{
    QString partNumber = userData;
    static QRegularExpression bigQrRe("QTY:([0-9]*)\\sPN:([^\\s]*)\\sPO:([^\\s]*).*");
    auto matches = bigQrRe.match(userData);
    if (matches.hasMatch()) {
        m_isQrDataBeingScanned = true;
        m_qtyFromQr = matches.captured(1).toDouble();
        partNumber = matches.captured(2);
        m_poFromQr = matches.captured(3);        
    }

    QList<QPair<QString, QString>> params;
    params.append(QPair<QString,QString>("SymbolList%5B0%5D", QUrl::toPercentEncoding(partNumber)));
    m_partDetailQueryReply = apiCall("Products/GetProducts", params);
}

int TME::pnLengthLimit() const
{
    return 65535;
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
    encodedParams.append("%26ApiSignature%3D" + apiSignature);

    params.append(QPair<QString,QString>("ApiSignature", apiSignature));

    QString paramString;
    for (const auto & param : params) {
        if (!paramString.isEmpty())
            paramString.append("&");
        paramString.append(param.first + "=" + param.second);
    }
    qWarning() << paramString;
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

int TME::invenTreeId() const
{
    // FIXME use settings
    return 4;
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
        params.append(QPair<QString,QString>("SymbolList%5B0%5D", QUrl::toPercentEncoding(m_part.name())));
        m_partPropertiesQueryReply = apiCall("Products/GetParameters", params);
        return;
    }

    auto json = QJsonDocument::fromJson(reply->readAll());
    qWarning().noquote() << json.toJson(QJsonDocument::Indented);

    auto jsonObject = json.object();
    auto ob = jsonObject["Data"].toObject();
    if (reply == m_partDetailQueryReply) {
        if (!reply->error()) {
            ob = ob["ProductList"].toArray().first().toObject();
            m_part = TMEPart(ob);

            if (m_isQrDataBeingScanned) {
                m_part.m_quantity = m_qtyFromQr;
                // TODO save the PO for further processing
            }

            QNetworkRequest req =  QNetworkRequest(QUrl("http:" + ob["Photo"].toString()));
            m_imageReply = m_manager->get(req);
        } else {
            auto errCode = jsonObject["ErrorCode"].toInt();
            if (errCode == 6) {
                emit partNotFound();
            } else {
                emit error(jsonObject["ErrorMessage"].toString());
            }
        }
    } else if (reply == m_partPropertiesQueryReply) {
        if (!reply->error()) {
            ob = ob["ProductList"].toArray().first().toObject();
            m_part.parseParametersResponse(ob["ParameterList"].toArray());

            QList<QPair<QString, QString>> params;
            params.append(QPair<QString,QString>("SymbolList%5B0%5D", QUrl::toPercentEncoding(m_part.name())));
            m_partAttachementsQueryReply = apiCall("Products/GetProductsFiles", params);
        } else {
            emit error(jsonObject["ErrorMessage"].toString());
        }
    } else if (reply == m_partAttachementsQueryReply) {
        if (!reply->error()) {
            ob = ob["ProductList"].toArray().first().toObject();
            ob = ob["Files"].toObject();
            m_part.parseAttachmentsResponse(ob["DocumentList"].toArray());

            QList<QPair<QString, QString>> params;
            params.append(QPair<QString,QString>("SymbolList%5B0%5D", QUrl::toPercentEncoding(m_part.name())));
            params.append(QPair<QString,QString>("Currency", m_currency));
            //params.append(QPair<QString,QString>("GrossPrices", m_grossPrices ? "true" : "false"));
            m_partPricingQueryReply = apiCall("Products/GetPrices", params);
        } else {
            emit error(jsonObject["ErrorMessage"].toString());
        }
    } else if (reply == m_partPricingQueryReply) {
        if (!reply->error()) {
            auto prodList = ob["ProductList"].toArray().first().toObject();
            m_part.parsePricingResponse(prodList["PriceList"].toArray(), ob["Currency"].toString());
            emit supplierPartRetrived(m_part);
        } else {
            emit error(jsonObject["ErrorMessage"].toString());
        }
    }
}
