#include "mousersettingsdialog.h"
#include "ui_mousersettingsdialog.h"

MouserSettingsDialog::MouserSettingsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MouserSettingsDialog)
{
    ui->setupUi(this);
    loadSettings();

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &MouserSettingsDialog::saveSettings);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &MouserSettingsDialog::reject);
}

MouserSettingsDialog::~MouserSettingsDialog()
{
    delete ui;
}

void MouserSettingsDialog::loadSettings()
{
    m_settings.beginGroup(SETTINGS_GROUP);
    ui->lineEditApiKey->setText(m_settings.value(KEY_API_KEY).toString());
    ui->spinBoxInvenTreeSupplierId->setValue(m_settings.value(KEY_INVENTREE_SUPPLIER_ID, 0).toInt());
    m_settings.endGroup();
}

void MouserSettingsDialog::saveSettings()
{
    m_settings.beginGroup(SETTINGS_GROUP);
    m_settings.setValue(KEY_API_KEY, ui->lineEditApiKey->text().trimmed());
    m_settings.setValue(KEY_INVENTREE_SUPPLIER_ID, ui->spinBoxInvenTreeSupplierId->value());
    m_settings.endGroup();
    accept();
}
