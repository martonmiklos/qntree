#include "wizardpagestockandpricing.h"
#include "ui_wizardpagestockandpricing.h"

#include "InvenTree_dialogs/dialogselectinventreestocklocation.h"

WizardPageStockAndPricing::WizardPageStockAndPricing(InvenTree::StockApi *api, InvenTreePartImportWizard *parent)
    : InvenTreePartImportWizardPage(parent)
    , ui(new Ui::WizardPageStockAndPricing),
    m_stockApi(api)
{
    ui->setupUi(this);
    m_stockWidgets
        << ui->checkBoxMakeStockLocationDefault
        << ui->textEditStockNotes
        << ui->doubleSpinBoxStockQuantity
        << ui->doubleSpinBoxUnitPrice
        << ui->comboBoxUnitPriceCurrency
        << ui->toolButtonChangeTargetLocation;
    m_priceBreakModel = new PricebreaksModel(this);
    ui->tableViewPriceBreaks->setModel(m_priceBreakModel);
    ui->tableViewPriceBreaks->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
}

WizardPageStockAndPricing::~WizardPageStockAndPricing()
{
    delete ui;
}

bool WizardPageStockAndPricing::isComplete() const
{
    return ui->checkBoxCreateStock->isChecked() && !qFuzzyIsNull(ui->doubleSpinBoxStockQuantity->value());
}

void WizardPageStockAndPricing::update()
{
    ui->doubleSpinBoxStockQuantity->setSuffix(" " + m_wizard->m_selectedPart.unit());
}

void WizardPageStockAndPricing::setPart(SupplierPart *part)
{
    m_priceBreakModel->setPart(part);
    m_priceWasEdited = false;
    updatePriceFromPriceBreaks();
}

void WizardPageStockAndPricing::on_doubleSpinBoxUnitPrice_valueChanged(double arg1)
{
    Q_UNUSED(arg1);
    emit completeChanged();
}

void WizardPageStockAndPricing::on_checkBoxCreateStock_toggled(bool checked)
{
    for (auto widget : qAsConst(m_stockWidgets)) {
        widget->setEnabled(checked);
    }
    emit completeChanged();
}

void WizardPageStockAndPricing::on_toolButtonChangeTargetLocation_clicked()
{
    auto dlg = new DialogSelectInvenTreeStockLocation(m_stockApi, this);
    dlg->show();
    connect(dlg, &DialogSelectInvenTreeStockLocation::stockLocationSelected,
            this, [=](int pk, const QString &locationName, const QString &locationPath) {
        ui->labelStockLocation->setText(locationName);
        ui->labelStockLocation->setToolTip(locationPath);
        m_selectedLocationPk = pk;
        dlg->close();
    });
}

int WizardPageStockAndPricing::selectedLocationPk() const
{
    return m_selectedLocationPk;
}

void WizardPageStockAndPricing::updatePriceFromPriceBreaks()
{
    QString currency;
    qreal calculatedPrice = m_priceBreakModel->getPriceForQuantity(ui->doubleSpinBoxStockQuantity->value(), &currency);
    if (calculatedPrice > 0) {
        ui->doubleSpinBoxUnitPrice->setValue(calculatedPrice);
        auto currencyIndex = ui->comboBoxUnitPriceCurrency->findData(currency);
        if (currencyIndex != -1)
            ui->comboBoxUnitPriceCurrency->setCurrentIndex(currencyIndex);
        else
            ui->comboBoxUnitPriceCurrency->addItem(currency, currency);
    }
}

void WizardPageStockAndPricing::on_doubleSpinBoxStockQuantity_valueChanged(double arg1)
{
    if (m_priceWasEdited)
        return;
    updatePriceFromPriceBreaks();
}


void WizardPageStockAndPricing::on_doubleSpinBoxUnitPrice_editingFinished()
{
    m_priceWasEdited = true;
}

