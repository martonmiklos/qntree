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
    QList<InvenTree::Attachment> m_attachments;
    int m_stockItemCreationLeft = 0;
    int m_defaultLocationId = -1;
    int m_paramsCreationLeft = 0;
    int m_priceBreakLeft = 0;
    int m_attachmentsLeft = 0;

    void createParameters();
    void setupPricing();
    void uploadAttachments();

    void downloadToTempFile(const QString &sourceUrl,
                            const QString &fileName,
                            QObject *context,
                            std::function<void(const QString &localFileUrl, const QString &mime)> onSuccess,
                            std::function<void(const QString &error)> onError);
    
    void createManufacturerPart();
    
private slots:
    void partCreated(InvenTree::Part summary);
    void partCreateError(InvenTree::Part summary, QNetworkReply::NetworkError error_type, const QString &error_str);

    void mfrCompanyCreated(InvenTree::Company summary);
    void mfrCompanyCreateError(InvenTree::Company summary, QNetworkReply::NetworkError error_type, const QString &error_str);

    void mfrPartCreated(InvenTree::ManufacturerPart summary);
    void mfrPartCreateError(InvenTree::ManufacturerPart summary, QNetworkReply::NetworkError error_type, const QString &error_str);

    void supplierPartCreated(InvenTree::SupplierPart summary);
    void supplierPartCreateError(InvenTree::SupplierPart summary, QNetworkReply::NetworkError error_type, const QString &error_str);

    void partUpdated(InvenTree::Part summary);
    void partUpdateError(InvenTree::Part summary, QNetworkReply::NetworkError error_type, const QString &error_str);

    void imageUploaded(InvenTree::Part summary);
    void imageUploadError(InvenTree::Part summary, QNetworkReply::NetworkError error_type, const QString &error_str);

    void stockItemCreated(QList<InvenTree::StockItem> summary);
    void stockItemCreateError(QList<InvenTree::StockItem> summary, QNetworkReply::NetworkError error_type, const QString &error_str);

    void defaultStockLocationSet(InvenTree::Part summary);
    void defaultStockLocationSetError(InvenTree::Part summary, QNetworkReply::NetworkError error_type, const QString &error_str);

    /*void partParameterCreated(InvenTree::PartParameter summary);
    void partParameterCreateError(InvenTree::PartParameter summary, QNetworkReply::NetworkError error_type, const QString &error_str);*/

    void priceBreakCreated(InvenTree::SupplierPriceBreak summary);
    void priceBreakCreateError(InvenTree::SupplierPriceBreak summary, QNetworkReply::NetworkError error_type, const QString &error_str);

    void attachmentCreated(InvenTree::Attachment summary);
    void attachmentCreateError(InvenTree::Attachment summary, QNetworkReply::NetworkError error_type, const QString &error_str);
};
