#pragma once

#include <QWizardPage>

namespace Ui {
class WizardPageSummary;
}

class WizardPageSummary : public QWizardPage
{
    Q_OBJECT

public:
    explicit WizardPageSummary(QWidget *parent = nullptr);
    ~WizardPageSummary();

private:
    Ui::WizardPageSummary *ui;
};
