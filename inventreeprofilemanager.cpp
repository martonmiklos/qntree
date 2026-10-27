#include "inventreeprofilemanager.h"

#include <QUuid>

InvenTreeProfileManager* InvenTreeProfileManager::m_instance = nullptr;

QString InvenTreeProfileManager::m_currentInstanceUuid = QString();
QSettings* InvenTreeProfileManager::m_settings = nullptr;

InvenTreeProfileManager::InvenTreeProfileManager(QObject *parent)
{
    m_settings = new QSettings(parent);

    auto profiles = m_settings->childGroups();
    m_settings->beginGroup("InvenTree");
    m_currentInstanceUuid = m_settings->value("currentInstance").toString();
    m_settings->endGroup();

    m_settings->beginGroup("InvenTreeProfiles");
    profiles = m_settings->childGroups();
    if (!profiles.contains(m_currentInstanceUuid)) {
        if (profiles.count()) {
            m_currentInstanceUuid = profiles.first();
        }
    }
    m_settings->endGroup();

    m_settings->beginGroup("InvenTree");
    m_settings->setValue("currentInstance", m_currentInstanceUuid);
    m_settings->endGroup();
}

InvenTreeProfileManager *InvenTreeProfileManager::instance(QObject *parent)
{
    if (!m_instance) {
        m_instance = new InvenTreeProfileManager(parent);
    }

    return m_instance;
}

QString InvenTreeProfileManager::currentInstanceUuid()
{
    return m_currentInstanceUuid;
}

void InvenTreeProfileManager::setCurrentInstanceUuid(const QString &uuid)
{
    m_currentInstanceUuid = uuid;
    m_settings->beginGroup("InvenTree");
    m_settings->setValue("currentInstance", m_currentInstanceUuid);
    m_settings->endGroup();
    m_settings->sync();
}

QList<QPair<QString, QString>> InvenTreeProfileManager::availableProfiles()
{
    QList<QPair<QString, QString>> ret;

    m_settings->beginGroup("InvenTreeProfiles");
    auto profiles = m_settings->childGroups();
    for (auto &prof : profiles) {
        m_settings->beginGroup(prof);
        ret.append(QPair<QString, QString>(prof, m_settings->value("name").toString()));
        m_settings->endGroup();
    }
    m_settings->endGroup();
    return ret;
}


