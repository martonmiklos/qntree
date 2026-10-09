#include "inventreesettingsdialog.h"
#include "ui_inventreesettingsdialog.h"

#include <QComboBox>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPushButton>
#include <QSignalBlocker>
#include <QUrl>

InventreeSettingsDialog::InventreeSettingsDialog(QWidget *parent)
    : QDialog(parent),
    ui(new Ui::InventreeSettingsDialog),
    m_settings(),
    m_urlModel(new QStringListModel(this)),
    urlCompleter(new QCompleter(m_urlModel, this)),
    m_networkAccessManager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);

    setupCompleter();
    loadSettings();

    connect(ui->buttonBox, &QDialogButtonBox::accepted,
            this, &InventreeSettingsDialog::saveSettings);
    connect(ui->buttonBox, &QDialogButtonBox::rejected,
            this, &InventreeSettingsDialog::reject);
    connect(ui->buttonTestConnection, &QPushButton::clicked,
            this, &InventreeSettingsDialog::testConnection);
    connect(ui->buttonSaveProfile, &QPushButton::clicked,
            this, &InventreeSettingsDialog::saveConnectionProfile);
    connect(ui->comboConnectionProfiles, &QComboBox::currentTextChanged,
            this, &InventreeSettingsDialog::loadConnectionProfile);
    connect(ui->buttonDeleteProfile, &QPushButton::clicked,
            this, &InventreeSettingsDialog::deleteConnectionProfile);

    refreshConnectionProfiles();
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

void InventreeSettingsDialog::refreshConnectionProfiles(const QString &selectedProfile)
{
    m_settings.beginGroup("InventTree");
    const QStringList profiles = m_settings.value(KEY_CONNECTION_PROFILES).toStringList();
    m_settings.endGroup();

    const QSignalBlocker blocker(ui->comboConnectionProfiles);
    ui->comboConnectionProfiles->clear();
    ui->comboConnectionProfiles->addItem(tr("Select saved connection..."));
    ui->comboConnectionProfiles->addItems(profiles);

    const int index = ui->comboConnectionProfiles->findText(selectedProfile);
    ui->comboConnectionProfiles->setCurrentIndex(index >= 0 ? index : 0);
    ui->buttonDeleteProfile->setEnabled(index >= 0);
}

void InventreeSettingsDialog::saveConnectionProfile()
{
    const QString name = ui->editProfileName->text().trimmed();
    if (name.isEmpty()) {
        QMessageBox::warning(this, tr("Save connection"), tr("Enter a name for the saved connection."));
        return;
    }
    if (name.contains('/') || name.contains('\\')) {
        QMessageBox::warning(this, tr("Save connection"), tr("The connection name cannot contain '/' or '\\'."));
        return;
    }

    m_settings.beginGroup("InventTree");
    QStringList profiles = m_settings.value(KEY_CONNECTION_PROFILES).toStringList();
    if (!profiles.contains(name)) {
        profiles.append(name);
        m_settings.setValue(KEY_CONNECTION_PROFILES, profiles);
    }
    m_settings.beginGroup(QStringLiteral("connection_profiles/%1").arg(name));
    m_settings.setValue(KEY_SERVER, ui->editServerUrl->text().trimmed());
    m_settings.setValue(KEY_TOKEN, ui->editApiToken->text().trimmed());
    m_settings.endGroup();
    m_settings.endGroup();

    refreshConnectionProfiles(name);
}

void InventreeSettingsDialog::loadConnectionProfile(const QString &profileName)
{
    if (profileName.isEmpty() || ui->comboConnectionProfiles->currentIndex() == 0) {
        ui->buttonDeleteProfile->setEnabled(false);
        return;
    }

    m_settings.beginGroup("InventTree");
    m_settings.beginGroup(QStringLiteral("connection_profiles/%1").arg(profileName));
    ui->editServerUrl->setText(m_settings.value(KEY_SERVER).toString());
    ui->editApiToken->setText(m_settings.value(KEY_TOKEN).toString());
    m_settings.endGroup();
    m_settings.endGroup();

    ui->editProfileName->setText(profileName);
    ui->buttonDeleteProfile->setEnabled(true);
}

void InventreeSettingsDialog::deleteConnectionProfile()
{
    const QString name = ui->comboConnectionProfiles->currentText();
    if (ui->comboConnectionProfiles->currentIndex() == 0 || name.isEmpty())
        return;

    m_settings.beginGroup("InventTree");
    QStringList profiles = m_settings.value(KEY_CONNECTION_PROFILES).toStringList();
    profiles.removeAll(name);
    m_settings.setValue(KEY_CONNECTION_PROFILES, profiles);
    m_settings.remove(QStringLiteral("connection_profiles/%1").arg(name));
    m_settings.endGroup();

    ui->editProfileName->clear();
    refreshConnectionProfiles();
}

QUrl InventreeSettingsDialog::healthCheckUrl(const QString &serverUrl) const
{
    QUrl url(serverUrl.trimmed());
    QString path = url.path();
    if (!path.endsWith('/'))
        path.append('/');
    path.append(QStringLiteral("api/system/health/"));
    url.setPath(path);
    url.setQuery(QString());
    url.setFragment(QString());
    return url;
}

void InventreeSettingsDialog::testConnection()
{
    const QUrl url = healthCheckUrl(ui->editServerUrl->text());
    if (!url.isValid() || url.scheme().isEmpty() || url.host().isEmpty()) {
        QMessageBox::warning(this, tr("Test connection"), tr("Enter a valid server URL including http:// or https://."));
        return;
    }

    QNetworkRequest request(url);
    const QString token = ui->editApiToken->text().trimmed();
    if (!token.isEmpty())
        request.setRawHeader("Authorization", "Token " + token.toUtf8());

    ui->buttonTestConnection->setEnabled(false);
    m_testReply = m_networkAccessManager->get(request);
    connect(m_testReply, &QNetworkReply::finished, this, [this]() {
        QNetworkReply *reply = m_testReply;
        m_testReply = nullptr;
        ui->buttonTestConnection->setEnabled(true);

        const int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (reply->error() == QNetworkReply::NoError && statusCode >= 200 && statusCode < 300) {
            QMessageBox::information(this, tr("Test connection"), tr("Connection successful."));
        } else {
            const QString details = reply->errorString();
            QMessageBox::critical(this, tr("Test connection"),
                                  tr("Connection failed%1: %2")
                                      .arg(statusCode ? tr(" (HTTP %1)").arg(statusCode) : QString(), details));
        }
        reply->deleteLater();
    });
}

QString InventreeSettingsDialog::serverUrl() const
{
    return ui->editServerUrl->text().trimmed();
}

QString InventreeSettingsDialog::apiToken() const
{
    return ui->editApiToken->text().trimmed();
}
