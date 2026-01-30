QT       += core gui widgets network

CONFIG += c++17


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(db/Nut/nut.pri)
include(db/gen_src/qntree.pri)

SOURCES += \
    InvenTree_dialogs/dialogselectinventreecategory.cpp \
    InvenTree_dialogs/dialogselectinventreecompany.cpp \
    InvenTree_dialogs/dialogselectinventreepart.cpp \
    InvenTree_dialogs/dialogselectinventreestocklocation.cpp \
    InvenTree_dialogs/inventreecategorymodel.cpp \
    InvenTree_dialogs/inventreestocklocationmodel.cpp \
    db/config_db.cpp \
    dialog.cpp \
    inventreepartimportwizard.cpp \
    main.cpp \
    mainwindow.cpp \
    supplier/abstractsupplier.cpp \
    supplier/supplierattachment.cpp \
    supplier/supplierpart.cpp \
    supplier/supplierpartproperty.cpp \
    supplier/supplierregistry.cpp \
    supplier/suppliers/tme.cpp \
    supplier/suppliers/tmepart.cpp \
    wizard/inventreepartimportwizardpage.cpp \
    wizard/inventreepartuploader.cpp \
    wizard/models/companymodel.cpp \
    wizard/models/partlistmodel.cpp \
    wizard/models/pricebreaksmodel.cpp \
    wizard/models/propertymappingmodel.cpp \
    wizard/models/supplierattachmentactiondelegate.cpp \
    wizard/models/supplierattachmentsmodel.cpp \
    wizard/stocklinewidget.cpp \
    wizard/wizardpageattachments.cpp \
    wizard/wizardpageinventreesyncstatus.cpp \
    wizard/wizardpagepartdetails.cpp \
    wizard/wizardpagepartparameters.cpp \
    wizard/wizardpageselectpartcreationmode.cpp \
    wizard/wizardpagestockandpricing.cpp \
    wizard/wizardpagesummary.cpp \
    wizard/wizardpagesupplierdataenter.cpp

HEADERS += \
    InvenTree_dialogs/dialogselectinventreecategory.h \
    InvenTree_dialogs/dialogselectinventreecompany.h \
    InvenTree_dialogs/dialogselectinventreepart.h \
    InvenTree_dialogs/dialogselectinventreestocklocation.h \
    InvenTree_dialogs/inventreecategorymodel.h \
    InvenTree_dialogs/inventreestocklocationmodel.h \
    db/config_db.h \
    dialog.h \
    inventreepartimportwizard.h \
    mainwindow.h \
    qt_goodies/qobjectsingleton.h \
    supplier/abstractsupplier.h \
    supplier/supplierattachment.h \
    supplier/supplierpart.h \
    supplier/supplierpartproperty.h \
    supplier/supplierregistry.h \
    supplier/suppliers/tme.h \
    supplier/suppliers/tmepart.h \
    wizard/inventreepartimportwizardpage.h \
    wizard/inventreepartuploader.h \
    wizard/models/companymodel.h \
    wizard/models/partlistmodel.h \
    wizard/models/pricebreaksmodel.h \
    wizard/models/propertymappingmodel.h \
    wizard/models/supplierattachmentactiondelegate.h \
    wizard/models/supplierattachmentsmodel.h \
    wizard/stocklinewidget.h \
    wizard/wizardpageattachments.h \
    wizard/wizardpageinventreesyncstatus.h \
    wizard/wizardpagepartdetails.h \
    wizard/wizardpagepartparameters.h \
    wizard/wizardpageselectpartcreationmode.h \
    wizard/wizardpagestockandpricing.h \
    wizard/wizardpagesummary.h \
    wizard/wizardpagesupplierdataenter.h

FORMS += \
    InvenTree_dialogs/dialogselectinventreecategory.ui \
    InvenTree_dialogs/dialogselectinventreecompany.ui \
    InvenTree_dialogs/dialogselectinventreepart.ui \
    InvenTree_dialogs/dialogselectinventreestocklocation.ui \
    dialog.ui \
    inventreepartimportwizard.ui \
    mainwindow.ui \
    wizard/stocklinewidget.ui \
    wizard/wizardpageattachments.ui \
    wizard/wizardpageinventreesyncstatus.ui \
    wizard/wizardpagepartdetails.ui \
    wizard/wizardpagepartparameters.ui \
    wizard/wizardpageselectpartcreationmode.ui \
    wizard/wizardpagestockandpricing.ui \
    wizard/wizardpagesummary.ui \
    wizard/wizardpagesupplierdataenter.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


include(gen_src/client/client.pri)

RESOURCES += \
    resources.qrc
