#include "wizardpagestockandpricing.h"
#include "ui_wizardpagestockandpricing.h"

#include "stocklinewidget.h"

WizardPageStockAndPricing::WizardPageStockAndPricing(InvenTreePartImportWizard *parent)
    : InvenTreePartImportWizardPage(parent)
    , ui(new Ui::WizardPageStockAndPricing)
{
    ui->setupUi(this);
    m_priceBreakModel = new PricebreaksModel(this);
    ui->tableViewPriceBreaks->setModel(m_priceBreakModel);
    ui->tableViewPriceBreaks->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    addNewLineWidget();
}

WizardPageStockAndPricing::~WizardPageStockAndPricing()
{
    delete ui;
}

bool WizardPageStockAndPricing::isComplete() const
{
    for (auto line : m_stockLines) {
        if (!line->ready())
            return false;
    }
    return true;
}

void WizardPageStockAndPricing::update()
{
    for (auto line : m_stockLines) {
        line->setQtySuffix(" " + m_selectedPart->unit());
    }
}

void WizardPageStockAndPricing::on_doubleSpinBoxUnitPrice_valueChanged(double arg1)
{
    Q_UNUSED(arg1);
    emit completeChanged();
}

void WizardPageStockAndPricing::setSelectedPart(SupplierPart *part)
{
    InvenTreePartImportWizardPage::setSelectedPart(part);
    m_priceBreakModel->setPart(part);
    m_priceWasEdited = false;
    updatePriceFromPriceBreaks();
}

QString WizardPageStockAndPricing::summary() const
{
    QString ret;
    bool createStock = false;
    for (auto line : m_stockLines) {
        if (line->create()) {
            createStock = true;
            break;
        }
    }

    if (createStock) {
        ret.append(tr("<b>Create stocks</b><br><ul>"));
        for (auto line : m_stockLines) {
            if (line->create()) {
                ret.append(tr("<li>%1%2</li>").arg(line->locationName(), line->isDefaultLocation() ? tr(" (make it default)") : QString()));
            }
        }
        ret.append("</ul><br>");
    }

    if (ui->checkBoxSavePriceBreaks->isChecked()) {
        ret.append(tr("<b>Create price breaks</b><br><ul>"));
        int min = 1;
        for (auto &pb : m_selectedPart->priceRanges()) {
            ret.append(QString("%1 - %2: %3 %4").arg(min).arg(pb.qtyMin).arg(pb.price).arg(pb.currency));
            min = pb.qtyMin + 1;
        }
        ret.append("</ul><br>");
    }
    return ret;
}

qreal WizardPageStockAndPricing::totalQuantity() const
{
    qreal ret = 0;
    for (auto line : m_stockLines) {
        ret += line->quantity();
    }
    return ret;
}

void WizardPageStockAndPricing::updatePriceFromPriceBreaks()
{
    QString currency;
    qreal calculatedPrice = m_priceBreakModel->getPriceForQuantity(totalQuantity(), &currency);
    if (calculatedPrice > 0) {
        ui->doubleSpinBoxUnitPrice->setValue(calculatedPrice);
        auto currencyIndex = ui->comboBoxUnitPriceCurrency->findData(currency);
        if (currencyIndex != -1)
            ui->comboBoxUnitPriceCurrency->setCurrentIndex(currencyIndex);
        else
            ui->comboBoxUnitPriceCurrency->addItem(currency, currency);
    }
}

void WizardPageStockAndPricing::addNewLineWidget()
{
    auto lineWidget = new StockLineWidget(m_wizard, this);
    if (m_stockLines.count() == 0) {
        lineWidget->setRemovable(false); // hide remove button from the first line
    } else {
        connect(lineWidget, &StockLineWidget::removeLine, this, [=]() {
            ui->verticalLayoutStock->removeWidget(lineWidget);
            m_stockLines.removeAll(lineWidget);
            lineWidget->deleteLater();
        });
    }
    m_stockLines.append(lineWidget);
    if (m_selectedPart)
        lineWidget->setQtySuffix(" " + m_selectedPart->unit());
    ui->verticalLayoutStock->addWidget(lineWidget);
    connect(lineWidget, &StockLineWidget::addNewLine, this, [=]() {
        addNewLineWidget();
    });

    connect(lineWidget, &StockLineWidget::setAsDefaultLocation, this, [=]() {
        for (auto line : m_stockLines) {
            if (line != this->sender()) {
                line->setAsDefaultLocation(false);
            }
        }
    });

    connect(lineWidget, &StockLineWidget::quantityChanged, this, &WizardPageStockAndPricing::quantityChanged);
}

void WizardPageStockAndPricing::quantityChanged()
{
    if (m_priceWasEdited)
        return;
    updatePriceFromPriceBreaks();
}

void WizardPageStockAndPricing::on_doubleSpinBoxUnitPrice_editingFinished()
{
    m_priceWasEdited = true;
}



