#ifndef CT_MATHSTATISTICS_H
#define CT_MATHSTATISTICS_H

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <math.h>
#include <algorithm>

#include <QList>

#include "ctlibmath_global.h"

class CTLIBMATH_EXPORT CT_MathStatistics
{
public:
    static double computeQuantile(QList<double> &values, double quantile, bool sort = false);
};

#endif // CT_MATHSTATISTICS_H
