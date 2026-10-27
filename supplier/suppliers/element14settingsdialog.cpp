#include "element14settingsdialog.h"
#include "ui_element14settingsdialog.h"


#include "element14.h"

Element14SettingsDialog::Element14SettingsDialog(QWidget *parent)
    : SupplierSettingsDialog(parent)
    , ui(new Ui::Element14SettingsDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &Element14SettingsDialog::saveSettings);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &Element14SettingsDialog::reject);
}

Element14SettingsDialog::~Element14SettingsDialog()
{
    delete ui;
}

void Element14SettingsDialog::loadSettings()
{
    Element14::instance()->loadSettings();
    ui->lineEditApiKey->setText(Element14::instance()->apiKey());
    ui->lineEditStoreId->setText(Element14::instance()->storeId());
    ui->spinBoxInvenTreeSupplierId->setValue(Element14::instance()->invenTreeId());
}

void Element14SettingsDialog::saveSettings()
{
    Element14::instance()->setApiKey(ui->lineEditApiKey->text().trimmed());
    Element14::instance()->setStoreId(ui->lineEditStoreId->text());
    Element14::instance()->setInventreeId(ui->spinBoxInvenTreeSupplierId->value());
    Element14::instance()->saveSettings();
    accept();
}
