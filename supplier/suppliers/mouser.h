#pragma once

#include "../abstractsupplier.h"
#include "mouserpart.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSettings>

class Mouser : public AbstractSupplier
{
    Q_OBJECT
public:
    explicit Mouser(QObject *parent = nullptr);

    void loadSettings() override;
    void saveSettings() override;

    void retrivePart(const QString &partNumber) override;

    QString apiKey() const;
    void setApiKey(const QString &newApiKey);
    static Mouser *instance(QObject *parent = nullptr);

private slots:
    void networkReplyFinished();

private:
    QNetworkReply *m_partReply = nullptr;
    QNetworkReply *m_imageReply = nullptr;
    MouserPart m_part;
    QString m_apiKey;
    static Mouser *m_instance;
};
