#pragma once

#include <QWidget>

class InvenTreePartImportWizard;

namespace Ui {
class StockLineWidget;
}

class StockLineWidget : public QWidget
{
    Q_OBJECT
public:
    friend class InvenTreePartImportWizard;
    explicit StockLineWidget(InvenTreePartImportWizard *wizard, QWidget *parent = nullptr);
    ~StockLineWidget();
    void setRemovable(bool removable);
    int selectedLocationPk() const;
    qreal quantity() const;
    bool ready() const;
    void setQtySuffix(const QString &suffix);
    void setAsDefaultLocation(bool defaultLocation);
    bool isDefaultLocation() const;
    bool create() const;
    QString locationName() const;

private slots:
    void on_toolButtonRemoveStock_clicked();
    void on_toolButtonEditStockNote_clicked();
    void on_toolButtonChangeTargetLocation_clicked();
    void on_doubleSpinBoxStockQuantity_valueChanged(double arg1);
    void on_toolButtonNewStock_clicked();
    void on_checkBoxMakeStockLocationDefault_clicked(bool checked);
    void on_checkBoxCreateStock_clicked(bool checked);

signals:
    void removeLine();
    void addNewLine();
    void setAsDefault();
    void quantityChanged();

private:
    QString m_notes;
    Ui::StockLineWidget *ui;
    int m_selectedLocationPk = -1;
    QList<QWidget*> m_stockWidgets;
    InvenTreePartImportWizard *m_wizard;
};
