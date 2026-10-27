#pragma once

#include <QString>
#include <QSettings>

class InvenTreeProfileManager
{
public:
    InvenTreeProfileManager(QObject *parent = nullptr);

    static InvenTreeProfileManager* instance(QObject *parent = nullptr);

    static QString currentInstanceUuid();
    static void setCurrentInstanceUuid(const QString &uuid);

    static QList <QPair<QString, QString>> availableProfiles();

private:
    static InvenTreeProfileManager *m_instance;
    static QString m_currentInstanceUuid;
    static QSettings *m_settings;
};
