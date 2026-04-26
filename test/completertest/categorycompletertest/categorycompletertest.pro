QT       += core gui widgets network

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../../InvenTree_dialogs/dialogselectinventreecategory.cpp \
    ../../../InvenTree_dialogs/inventreecategorymodel.cpp \
    ../../../inventreecategorypathlineedit.cpp \
    main.cpp

HEADERS += \
    ../../../InvenTree_dialogs/dialogselectinventreecategory.h \
    ../../../InvenTree_dialogs/inventreecategorymodel.h \
    ../../../inventreecategorypathlineedit.h

FORMS += \
    ../../../InvenTree_dialogs/dialogselectinventreecategory.ui
INCLUDEPATH += ../../../

include(../../../gen_src/client/client.pri)
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=
