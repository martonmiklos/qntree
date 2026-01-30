#pragma once

#include "inventreepartimportwizard.h"
#include "inventreepartimportwizardpage.h"
#include "inventreepartuploader.h"
#include "qlabel.h"
#include <QWizardPage>

namespace Ui {
class WizardPageInvenTreeSyncStatus;
}

class WizardPageInvenTreeSyncStatus : public InvenTreePartImportWizardPage
{
    Q_OBJECT

public:
    explicit WizardPageInvenTreeSyncStatus(InvenTreePartImportWizard *parent = nullptr);
    ~WizardPageInvenTreeSyncStatus();
    bool isComplete() const override;

private:
    Ui::WizardPageInvenTreeSyncStatus *ui;
    bool m_completed = false;
    static QMap<InvenTreePartUploader::State, QLabel*> m_resultLabelMap;

public slots:
    void stateChanged(InvenTreePartUploader::State old, InvenTreePartUploader::State newState);
    void stateError(InvenTreePartUploader::State old, const QString &error);
};


