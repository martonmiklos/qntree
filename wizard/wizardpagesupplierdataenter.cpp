#include "wizardpagesupplierdataenter.h"
#include "ui_wizardpagesupplierdataenter.h"

#include <QGuiApplication>
#include <QClipboard>
#include <QMimeData>
#include <QThread>

#include "../supplier/supplierregistry.h"
#include "../supplier/supplierpart.h"

WizardPageSupplierDataEnter::WizardPageSupplierDataEnter(InvenTreePartImportWizard *parent)
    : InvenTreePartImportWizardPage(parent)
    , ui(new Ui::WizardPageSupplierDataEnter)
{
    m_wizard = static_cast<InvenTreePartImportWizard*>(parent);
    ui->setupUi(this);
    for (const auto &supplierData : SupplierRegistry::instance()->getSuppliersDisplayInfo()) {
        ui->comboBoxSupplier->addItem(supplierData.first, supplierData.second);
    }

    QClipboard *clipboard = QGuiApplication::clipboard();
    if (clipboard->mimeData()->hasText()) {
        auto clipText = clipboard->text();
        if (clipText.length() < 20)
            ui->lineEditPartNumber->setText(clipText);
    }
}

WizardPageSupplierDataEnter::~WizardPageSupplierDataEnter()
{
    partDataError = true;
    delete ui;
}

void WizardPageSupplierDataEnter::on_lineEditPartNumber_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)
    emit completeChanged();
}

bool WizardPageSupplierDataEnter::isComplete() const
{
    return !ui->lineEditPartNumber->text().isEmpty() && ui->comboBoxSupplier->currentIndex() != -1;
}

bool WizardPageSupplierDataEnter::validatePage()
{
    m_selectedSupplier = SupplierRegistry::instance()->getSupplierByUId(ui->comboBoxSupplier->currentData().toString());
    connect(m_selectedSupplier, &AbstractSupplier::supplierPartRetrived, this, [=](SupplierPart &part) {
        partDataRetrived = true;
        m_wizard->setSelectedPart(part);
    });

    connect(m_selectedSupplier, &AbstractSupplier::error, this, [=](const QString &error) {
        partDataError = true;
        ui->labelMessage->setText(error);
    });

    connect(m_selectedSupplier, &AbstractSupplier::partNotFound, this, [=]() {
        partDataError = true;
        ui->labelMessage->setText(tr("No part data found for the %1 partnumber").arg(ui->lineEditPartNumber->text()));
    });

    m_selectedSupplier->retrivePart(ui->lineEditPartNumber->text());

    partDataRetrived = false;
    partDataError = false;

    int counter = 0, dotCounter = 0;
    ui->labelMessage->setText(tr("Keep calm retriving data"));
    while (!partDataRetrived && !partDataError) {
        if (partDataRetrived) {
            return true;
        }

        if (partDataError) {
            return false;
        }
        QThread::msleep(50);
        QCoreApplication::processEvents();
        counter++;
        if (counter == 1000 / 50) {
            counter = 0;
            dotCounter++;
            if (dotCounter == 4) {
                dotCounter = 1;
            }
            QString dotText;
            for (int i = 0; i<dotCounter; i++)
                dotText.append(".");
            ui->labelMessage->setText(tr("Keep calm retriving data") + dotText);
        }
    }
    return true;
}

void WizardPageSupplierDataEnter::update()
{

}

void WizardPageSupplierDataEnter::on_comboBoxSupplier_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    auto selectedSupplier = SupplierRegistry::instance()->getSupplierByUId(ui->comboBoxSupplier->currentData().toString());
    ui->lineEditPartNumber->setMaxLength(selectedSupplier->pnLengthLimit());
}

AbstractSupplier *WizardPageSupplierDataEnter::selectedSupplier() const
{
    return m_selectedSupplier;
}

