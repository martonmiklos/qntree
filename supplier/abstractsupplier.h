#pragma once

#include "supplierpart.h"
#include "inventreeprofilemanager.h"
#include "db/config_db.h"


#include <QObject>
#include <QSettings>
#include <QtNetwork/QNetworkAccessManager>


class AbstractSupplier : public QObject
{
    Q_OBJECT
public:
    explicit AbstractSupplier(const QString &name,
                              const QString &uuid,
                              QObject *parent = nullptr)
        : QObject(parent), m_uuid(uuid), m_name(name)
    {
        auto dbEntry = ConfigDb::instance()->suppliers()->query()
            ->where(Suppliers::uuidField() == m_uuid)
            ->first();
        if (!dbEntry) {
            dbEntry = Nut::create<Suppliers>();
            dbEntry->setUuid(m_uuid);
            dbEntry->setName(m_name);
            dbEntry->save(ConfigDb::instance());
        }
        m_id = dbEntry->id();
        this->loadSettings();

        m_manager = new QNetworkAccessManager(this);
    }

    virtual void retrivePart(const QString &partNumber) = 0;

    QString name()
    {
        return m_name;
    }

    QString uid() const // unique UID per supplier type
    {
        return m_uuid;
    }

    virtual int pnLengthLimit() const
    {
        return 32767;
    }

    int invenTreeId() const // ID in the inventree
    {
        return m_inventreeId;
    }

    int getId() const // ID in the SQLite database
    {
        return m_id;
    }

    virtual void loadSettings()
    {
        m_settings.beginGroup("InvenTreeProfiles");
        m_settings.beginGroup(InvenTreeProfileManager::currentInstanceUuid());
        m_settings.beginGroup("Suppliers");
        m_settings.beginGroup(m_name);

        m_inventreeId = m_settings.value("inventree_id").toInt();

        m_settings.endGroup();
        m_settings.endGroup();
        m_settings.endGroup();
        m_settings.endGroup();
    }

    virtual void saveSettings()
    {
        m_settings.beginGroup("InvenTreeProfiles");
        m_settings.beginGroup(InvenTreeProfileManager::currentInstanceUuid());
        m_settings.beginGroup("Suppliers");
        m_settings.beginGroup(m_name);

        m_settings.setValue("inventree_id", m_inventreeId);

        m_settings.endGroup();
        m_settings.endGroup();
        m_settings.endGroup();
        m_settings.endGroup();
    }

    void setInventreeId(int newInventreeId)
    {
        m_inventreeId = newInventreeId;
    }

signals:
    void supplierPartRetrived(SupplierPart &part);
    void error(const QString &error);
    void partNotFound();

protected:
    QNetworkAccessManager *m_manager = nullptr;
    static constexpr const char *KEY_API_KEY = "api_key";

    int m_id = -1, m_inventreeId = -1;
    QString m_uuid;
    QString m_name;
    QSettings m_settings;
    static AbstractSupplier *m_instance;
};
