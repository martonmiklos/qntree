#pragma once

#include "qwizard.h"
#include <QString>

class SummarizableWizardPage : public QWizardPage
{
public:
    SummarizableWizardPage(QWidget *parent = nullptr) = default;

    virtual QString summary() const = 0;
};
