#ifndef CT_VALUEANDBOOL_H
#define CT_VALUEANDBOOL_H

#include "ctlibmetrics/ctlibmetrics_global.h"

class CTLIBMETRICS_EXPORT CT_AbstractVaB
{
public:
    CT_AbstractVaB() : used(true) {}
    CT_AbstractVaB(const CT_AbstractVaB &other) {
        used = other.used;
        // value not copied !
    }

    virtual ~CT_AbstractVaB() {}

    bool used;

    virtual void* valueToVoidPtr() const = 0;
};

template<typename VType>
class VaB : public CT_AbstractVaB
{
public:
    VaB() : CT_AbstractVaB() {}

    VaB(const VaB<VType> &other) : CT_AbstractVaB(other) {}

    VaB<VType>& operator=(const VaB<VType> &other)
    {
        used = other.used;
        // value not copied !

        return *this;
    }

    void* valueToVoidPtr() const { return (void*)&value; }

    VType value;
};

#endif // CT_VALUEANDBOOL_H
