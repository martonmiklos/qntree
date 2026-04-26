#include "InvenTree_dialogs/dialogselectinventreecategory.h"
#include "gen_src/client/PartApi.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("MM");
    QCoreApplication::setApplicationName("categorycompletertest");
    QApplication a(argc, argv);
    auto m_api = new InvenTree::PartApi();

    m_api->addHeaders("Authorization", "Token inv-cfdeb7eb1b8a30c013ecf21db9870816955dc557-20250325");

    DialogSelectInvenTreeCategory *dlg = new DialogSelectInvenTreeCategory(m_api);
    dlg->show();
    return a.exec();
}
