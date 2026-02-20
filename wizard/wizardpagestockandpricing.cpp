#include "wizardpagestockandpricing.h"
#include "ui_wizardpagestockandpricing.h"

#include "stocklinewidget.h"

WizardPageStockAndPricing::WizardPageStockAndPricing(InvenTreePartImportWizard *parent)
    : InvenTreePartImportWizardPage(parent)
    , ui(new Ui::WizardPageStockAndPricing)
{
    ui->setupUi(this);
    m_priceBreakModel = new PricebreaksModel(this);
    m_priceBreakModel->setPart(&m_wizard->m_selectedPart);
    ui->tableViewPriceBreaks->setModel(m_priceBreakModel);
    ui->tableViewPriceBreaks->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    addNewLineWidget();

    /*connect(m_wizard->currencyApi(), &InvenTree::CurrencyApi::currencyExchangeRetrieveSignal, this, [=] (InvenTree::CurrencyExchange summary) {
        disconnect(m_wizard->currencyApi(), nullptr, this, nullptr);

        for (auto c : summary.)
        for (int i = 0; i<std::numeric_limits<int>().max(); i++) {
            e.setValue(static_cast<InvenTree::SalePriceCurrencyEnum::eSalePriceCurrencyEnum>(i));
            auto s = e.asJson();
            if (s.isEmpty())
                return;
            ui->comboBoxUnitPriceCurrency->addItem(s);
        }
    });
    m_wizard->currencyApi()->currencyExchangeRetrieve();*/

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
    m_priceWasEdited = false;
    updatePriceFromPriceBreaks();
    for (auto line : m_stockLines) {
        line->setQtySuffix(" " + m_wizard->m_selectedPart.unit());
    }
}

void WizardPageStockAndPricing::on_doubleSpinBoxUnitPrice_valueChanged(double arg1)
{
    Q_UNUSED(arg1);
    emit completeChanged();
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
                ret.append(tr("<li>%1%2 %3 %4</li>").arg(line->locationName(), line->isDefaultLocation() ? tr(" (make it default)") : QString()).arg(line->quantity()).arg(m_wizard->m_selectedPart.unit()));
            }
        }
        ret.append("</ul><br>");
    }

    if (ui->checkBoxSavePriceBreaks->isChecked()) {
        ret.append(tr("<b>Create price breaks</b><br><ul>"));
        int min = 1;
        for (auto &pb : m_wizard->m_selectedPart.priceRanges()) {
            QString minMax;
            if (min == pb.qtyMin)
                minMax = tr("%1 pc").arg(min);
            else
                minMax = tr("%1 - %2 pcs").arg(min).arg(pb.qtyMin);

            // highlight the price break where we are
            bool current = false;
            if (min <= ui->doubleSpinBoxUnitPrice->value() && ui->doubleSpinBoxUnitPrice->value() < pb.qtyMin)
                current = true;
            ret.append(QString("<li>%1: %2 %3%4</li>").arg(minMax).arg(pb.price).arg(pb.currency, current ? " <-" : QString()));
            min = pb.qtyMin + 1;
        }
        ret.append("</ul><br>");
    }
    return ret;
}

void WizardPageStockAndPricing::resetStockLines(qreal initialQty)
{
    while (m_stockLines.count() > 1) {
        auto l = m_stockLines.takeFirst();
        ui->verticalLayoutStock->removeWidget(l);
    }

    if (initialQty > 0) {
        m_stockLines.first()->setQuantity(initialQty);
    }
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
    lineWidget->setQtySuffix(" " + m_wizard->m_selectedPart.unit());
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



