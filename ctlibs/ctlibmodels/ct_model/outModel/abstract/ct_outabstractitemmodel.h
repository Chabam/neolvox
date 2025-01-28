#ifndef CT_OUTABSTRACTITEMMODEL_H
#define CT_OUTABSTRACTITEMMODEL_H

#include "ct_model/outModel/abstract/ct_outabstractmodel.h"

class CT_AbstractItemDrawable;

/**
 * @brief Represent an output model for a IItemDrawableForModel (singular or group)
 */
class CTLIBMODELS_EXPORT CT_OutAbstractItemModel : public CT_OutAbstractModel
{
    Q_OBJECT
    typedef CT_OutAbstractModel SuperClass;

public:
    /**
     * @brief Construct an output item model
     * @param displayableName : a displayable name
     */
    CT_OutAbstractItemModel(const QString& displayableName);

    /**
     * @brief Copy constructor, the item prototype will be copied
     * @warning The parent model will not be copied !
     */
    CT_OutAbstractItemModel(const CT_OutAbstractItemModel& other);

    /**
     * @brief Returns the real item of this model
     */
    IItemDrawableForModel* itemDrawable() const;

    /**
     * @brief Returns the step statically cast in the class that you want
     */
    template<class T = CT_AbstractItemDrawable>
    T* itemDrawableStaticCastT() const { return static_cast<T*>(itemDrawable()); }
};

#endif // CT_OUTABSTRACTITEMMODEL_H
