#include "abstractsupplier.h"

AbstractSupplier::AbstractSupplier(QObject *parent)
    : QObject{parent}
{

}

QString AbstractSupplier::name() const
{
    return m_name;
}

QString AbstractSupplier::uid() const
{
    return m_uid;
}
