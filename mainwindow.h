#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "newproject.h"
#include "verify.h"
#include <QFile>
#include <QDialog>
#include <QFileDialog>
#include <QDebug>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarSeries>
#include <QtCharts/QChart>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChartView>
#include <QPlainTextEdit>
using namespace QtCharts;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:

private slots:

    void on_actionOpen_File_triggered();

    void on_actionSave_File_triggered();

    void on_actionCreate_New_Project_Files_triggered();

    void on_actionOpen_Project_triggered();

    void onProjectTreeWidgetClicked(QTreeWidgetItem*, int);

    void onProjectTreeWidgetClicked_Verify(QTreeWidgetItem*, int);

    void openFile(QString, QString);

    void on_actionSave_As_triggered();

    void on_pushButton_clicked();

    void on_actionClose_Project_triggered();

    void on_actionClose_File_triggered();

    void on_actionVerify_AEB_File_triggered();

    void on_actionVerify_COM_FILE_triggered();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    QMap<QString, QString> dirMap;
    NewProject *np;
    QAction *action;
    QString currentFileDirectory;
    QVector<QString> currentfile;
    Verify *v = new Verify();
};

#endif // MAINWINDOW_H
