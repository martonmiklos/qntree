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
#include <QUuid>

#include "inventreeprofilemanager.h"

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
    m_settings.beginGroup("InvenTree");

    QStringList urls = m_settings.value(KEY_URL_LIST).toStringList();
    m_urlModel->setStringList(urls);

    urlCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    urlCompleter->setCompletionMode(QCompleter::PopupCompletion);

    ui->editServerUrl->setCompleter(urlCompleter);
    m_settings.endGroup();
}

void InventreeSettingsDialog::loadSettings()
{
    m_settings.beginGroup("InvenTreeProfiles");
    m_settings.beginGroup(InvenTreeProfileManager::instance()->currentInstanceUuid());

    ui->editServerUrl->setText(m_settings.value(KEY_SERVER).toString());
    ui->editApiToken->setText(m_settings.value(KEY_TOKEN).toString());

    m_settings.endGroup();
    m_settings.endGroup();
}

void InventreeSettingsDialog::saveSettings()
{
    m_settings.beginGroup("InvenTreeProfiles");
    m_settings.beginGroup(InvenTreeProfileManager::instance()->currentInstanceUuid());

    QString url = ui->editServerUrl->text().trimmed();
    QString token = ui->editApiToken->text().trimmed();

    m_settings.setValue(KEY_SERVER, url);
    m_settings.setValue(KEY_TOKEN, token);
    m_settings.setValue("name", ui->editProfileName->text());

    m_settings.endGroup();
    m_settings.endGroup();

    m_settings.beginGroup("InvenTree");
    QStringList urls = m_settings.value(KEY_URL_LIST).toStringList();
    if (!url.isEmpty() && !urls.contains(url)) {
        urls.prepend(url);
        m_settings.setValue(KEY_URL_LIST, urls);
        m_urlModel->setStringList(urls);
    }
    accept();
    m_settings.endGroup();
}

void InventreeSettingsDialog::refreshConnectionProfiles()
{
    const QSignalBlocker blocker(ui->comboConnectionProfiles);
    ui->comboConnectionProfiles->clear();
    ui->comboConnectionProfiles->addItem(tr("Select saved connection..."));

    auto profiles = InvenTreeProfileManager::instance()->availableProfiles();
    int index = 1, selectedIndex = 0;
    for (auto &prof : profiles) {
        ui->comboConnectionProfiles->addItem(prof.second, prof.first);
        if (prof.first == InvenTreeProfileManager::currentInstanceUuid())
            selectedIndex = index;
        index++;
    }
    ui->comboConnectionProfiles->setCurrentIndex(selectedIndex);
}

void InventreeSettingsDialog::saveConnectionProfile()
{
    const QString name = ui->editProfileName->text().trimmed();
    if (name.isEmpty()) {
        QMessageBox::warning(this, tr("Save connection"), tr("Enter a name for the saved connection."));
        ui->editProfileName->setFocus();
        return;
    }

    if (name.contains('/') || name.contains('\\')) {
        QMessageBox::warning(this, tr("Save connection"), tr("The connection name cannot contain '/' or '\\'."));
        ui->editProfileName->setFocus();
        return;
    }

    if (ui->editApiToken->text().isEmpty()) {
        QMessageBox::warning(this, tr("Save connection"), tr("The token cannot left blank"));
        ui->editApiToken->setFocus();
        return;
    }

    if (ui->editServerUrl->text().isEmpty()) {
        QMessageBox::warning(this, tr("Save connection"), tr("The server url cannot left blank"));
        ui->editServerUrl->setFocus();
        return;
    }

    m_settings.beginGroup(QStringLiteral("InvenTreeProfiles"));
    auto profiles = m_settings.childGroups();
    bool found = false;
    for (auto &prof : profiles) {
        m_settings.beginGroup(prof);
        if (m_settings.value("name") == name) {
            m_settings.setValue(KEY_SERVER, ui->editServerUrl->text().trimmed());
            m_settings.setValue(KEY_TOKEN, ui->editApiToken->text().trimmed());
            InvenTreeProfileManager::setCurrentInstanceUuid(prof);
            found = true;
        }
        m_settings.endGroup();
        if (found)
            break;
    }

    if (!found) {
        auto newUuid = QUuid::createUuid().toString(QUuid::WithoutBraces);
        m_settings.beginGroup(newUuid);
        m_settings.setValue(KEY_SERVER, ui->editServerUrl->text().trimmed());
        m_settings.setValue(KEY_TOKEN, ui->editApiToken->text().trimmed());
        m_settings.setValue("name", ui->editProfileName->text());
        InvenTreeProfileManager::setCurrentInstanceUuid(newUuid);
        m_settings.endGroup();
    }

    m_settings.endGroup(); // InvenTreeProfiles

    refreshConnectionProfiles();
}

void InventreeSettingsDialog::loadConnectionProfile(const QString &profileName)
{
    if (profileName.isEmpty() || ui->comboConnectionProfiles->currentIndex() == 0) {
        ui->buttonDeleteProfile->setEnabled(false);
        return;
    }

    InvenTreeProfileManager::setCurrentInstanceUuid(ui->comboConnectionProfiles->currentData().toString());

    m_settings.beginGroup(QStringLiteral("InvenTreeProfiles"));
    m_settings.beginGroup(ui->comboConnectionProfiles->currentData().toString());
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

    m_settings.beginGroup(QStringLiteral("InvenTreeProfiles"));
    m_settings.remove(ui->comboConnectionProfiles->currentData().toString());
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

void InventreeSettingsDialog::on_InventreeSettingsDialog_accepted()
{
    InvenTreeProfileManager::instance()->setCurrentInstanceUuid(ui->comboConnectionProfiles->currentData().toString());
}

