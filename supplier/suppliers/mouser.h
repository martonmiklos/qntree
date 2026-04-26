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
    void retrivePart(const QString &partNumber) override;
    int invenTreeId() const override;

private slots:
    void networkReplyFinished();

private:
    QNetworkAccessManager *m_manager = nullptr;
    QNetworkReply *m_partReply = nullptr;
    MouserPart m_part;
    QSettings m_settings;
};
