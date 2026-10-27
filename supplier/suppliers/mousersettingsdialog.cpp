#include "mousersettingsdialog.h"
#include "ui_mousersettingsdialog.h"

#include "mouser.h"

MouserSettingsDialog::MouserSettingsDialog(QWidget *parent)
    : SupplierSettingsDialog(parent)
    , ui(new Ui::MouserSettingsDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &MouserSettingsDialog::saveSettings);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &MouserSettingsDialog::reject);
}

MouserSettingsDialog::~MouserSettingsDialog()
{
    delete ui;
}

void MouserSettingsDialog::loadSettings()
{
    Mouser::instance()->loadSettings();
    ui->lineEditApiKey->setText(Mouser::instance()->apiKey());
    ui->spinBoxInvenTreeSupplierId->setValue(Mouser::instance()->invenTreeId());
}

void MouserSettingsDialog::saveSettings()
{
    Mouser::instance()->setApiKey(ui->lineEditApiKey->text());
    Mouser::instance()->setInventreeId(ui->spinBoxInvenTreeSupplierId->value());
    Mouser::instance()->saveSettings();
    accept();
}
