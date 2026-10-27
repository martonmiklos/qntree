#include "tmesettingsdialog.h"
#include "ui_tmesettingsdialog.h"

#include "tme.h"
#include "inventreeprofilemanager.h"

TMESettingsDialog::TMESettingsDialog(QWidget *parent)
    : SupplierSettingsDialog(parent)
    , ui(new Ui::TMESettingsDialog)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &TMESettingsDialog::saveSettings);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &TMESettingsDialog::reject);
}

TMESettingsDialog::~TMESettingsDialog()
{
    delete ui;
}

void TMESettingsDialog::loadSettings()
{
    TME::instance()->loadSettings();
    ui->lineEditSecret->setText(TME::instance()->secret());
    ui->lineEditToken->setText(TME::instance()->token());
    ui->lineEditCountry->setText(TME::instance()->country());
    ui->lineEditLanguage->setText(TME::instance()->language());
    ui->lineEditCurrency->setText(TME::instance()->currency());
    ui->spinBoxInvenTreeSupplierId->setValue(TME::instance()->invenTreeId());
}

void TMESettingsDialog::saveSettings()
{
    TME::instance()->setSecret(ui->lineEditSecret->text());
    TME::instance()->setToken(ui->lineEditToken->text());
    TME::instance()->setCountry(ui->lineEditCountry->text());
    TME::instance()->setLanguage(ui->lineEditLanguage->text());
    TME::instance()->setCurrency(ui->lineEditCurrency->text());
    TME::instance()->setInventreeId(ui->spinBoxInvenTreeSupplierId->value());
    TME::instance()->saveSettings();
    accept();
}
