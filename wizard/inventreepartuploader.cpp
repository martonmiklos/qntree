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

    connect(m_wizard ->partApi(), &InvenTree::PartApi::partUpdateSignal, this, &InvenTreePartUploader::partUpdated);
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
    emit stateChanged(SetDefaultSupplierPart, UploadPartImage);

    InvenTree::Part part;
    part.setPk(m_part.getPk());

    QList<InvenTree::HttpFileElement> files;
    InvenTree::HttpFileElement image;
    QTemporaryFile localFile("XXXXXX.png");
    if (localFile.open()) {
        m_wizard->m_selectedPart.image().save(&localFile, "PNG");
    }
    image.loadFromFile("image", localFile.fileName(), m_wizard->m_selectedPart.name() + ".png", "image/png");
    files.append(image);

    connect(m_wizard->partApi(), &InvenTree::PartApi::partPartialUpdateSignal, this, &InvenTreePartUploader::imageUploaded);
    connect(m_wizard->partApi(), &InvenTree::PartApi::partPartialUpdateSignalError, this, &InvenTreePartUploader::imageUploadError);
    m_wizard->partApi()->partPartialUpdate(m_part.getPk(), InvenTree::OptionalParam<InvenTree::PatchedPart>(), files);
}

void InvenTreePartUploader::imageUploadError(InvenTree::Part summary, QNetworkReply::NetworkError error_type, const QString &error_str)
{
    Q_UNUSED(summary)
    Q_UNUSED(error_type)
    disconnect(m_wizard->partApi(), nullptr, this, nullptr);
    emit stateFailed(UploadPartImage, error_str);
}

void InvenTreePartUploader::imageUploaded(InvenTree::Part summary)
{
    Q_UNUSED(summary)
    disconnect(m_wizard->partApi(), nullptr, this, nullptr);

    QList<InvenTree::StockItem> items;
    m_defaultLocationId = -1;
    m_wizard->initStockItems(m_part.getPk(), &items, &m_defaultLocationId);
    if (items.count() == 0) {
        // no stock items to upload
        createParameters();
    } else {
        m_stockItemCreationLeft = items.count();
        connect(m_wizard->stockApi(), &InvenTree::StockApi::stockCreateSignal, this, &InvenTreePartUploader::stockItemCreated);
        connect(m_wizard->stockApi(), &InvenTree::StockApi::stockCreateSignalError, this, &InvenTreePartUploader::stockItemCreateError);
        emit stateChanged(UploadPartImage, AddStockItems);
        for (const auto &i : std::as_const(items)) {
            m_wizard->stockApi()->stockCreate(i);
        }
    }
}

void InvenTreePartUploader::stockItemCreateError(InvenTree::StockItem summary, QNetworkReply::NetworkError error_type, const QString &error_str)
{
    Q_UNUSED(summary)
    Q_UNUSED(error_type)
    disconnect(m_wizard->stockApi(), nullptr, this, nullptr);
    emit stateFailed(AddStockItems, error_str);
}

void InvenTreePartUploader::stockItemCreated(InvenTree::StockItem summary)
{
    m_stockItemCreationLeft--;
    if (m_stockItemCreationLeft == 0) {
        Q_UNUSED(summary)
        disconnect(m_wizard->stockApi(), nullptr, this, nullptr);

        if (m_defaultLocationId != -1) {
            connect(m_wizard->partApi(), &InvenTree::PartApi::partPartialUpdateSignal, this, &InvenTreePartUploader::defaultStockLocationSet);
            connect(m_wizard->partApi(), &InvenTree::PartApi::partPartialUpdateSignalError, this, &InvenTreePartUploader::defaultStockLocationSetError);
            InvenTree::PatchedPart p;
            p.setDefaultLocation(m_defaultLocationId);
            m_wizard->partApi()->partPartialUpdate(m_part.getPk(), InvenTree::OptionalParam<InvenTree::PatchedPart>(p));
        } else {
            createParameters();
        }
    }
}

void InvenTreePartUploader::defaultStockLocationSetError(InvenTree::Part summary, QNetworkReply::NetworkError error_type, const QString &error_str)
{
    Q_UNUSED(summary)
    Q_UNUSED(error_type)
    disconnect(m_wizard->partApi(), nullptr, this, nullptr);
    emit stateFailed(AddStockItems, error_str);
}

void InvenTreePartUploader::defaultStockLocationSet(InvenTree::Part summary)
{
    Q_UNUSED(summary)
    disconnect(m_wizard->partApi(), nullptr, this, nullptr);
    createParameters();
}

void InvenTreePartUploader::createParameters()
{
    QList<InvenTree::PartParameter> params;
    m_wizard->initParameterList(m_part.getPk(), &params);
    if (params.count()) {
        emit stateChanged(m_state, CreateParameters);
        m_paramsCreationLeft = params.count();
        for (const auto &p : params) {
            m_wizard->partApi()->partParameterCreate(p);
        }
    } else {
        setupPricing();
        emit stateChanged(CreateParameters, SetupSuppliersAndPricing);
    }
}

void InvenTreePartUploader::partParameterCreateError(InvenTree::PartParameter summary, QNetworkReply::NetworkError error_type, const QString &error_str)
{
    Q_UNUSED(summary)
    Q_UNUSED(error_type)
    disconnect(m_wizard->partApi(), nullptr, this, nullptr);
    emit stateFailed(CreateParameters, error_str);
}

void InvenTreePartUploader::partParameterCreated(InvenTree::PartParameter summary)
{
    m_paramsCreationLeft--;
    if (m_paramsCreationLeft == 0) {
        Q_UNUSED(summary)
        disconnect(m_wizard->partApi(), nullptr, this, nullptr);
        setupPricing();
    }
}

void InvenTreePartUploader::setupPricing()
{
    emit stateChanged(CreateParameters, SetupSuppliersAndPricing);
}
