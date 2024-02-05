#pragma once

#include <Database>
#include <Query>
#include "gen_src/qntree.h"

class Database;
class TableSet;

class TagDbMetaReg
{

};

class ConfigDb : public Qntree
{
public:
    Q_INVOKABLE ConfigDb();
    static ConfigDb *instance();

    bool init();
private:
    bool m_isConnected = false;
    static ConfigDb *m_instance;
};

