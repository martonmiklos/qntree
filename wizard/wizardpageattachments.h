#pragma once

#include "inventreepartimportwizard.h"
#include "inventreepartimportwizardpage.h"
#include <QWizardPage>

class SupplierAttachmentsModel;

namespace Ui {
class WizardPageAttachments;
}

class WizardPageAttachments : public InvenTreePartImportWizardPage
{
    Q_OBJECT

public:
    explicit WizardPageAttachments(InvenTreePartImportWizard *parent = nullptr);
    virtual void setSelectedPart(::SupplierPart *newSelectedPart) override;
    ~WizardPageAttachments();

    QString summary() const override;

private slots:
    void on_tableViewAttachmentMapping_clicked(const QModelIndex &index);

private:
    SupplierAttachmentsModel *m_attachmentsModel = nullptr;
    QNetworkAccessManager *m_networkAccessManager = nullptr;

    Ui::WizardPageAttachments *ui;
};
