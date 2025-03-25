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
        CreateParameters,
        SetupSuppliersAndPricing,
        UploadFiles,
        Finished
    };
    explicit InvenTreePartUploader(InvenTreePartImportWizard *parent = nullptr);
    void start();

signals:
    void stateChanged(State oldState, State newState);
    void stateFailed(State state, const QString &error);
private:
    InvenTreePartImportWizard *m_wizard = nullptr;
    InvenTree::Part m_part;
    State m_state = Idle;

private slots:
    void partCreated(InvenTree::Part summary);
    void partCreateSignalError(InvenTree::Part summary, QNetworkReply::NetworkError error_type, const QString &error_str);
};
