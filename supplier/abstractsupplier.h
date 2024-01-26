#pragma once

#include "supplierpart.h"
#include <QObject>

class AbstractSupplier : public QObject
{
    Q_OBJECT
public:
    explicit AbstractSupplier(QObject *parent = nullptr);
    virtual void retrivePart(const QString &partNumber) = 0;

    QString name() const;
    QString uid() const;

    virtual int pnLengthLimit() const  {return 32767;}

signals:
    void supplierPartRetrived(SupplierPart &part);
    void error(const QString &error);
    void partNotFound();
protected:
    QString m_uid;
    QString m_name;
};
