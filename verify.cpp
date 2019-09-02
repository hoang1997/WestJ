#include "verify.h"
#include <QFile>
#include <QIODevice>
#include <QTextStream>
#include <QDebug>

Verify::Verify()
{
    aebTemplate = setFile(":/NR_WNC_R2_AEB_FMA1.ADC");
    comTemplate = setFile(":/NR_WNC_R2_COM_standalone.ADC");
}

QVector<QString> Verify::setFile(QString filePath)
{
    QVector<QString> outFile;

     QFile file(filePath);

     if(!file.exists()){
         qDebug() << "No file exists";
     }
     else {
         QString line;
         if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
             QTextStream stream(&file);
             while (!stream.atEnd()){
                 line = stream.readLine();
                 outFile.push_back(line);
             }
         }
         file.close();
     }
     return outFile;
}

QVector<bool> Verify::verifyAEB(QVector<QString> aebFile)
{
    QVector<bool> analysisVector;

    for(int i = 0; i < aebFile.size(); i++) {
        for(int j = 0; j < aebTemplate.size(); j++) {
            if(aebFile[i] == aebTemplate[j]) {
                analysisVector.push_back(true);
            } else {
                analysisVector.push_back(false);
            }
        }
        if(i > aebTemplate.size()) {
            analysisVector.push_back(false);
        }
    }

    return analysisVector;
}

QVector<bool> Verify::verifyCOM(QVector<QString> comFile)
{
    QVector<bool> analysisVector;

    for(int i = 0; i < comFile.size(); i++) {
        for(int j = 0; j < comTemplate.size(); j++) {
            if(comFile[i] == comTemplate[j]) {
                analysisVector.push_back(true);
            } else {
                analysisVector.push_back(false);
            }
        }
        if(i > comTemplate.size()) {
            analysisVector.push_back(false);
        }
    }

    return analysisVector;
}

QVector<QString> Verify::returnTemplate_AEB()
{
    return aebTemplate;
}

QVector<QString> Verify::returnTemplate_COM()
{
    return comTemplate;
}
