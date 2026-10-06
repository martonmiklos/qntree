QT = core network gui

CONFIG += c++17 cmdline

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        ../../gen_src/client/Attachment.cpp \
        ../../gen_src/client/AttachmentApi.cpp \
        ../../gen_src/client/AttachmentModelTypeEnum.cpp \
        ../../gen_src/client/Helpers.cpp \
        ../../gen_src/client/HttpFileElement.cpp \
        ../../gen_src/client/HttpRequest.cpp \
        ../../gen_src/client/Oauth.cpp \
        ../../gen_src/client/PaginatedAttachmentList.cpp \
        ../../gen_src/client/PatchedAttachment.cpp \
        ../../gen_src/client/User.cpp \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ../../gen_src/client/Attachment.h \
    ../../gen_src/client/AttachmentApi.h \
    ../../gen_src/client/AttachmentModelTypeEnum.h \
    ../../gen_src/client/Helpers.h \
    ../../gen_src/client/HttpFileElement.h \
    ../../gen_src/client/HttpRequest.h \
    ../../gen_src/client/Oauth.h \
    ../../gen_src/client/PaginatedAttachmentList.h \
    ../../gen_src/client/PatchedAttachment.h \
    ../../gen_src/client/User.h
