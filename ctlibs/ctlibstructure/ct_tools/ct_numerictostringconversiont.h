#ifndef CT_NUMERICTOSTRINGCONVERSIONT_H
#define CT_NUMERICTOSTRINGCONVERSIONT_H

#include <QLocale>
#include <sstream>
#include <iomanip>

template<typename T>
class CT_NumericToStringConversionT
{
public:
    static QString toString(const T &i, char f ='f', int prec = 3)
    {
        QLocale locale = QLocale::system();
        std::stringstream ss;

        ss << std::setprecision(prec);

        if(f == 'f')
            ss << std::fixed;
        else
            ss << std::scientific;

        ss << i;

        return QString::fromStdString(ss.str()).replace('.', locale.decimalPoint());
    }
};

template<>
inline QString CT_NumericToStringConversionT<bool>::toString(const bool &i, char f, int prec)
{
    Q_UNUSED(f)
    Q_UNUSED(prec)

    return i ? "true" : "false";
}

template<>
inline QString CT_NumericToStringConversionT<float>::toString(const float &i, char f, int prec)
{
    Q_UNUSED(f)
    Q_UNUSED(prec)

    return QString::number(i, 'g', 10);
}

template<>
inline QString CT_NumericToStringConversionT<double>::toString(const double &i, char f, int prec)
{
    Q_UNUSED(f)
    Q_UNUSED(prec)

    return QString::number(i, 'g', 20);
}

template<>
inline QString CT_NumericToStringConversionT<long double>::toString(const long double &i, char f, int prec)
{
    Q_UNUSED(f)
    Q_UNUSED(prec)

    return QString::number(i, 'g', 40);
}

#endif // CT_NUMERICTOSTRINGCONVERSIONT_H
