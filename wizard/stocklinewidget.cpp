#include "stocklinewidget.h"
#include "InvenTree_dialogs/dialogselectinventreestocklocation.h"
#include "inventreepartimportwizard.h"
#include "ui_stocklinewidget.h"

StockLineWidget::StockLineWidget(InvenTreePartImportWizard *wizard, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StockLineWidget)
    , m_wizard(wizard)
{
    ui->setupUi(this);

    m_stockWidgets
        << ui->checkBoxMakeStockLocationDefault
        << ui->toolButtonEditStockNote
        << ui->doubleSpinBoxStockQuantity
        << ui->toolButtonChangeTargetLocation;
}

StockLineWidget::~StockLineWidget()
{
    delete ui;
}

void StockLineWidget::setRemovable(bool removable)
{
    ui->toolButtonRemoveStock->setEnabled(removable);
}

void StockLineWidget::on_toolButtonNewStock_clicked()
{
    emit addNewLine();
}

bool StockLineWidget::ready() const
{
    if (!ui->checkBoxCreateStock->isChecked())
        return true;

    if (ui->doubleSpinBoxStockQuantity->value() == 0)
        return false;

    return true;
}

void StockLineWidget::setQtySuffix(const QString &suffix)
{
    ui->doubleSpinBoxStockQuantity->setSuffix(suffix);
}

void StockLineWidget::setAsDefaultLocation(bool defaultLocation)
{
    ui->checkBoxCreateStock->setChecked(defaultLocation);
}

bool StockLineWidget::isDefaultLocation() const
{
    return ui->checkBoxMakeStockLocationDefault->isChecked();
}

bool StockLineWidget::create() const
{
    return ui->checkBoxCreateStock->isChecked();
}

QString StockLineWidget::locationName() const
{
    return ui->labelStockLocation->text();
}

int StockLineWidget::selectedLocationPk() const
{
    return m_selectedLocationPk;
}

qreal StockLineWidget::quantity() const
{
    if (!ui->checkBoxCreateStock->isChecked())
        return 0.0;

    return ui->doubleSpinBoxStockQuantity->value();
}

void StockLineWidget::on_toolButtonRemoveStock_clicked()
{
    emit removeLine();
}

void StockLineWidget::on_toolButtonEditStockNote_clicked()
{

}

void StockLineWidget::on_toolButtonChangeTargetLocation_clicked()
{
    auto dlg = new DialogSelectInvenTreeStockLocation(m_wizard->stockApi(), this);
    dlg->show();
    connect(dlg, &DialogSelectInvenTreeStockLocation::stockLocationSelected,
            this, [=](int pk, const QString &locationName, const QString &locationPath) {
                ui->labelStockLocation->setText(locationName);
                ui->labelStockLocation->setToolTip(locationPath);
                m_selectedLocationPk = pk;
                dlg->close();
            });
}

void StockLineWidget::on_doubleSpinBoxStockQuantity_valueChanged(double arg1)
{
    Q_UNUSED(arg1)
    emit quantityChanged();
}

void StockLineWidget::on_checkBoxMakeStockLocationDefault_clicked(bool checked)
{
    if (checked)
        emit setAsDefault();
}


void StockLineWidget::on_checkBoxCreateStock_clicked(bool checked)
{
    for (auto widget : m_stockWidgets) {
        widget->setEnabled(checked);
    }
    emit quantityChanged();
}

