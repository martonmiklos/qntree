#include <QCoreApplication>

#include "../../gen_src/client/Attachment.h"
#include "../../gen_src/client/AttachmentApi.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    InvenTree::AttachmentApi *attachmentApi = new InvenTree::AttachmentApi();
    InvenTree::HttpFileElement file;
    QFile lfile("/tmp/1.txt");
    lfile.open(QFile::WriteOnly);
    lfile.write("Helllo leo!\n");
    lfile.close();

    file.loadFromFile("attachment", "/tmp/1.txt", QUuid::createUuid().toString() + ".txt", "text/utf-8");
    // we need to copy over the attachment as the filename must not be filled
    InvenTree::Attachment ac;
    ac.setModelId(1);
    InvenTree::AttachmentModelTypeEnum t;
    t.setValue(InvenTree::AttachmentModelTypeEnum::eAttachmentModelTypeEnum::PART);
    ac.setModelType(t);
    ac.setAttachment(file);
    attachmentApi->addHeaders("Authorization", "Token inv-cfdeb7eb1b8a30c013ecf21db9870816955dc557-20250325");
    attachmentApi->attachmentCreate(ac);

    return QCoreApplication::exec();
}
