#ifndef VERIFY_H
#define VERIFY_H
#include <iostream>
#include <QVector>

class Verify
{
private:
    QVector<QString> aebTemplate;
    QVector<QString> comTemplate;

    QVector<QString> setFile(QString);
public:
    Verify();

    QVector<bool> verifyAEB(QVector<QString> aebFile);
    QVector<bool> verifyCOM(QVector<QString> comFile);
    QVector<QString> returnTemplate_AEB();
     QVector<QString> returnTemplate_COM();
};

#endif // VERIFY_H
