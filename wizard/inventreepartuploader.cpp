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
    connect(m_wizard->partApi(), &InvenTree::PartApi::partCreateSignalError, this, &InvenTreePartUploader::partCreateError);
    m_wizard->partApi()->partCreate(m_part);
}

void InvenTreePartUploader::partCreateError(InvenTree::Part , QNetworkReply::NetworkError, const QString &error_str)
{
    disconnect(m_wizard->partApi(), nullptr, this, nullptr);
    emit stateFailed(CreatePart, error_str);
}

void InvenTreePartUploader::partCreated(InvenTree::Part summary)
{
    disconnect(m_wizard->partApi(), &InvenTree::PartApi::partCreateSignal, this, &InvenTreePartUploader::partCreated);
    emit stateChanged(CreatePart, CreateManufacturerPart);

    m_part = summary;
    m_wizard->initNewManufacturerPart(summary.getPk(), &m_mfrPart);

    connect(m_wizard->companyApi(), &InvenTree::CompanyApi::companyPartManufacturerCreateSignal, this, &InvenTreePartUploader::mfrPartCreated);
    connect(m_wizard->companyApi(), &InvenTree::CompanyApi::companyPartManufacturerCreateSignalError, this, &InvenTreePartUploader::mfrPartCreateError);

    m_wizard->companyApi()->companyPartManufacturerCreate(m_mfrPart);
}

void InvenTreePartUploader::mfrPartCreateError(InvenTree::ManufacturerPart summary, QNetworkReply::NetworkError error_type, const QString &error_str)
{
    Q_UNUSED(summary)
    Q_UNUSED(error_type)
    disconnect(m_wizard->companyApi(), &InvenTree::CompanyApi::companyPartManufacturerCreateSignalError, this, &InvenTreePartUploader::mfrPartCreateError);
    emit stateFailed(CreateManufacturerPart, error_str);
}

void InvenTreePartUploader::mfrPartCreated(InvenTree::ManufacturerPart summary)
{
    disconnect(m_wizard->companyApi(), nullptr, this, nullptr);
    emit stateChanged(CreateManufacturerPart, CreateSupplierPart);

    m_wizard->initNewSupplierPart(m_part.getPk(), summary.getPk(), &m_supplierPart);

    connect(m_wizard->companyApi(), &InvenTree::CompanyApi::companyPartCreateSignal, this, &InvenTreePartUploader::supplierPartCreated);
    connect(m_wizard->companyApi(), &InvenTree::CompanyApi::companyPartCreateSignalError, this, &InvenTreePartUploader::supplierPartCreateError);

    m_wizard->companyApi()->companyPartCreate(m_supplierPart);
}

void InvenTreePartUploader::supplierPartCreateError(InvenTree::SupplierPart summary, QNetworkReply::NetworkError error_type, const QString &error_str)
{
    Q_UNUSED(summary)
    Q_UNUSED(error_type)
    disconnect(m_wizard->companyApi(), nullptr, this, nullptr);
    emit stateFailed(CreateSupplierPart, error_str);
}

void InvenTreePartUploader::supplierPartCreated(InvenTree::SupplierPart summary)
{
    disconnect(m_wizard->companyApi(), nullptr, this, nullptr);
    emit stateChanged(CreateSupplierPart, SetDefaultSupplierPart);

    m_part.setDefaultSupplier(summary.getPk());

    connect(m_wizard->partApi(), &InvenTree::PartApi::partUpdateSignal, this, &InvenTreePartUploader::partUpdated);
    connect(m_wizard->partApi(), &InvenTree::PartApi::partUpdateSignalError, this, &InvenTreePartUploader::partUpdateError);

    m_wizard->partApi()->partUpdate(m_part.getPk(), m_part);
}

void InvenTreePartUploader::partUpdateError(InvenTree::Part summary, QNetworkReply::NetworkError error_type, const QString &error_str)
{
    Q_UNUSED(summary)
    Q_UNUSED(error_type)
    disconnect(m_wizard->partApi(), nullptr, this, nullptr);
    emit stateFailed(SetDefaultSupplierPart, error_str);
}

void InvenTreePartUploader::partUpdated(InvenTree::Part summary)
{
    Q_UNUSED(summary)
    disconnect(m_wizard->partApi(), nullptr, this, nullptr);
    emit stateChanged(SetDefaultSupplierPart, UploadImage);

}


