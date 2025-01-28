#ifndef CT_ABSTRACTINHANDLEWITHMODELT_H
#define CT_ABSTRACTINHANDLEWITHMODELT_H

#include "ct_model/handle/ct_handlewithmodelt.h"

template<class ModelT>
class CT_AbstractInHandleWithModelT : public CT_HandleWithModelT<ModelT> {
public:
    /**
     * @brief Returns the minimum model to found
     */
    virtual int minimum() const = 0;

    /**
     * @brief Returns the maximum model to found. -1 if no limit. 0 is invalid !
     */
    virtual int maximum() const = 0;
};
#endif // CT_ABSTRACTINHANDLEWITHMODELT_H
