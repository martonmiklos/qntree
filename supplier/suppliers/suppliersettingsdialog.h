#pragma once


#include <QDialog>
#include <QSettings>

class SupplierSettingsDialog : public QDialog
{
    Q_OBJECT
public:
    SupplierSettingsDialog(QWidget *parent = nullptr) : QDialog(parent) {}
    virtual void loadSettings() = 0;

protected slots:
    virtual void saveSettings() = 0;

protected:
    QSettings m_settings;
};
