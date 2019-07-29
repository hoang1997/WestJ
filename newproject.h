#ifndef NEWPROJECT_H
#define NEWPROJECT_H

#include <QDialog>
#include "fma.h"
#include <iostream>
#include <ctime>
#include <QMessageBox>
struct AEB {
    QVector<FMA*> fmaArr;
    QString evaluatorName;
    QString AEB_Id;
    QString generationTime;
};

struct COM {
    QVector<int> IP_A_BASIC;
    QVector<int> IP_B_BASIC;
    QVector<int> defaultGateway;
};

namespace Ui {
class NewProject;
}

class NewProject : public QDialog
{
    Q_OBJECT

public:
    explicit NewProject(QWidget *parent = nullptr);
    ~NewProject();

    QString returnGenTime();
    QVector<AEB*> returnAEBarr();
    QVector<COM*> returnCOMarr();
    QString returnProjectNumber();
    QString timeCheck(QString time);

    void displayError(QString errorMessage);
public slots:
    void fmaSignalReceived(FMA*);

private slots:
    void on_pushButton_4_clicked();

    void on_pushButton_clicked();

    void updateFMApreview(FMA*);

    void updatePPpreview(AEB*);
    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::NewProject *ui;
    FMA *fma;
    AEB *aeb;
    QVector<AEB*> aebArr;
    QVector<COM*> comArr;
    QString projectNumber;

    int fmaCounter = 1;
};



#endif // NEWPROJECT_H
