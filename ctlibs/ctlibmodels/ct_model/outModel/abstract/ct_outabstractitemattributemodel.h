#ifndef CT_OUTABSTRACTITEMATTRIBUTEMODEL_H
#define CT_OUTABSTRACTITEMATTRIBUTEMODEL_H

#include "ct_model/outModel/abstract/ct_outabstractmodel.h"

class CT_AbstractItemAttribute;

/**
 * @brief Represent an output model of type item attribute
 */
class CTLIBMODELS_EXPORT CT_OutAbstractItemAttributeModel : public CT_OutAbstractModel
{
    Q_OBJECT
    typedef CT_OutAbstractModel SuperClass;

public:
    /**
     * @brief Construct an output model that has a unique index (by default at -1 so it was invalid) and a displayable name
     * @param displayableName : a displayable name
     */
    CT_OutAbstractItemAttributeModel(const QString& displayableName);

    /**
     * @brief Copy constructor
     * @warning The parent model will not be copied !
     */
    CT_OutAbstractItemAttributeModel(const CT_OutAbstractItemAttributeModel& other);

    /**
     * @brief Returns the item attribute prototype
     */
    IItemAttributeForModel* itemAttribute() const;

    /**
     * @brief Returns the step statically cast in the class that you want
     */
    template<class T = CT_AbstractItemAttribute>
    T* itemAttributeStaticCastT() const { return static_cast<T*>(itemAttribute()); }

    /**
     * @brief Returns true if this model or if recursiveOriginalModel() is a default item attribute model (default = created by the itemdrawable statically = not added by a step)
     */
    bool isADefaultItemAttributeModel() const;

    /**
     * @brief Delete the item attribute from this model and from memory
     */
    void clearItemAttribute();

    /**
     * @brief Called from CT_DefaultItemAttributeManager to define this attribute model as default
     * @warning Not intended for direct use by plugin developper
     */
    void setAsDefaultItemAttributeModel();

private:
    bool                    m_default;
};

#endif // CT_OUTABSTRACTITEMATTRIBUTEMODEL_H
