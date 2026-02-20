#include "inventreesettingsdialog.h"
#include "ui_inventreesettingsdialog.h"

InventreeSettingsDialog::InventreeSettingsDialog(QWidget *parent)
    : QDialog(parent),
    ui(new Ui::InventreeSettingsDialog),
    m_settings(),
    m_urlModel(new QStringListModel(this)),
    urlCompleter(new QCompleter(m_urlModel, this))
{
    ui->setupUi(this);

    setupCompleter();
    loadSettings();

    connect(ui->buttonBox, &QDialogButtonBox::accepted,
            this, &InventreeSettingsDialog::saveSettings);
    connect(ui->buttonBox, &QDialogButtonBox::rejected,
            this, &InventreeSettingsDialog::reject);
}

InventreeSettingsDialog::~InventreeSettingsDialog()
{
    delete ui;
}

void InventreeSettingsDialog::setupCompleter()
{
    m_settings.beginGroup("InventTree");

    QStringList urls = m_settings.value(KEY_URL_LIST).toStringList();
    m_urlModel->setStringList(urls);

    urlCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    urlCompleter->setCompletionMode(QCompleter::PopupCompletion);

    ui->editServerUrl->setCompleter(urlCompleter);
    m_settings.endGroup();
}

void InventreeSettingsDialog::loadSettings()
{
    m_settings.beginGroup("InventTree");
    ui->editServerUrl->setText(m_settings.value(KEY_SERVER).toString());
    ui->editApiToken->setText(m_settings.value(KEY_TOKEN).toString());
    m_settings.endGroup();
}

void InventreeSettingsDialog::saveSettings()
{
    m_settings.beginGroup("InventTree");
    QString url = ui->editServerUrl->text().trimmed();
    QString token = ui->editApiToken->text().trimmed();

    m_settings.setValue(KEY_SERVER, url);
    m_settings.setValue(KEY_TOKEN, token);

    // URL history update
    QStringList urls = m_settings.value(KEY_URL_LIST).toStringList();
    if (!url.isEmpty() && !urls.contains(url))
    {
        urls.prepend(url);
        m_settings.setValue(KEY_URL_LIST, urls);
        m_urlModel->setStringList(urls);
    }

    accept();
    m_settings.endGroup();
}

QString InventreeSettingsDialog::serverUrl() const
{
    return ui->editServerUrl->text().trimmed();
}

QString InventreeSettingsDialog::apiToken() const
{
    return ui->editApiToken->text().trimmed();
}
