#ifndef CT_INHANDLEMINMAXWITHMODELT_H
#define CT_INHANDLEMINMAXWITHMODELT_H

#include <QtGlobal>

#include "ct_model/inModel/handle/abstract/ct_abstractinhandlewithmodelt.h"

#include <QDebug>

template<class ModelType, int min = 1, int max = 1>
class CT_InHandleMinMaxWithModelT : public CT_AbstractInHandleWithModelT<ModelType> {

    using SuperClass = CT_AbstractInHandleWithModelT<ModelType>;

public:
    enum {
        MinValue = min,
        MaxValue = max
    };

    CT_InHandleMinMaxWithModelT() : SuperClass() {
        constexpr bool testMinAndMaxValid = (min <= max) || (max == -1);
        static_assert(testMinAndMaxValid, "Value of min and max was invalid ! Max must be greather than min or -1 for your handle.");

        static_assert(!testMinAndMaxValid || (min != max) || (min != 0), "Value of max is invalid ! Must be -1 or greather than 0 for your handle.");
    }

    /**
     * @brief Returns the minimum model to found
     */
    int minimum() const final { return MinValue; }

    /**
     * @brief Returns the maximum model to found. -1 if no limit. 0 is invalid !
     */
    int maximum() const final { return MaxValue; }
};

#endif // CT_INHANDLEMINMAXWITHMODELT_H
