#pragma once

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#include "gen_src/client/ActionApi.h"
#include "gen_src/client/PartApi.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButtonConnect_clicked();

    void on_pushButtonImportParts_clicked();

private:
    Ui::MainWindow *ui;
    InvenTree::ActionApi *m_apiInstance = nullptr;
    InvenTree::PartApi *m_partApi = nullptr;
};
