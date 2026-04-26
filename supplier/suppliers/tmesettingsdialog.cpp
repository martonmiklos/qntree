#include "tmesettingsdialog.h"
#include "ui_tmesettingsdialog.h"

TMESettingsDialog::TMESettingsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TMESettingsDialog)
{
    ui->setupUi(this);
    loadSettings();

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &TMESettingsDialog::saveSettings);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &TMESettingsDialog::reject);
}

TMESettingsDialog::~TMESettingsDialog()
{
    delete ui;
}

void TMESettingsDialog::loadSettings()
{
    m_settings.beginGroup(SETTINGS_GROUP);
    ui->lineEditSecret->setText(m_settings.value(KEY_SECRET, "20948d40191f45adfebc").toString());
    ui->lineEditToken->setText(m_settings.value(KEY_TOKEN, "22ecd8bff75788af5b46130c83ec68f0326b0d53979ee4c9f1").toString());
    ui->lineEditCountry->setText(m_settings.value(KEY_COUNTRY, "HU").toString());
    ui->lineEditLanguage->setText(m_settings.value(KEY_LANGUAGE, "HU").toString());
    ui->lineEditCurrency->setText(m_settings.value(KEY_CURRENCY, "EUR").toString());
    ui->checkBoxGrossPrices->setChecked(m_settings.value(KEY_GROSS_PRICES, true).toBool());
    ui->spinBoxInvenTreeSupplierId->setValue(m_settings.value(KEY_INVENTREE_SUPPLIER_ID, 4).toInt());
    m_settings.endGroup();
}

void TMESettingsDialog::saveSettings()
{
    m_settings.beginGroup(SETTINGS_GROUP);
    m_settings.setValue(KEY_SECRET, ui->lineEditSecret->text().trimmed());
    m_settings.setValue(KEY_TOKEN, ui->lineEditToken->text().trimmed());
    m_settings.setValue(KEY_COUNTRY, ui->lineEditCountry->text().trimmed());
    m_settings.setValue(KEY_LANGUAGE, ui->lineEditLanguage->text().trimmed());
    m_settings.setValue(KEY_CURRENCY, ui->lineEditCurrency->text().trimmed());
    m_settings.setValue(KEY_GROSS_PRICES, ui->checkBoxGrossPrices->isChecked());
    m_settings.setValue(KEY_INVENTREE_SUPPLIER_ID, ui->spinBoxInvenTreeSupplierId->value());
    m_settings.endGroup();
    accept();
}
