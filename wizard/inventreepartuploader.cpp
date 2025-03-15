#include "inventreepartuploader.h"

InvenTreePartUploader::InvenTreePartUploader(InvenTreePartImportWizard *parent)
    : QObject{parent},
    m_wizard(parent)
{
}

void InvenTreePartUploader::start()
{
    emit stateChanged(Idle, CreatePart);

    m_wizard->initNewInvenTreePart(&m_part);

    connect(m_wizard->partApi(), &InvenTree::PartApi::partCreateSignal, this, &InvenTreePartUploader::partCreated);
    m_wizard->partApi()->partCreate(m_part);
}

void InvenTreePartUploader::partCreated(InvenTree::Part summary)
{
    Q_UNUSED(summary)
    disconnect(m_wizard->partApi(), &InvenTree::PartApi::partCreateSignal, this, &InvenTreePartUploader::partCreated);
    emit stateChanged(CreatePart, CreateParameters);
}
