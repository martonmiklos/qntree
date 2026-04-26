#pragma once

#include "supplierpart.h"
#include <QObject>
#include <QSettings>

class AbstractSupplier : public QObject
{
    Q_OBJECT
public:
    explicit AbstractSupplier(QObject *parent = nullptr);
    virtual void retrivePart(const QString &partNumber) = 0;

    QString name() const;
    QString uid() const;

    virtual int pnLengthLimit() const  {return 32767;}

    virtual int invenTreeId() const = 0;

    int getId() const;

signals:
    void supplierPartRetrived(SupplierPart &part);
    void error(const QString &error);
    void partNotFound();

protected:
    int m_id;
    QString m_uid;
    QString m_name;
    mutable QSettings m_settings;
};
