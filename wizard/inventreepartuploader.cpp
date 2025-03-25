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
    connect(m_wizard->partApi(), &InvenTree::PartApi::partCreateSignalError, this, &InvenTreePartUploader::partCreateSignalError);
    m_wizard->partApi()->partCreate(m_part);
}

void InvenTreePartUploader::partCreated(InvenTree::Part summary)
{
    Q_UNUSED(summary)
    disconnect(m_wizard->partApi(), &InvenTree::PartApi::partCreateSignal, this, &InvenTreePartUploader::partCreated);
    emit stateChanged(CreatePart, CreateParameters);
}

void InvenTreePartUploader::partCreateSignalError(InvenTree::Part , QNetworkReply::NetworkError, const QString &error_str)
{
    disconnect(m_wizard->partApi(), &InvenTree::PartApi::partCreateSignalError, this, &InvenTreePartUploader::partCreateSignalError);
    emit stateFailed(CreatePart, error_str);
}
