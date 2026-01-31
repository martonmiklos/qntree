#pragma once

#include "inventreepartimportwizard.h"
#include <QObject>

class InvenTreePartUploader : public QObject
{
    Q_OBJECT
public:
    enum State {
        Idle,
        CreatePart,
        CreateManufacturerPart,
        CreateSupplierPart,
        SetDefaultSupplierPart,
        UploadPartImage,
        AddStockItems,
        CreateParameters,
        SetupSuppliersAndPricing,
        UploadFiles,
        Finished
    };
    explicit InvenTreePartUploader(InvenTreePartImportWizard *parent = nullptr);
    void start();

signals:
    void stateChanged(InvenTreePartUploader::State oldState, InvenTreePartUploader::State newState);
    void stateFailed(InvenTreePartUploader::State state, const QString &error);

private:
    InvenTreePartImportWizard *m_wizard = nullptr;
    InvenTree::Part m_part;
    InvenTree::SupplierPart m_supplierPart;
    InvenTree::ManufacturerPart m_mfrPart;
    State m_state = Idle;
    int m_stockItemCreationLeft = 0;
    int m_defaultLocationId = -1;
    int m_paramsCreationLeft = 0;

    void createParameters();
    void setupPricing();

private slots:
    void partCreated(InvenTree::Part summary);
    void partCreateError(InvenTree::Part summary, QNetworkReply::NetworkError error_type, const QString &error_str);

    void mfrPartCreated(InvenTree::ManufacturerPart summary);
    void mfrPartCreateError(InvenTree::ManufacturerPart summary, QNetworkReply::NetworkError error_type, const QString &error_str);

    void supplierPartCreated(InvenTree::SupplierPart summary);
    void supplierPartCreateError(InvenTree::SupplierPart summary, QNetworkReply::NetworkError error_type, const QString &error_str);

    void partUpdated(InvenTree::Part summary);
    void partUpdateError(InvenTree::Part summary, QNetworkReply::NetworkError error_type, const QString &error_str);

    void imageUploaded(InvenTree::Part summary);
    void imageUploadError(InvenTree::Part summary, QNetworkReply::NetworkError error_type, const QString &error_str);

    void stockItemCreated(InvenTree::StockItem summary);
    void stockItemCreateError(InvenTree::StockItem summary, QNetworkReply::NetworkError error_type, const QString &error_str);

    void defaultStockLocationSet(InvenTree::Part summary);
    void defaultStockLocationSetError(InvenTree::Part summary, QNetworkReply::NetworkError error_type, const QString &error_str);

    void partParameterCreated(InvenTree::PartParameter summary);
    void partParameterCreateError(InvenTree::PartParameter summary, QNetworkReply::NetworkError error_type, const QString &error_str);
};
