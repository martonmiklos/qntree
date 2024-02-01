QT       += core gui widgets network

CONFIG += c++17


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(db/Nut/nut.pri)
include(db/gen_src/qntree.pri)

SOURCES += \
    InvenTree_dialogs/dialogselectinventreecategory.cpp \
    InvenTree_dialogs/dialogselectinventreestocklocation.cpp \
    InvenTree_dialogs/inventreecategorymodel.cpp \
    InvenTree_dialogs/inventreestocklocationmodel.cpp \
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
    wizard/models/pricebreaksmodel.cpp \
    wizard/models/propertymappingmodel.cpp \
    wizard/models/supplierattachmentactiondelegate.cpp \
    wizard/models/supplierattachmentsmodel.cpp \
    wizard/wizardpagestockandpricing.cpp \
    wizard/wizardpagesupplierdataenter.cpp

HEADERS += \
    InvenTree_dialogs/dialogselectinventreecategory.h \
    InvenTree_dialogs/dialogselectinventreestocklocation.h \
    InvenTree_dialogs/inventreecategorymodel.h \
    InvenTree_dialogs/inventreestocklocationmodel.h \
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
    wizard/models/pricebreaksmodel.h \
    wizard/models/propertymappingmodel.h \
    wizard/models/supplierattachmentactiondelegate.h \
    wizard/models/supplierattachmentsmodel.h \
    wizard/wizardpagestockandpricing.h \
    wizard/wizardpagesupplierdataenter.h

FORMS += \
    InvenTree_dialogs/dialogselectinventreecategory.ui \
    InvenTree_dialogs/dialogselectinventreestocklocation.ui \
    inventreepartimportwizard.ui \
    mainwindow.ui \
    wizard/wizardpagestockandpricing.ui \
    wizard/wizardpagesupplierdataenter.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


include(gen_src/client/client.pri)
