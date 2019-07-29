#ifndef PROCESSFILES_H
#define PROCESSFILES_H
#include <iostream>
#include "newproject.h"
#include <QFile>
#include <QTextStream>
#include <vector>
#include <QDebug>
#include <QFileDialog>
#include <string>
class ProcessFiles : public QWidget
{
private:
    QVector<AEB*> aebArr;
    QVector<COM*> comArr;
    QVector<QString> orgAEB_File;
    QVector<QString> orgCOM_File;
    QVector<QString> editedAEB_File;
    QVector<QString> editedCOM_File;

    QString projectNumber;

    QVector<QString> setOriginalFile(QString fileName);
    QVector<QString> returnEdited_AEB(AEB*, QString projectNum);
    QVector<QString> returnEdited_COM(COM*);
    QString returnStamp();
    QString timeCheck(QString);

public:
    ProcessFiles(NewProject *np);

    void writeFiles(QString projNum, QString dir);

};

#endif // PROCESSFILES_H