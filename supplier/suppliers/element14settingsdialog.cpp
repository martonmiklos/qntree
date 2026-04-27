#include "element14settingsdialog.h"
#include "ui_element14settingsdialog.h"

Element14SettingsDialog::Element14SettingsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Element14SettingsDialog)
{
    ui->setupUi(this);
    loadSettings();

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &Element14SettingsDialog::saveSettings);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &Element14SettingsDialog::reject);
}

Element14SettingsDialog::~Element14SettingsDialog()
{
    delete ui;
}

void Element14SettingsDialog::loadSettings()
{
    m_settings.beginGroup(SETTINGS_GROUP);
    ui->lineEditApiKey->setText(m_settings.value(KEY_API_KEY).toString());
    ui->lineEditStoreId->setText(m_settings.value(KEY_STORE_ID, QStringLiteral("us.newark.com")).toString());
    ui->spinBoxInvenTreeSupplierId->setValue(m_settings.value(KEY_INVENTREE_SUPPLIER_ID, 0).toInt());
    m_settings.endGroup();
}

void Element14SettingsDialog::saveSettings()
{
    m_settings.beginGroup(SETTINGS_GROUP);
    m_settings.setValue(KEY_API_KEY, ui->lineEditApiKey->text().trimmed());
    m_settings.setValue(KEY_STORE_ID, ui->lineEditStoreId->text().trimmed());
    m_settings.setValue(KEY_INVENTREE_SUPPLIER_ID, ui->spinBoxInvenTreeSupplierId->value());
    m_settings.endGroup();
    accept();
}
