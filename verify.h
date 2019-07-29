#ifndef VERIFY_H
#define VERIFY_H
#include <iostream>
#include <QVector>

class Verify
{
private:

public:
    Verify();

    void verifyAEB(QVector<QString> aebFile);
    void verifyCOM();
};

#endif // VERIFY_H
