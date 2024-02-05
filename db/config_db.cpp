#include "config_db.h"


#include <TableSet>
#include <QFile>
#include <QStandardPaths>
#include <QDir>

#ifdef NUT_NAMESPACE
using namespace NUT_NAMESPACE;
#endif

ConfigDb *ConfigDb::m_instance = nullptr;



ConfigDb::ConfigDb()
    : Qntree()
{
}

ConfigDb *ConfigDb::instance()
{
    if (m_instance == nullptr)
        m_instance = new ConfigDb();
    return m_instance;
}

bool ConfigDb::init()
{
    setDriver("QSQLITE");
    const QString dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
            + QDir::separator()
            + "qntree.db";
    if (!QFile::exists(dbPath)) {
        if (!QFile::exists(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation))) {
            QDir dir;
            if (!dir.mkpath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation))) {
                qCritical() << "Unable to create the folder for the database"
                            << QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
                m_isConnected = false;
                return false;
            }
        }

        if (!QFile::copy(QStringLiteral(":/db/qntree.db"), dbPath)) {
            qCritical() << "Unable to copy the default db to" << dbPath;
            m_isConnected = false;
            return false;
        }
    }

    QFile dbFile(dbPath);
    if (!dbFile.isWritable()) {
        if (!dbFile.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner)) {
            qCritical() << "Unable to set write permissions to the database " << dbPath;
            m_isConnected = false;
            return false;
        }
    }

    setDatabaseName(dbPath);
    m_isConnected = open();
    return m_isConnected;
}
