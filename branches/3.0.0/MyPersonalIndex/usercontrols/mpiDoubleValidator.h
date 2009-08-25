#ifndef MPIDOUBLEVALIDATOR_H
#define MPIDOUBLEVALIDATOR_H

#include <QtGui>

class mpiDoubleValidator: public QDoubleValidator
{

public:

    mpiDoubleValidator(double bottom, double top, int decimals, QObject *parent = 0) : QDoubleValidator(bottom, top, decimals, parent) {}

    QValidator::State mpiDoubleValidator::validate(QString &input, int &pos) const
    {
        if (input.isEmpty())
            return QValidator::Intermediate;

        if ( QDoubleValidator::validate(input, pos) != QValidator::Acceptable
             || input == "," || input == "." )  // has to be greater than 1
            return QValidator::Invalid;

        return QValidator::Acceptable;
    }

};

#endif // MPIDOUBLEVALIDATOR_H
