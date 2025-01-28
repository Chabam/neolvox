#ifndef CT_TOPFNODEGROUP_H
#define CT_TOPFNODEGROUP_H

#include "ct_itemdrawable/ct_tnodegroup.h"

#include <QMatrix4x4>

template<class OPFGroup>
class CT_OutOPFNodeGroupModel;

/**
 * @brief A class that you can use with a CT_TTreeGroup to create a topology.
 *
 * This class has other method to save elements from a opf file
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_TOPFNodeGroup : public CT_TNodeGroup
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_TOPFNodeGroup, CT_TNodeGroup, OPF node group)

    using SuperClass = CT_TNodeGroup;

public:
    CT_TOPFNodeGroup();

    /**
     * @brief Copy constructor.
     *
     *        What is copied :
     *          - Pointer of the result and model of the original item.
     *          - ID
     *          - Pointer of base and alternative draw manager
     *          - Pointer of context
     *          - Removed later flags
     *
     *        What is initialized differently :
     *          - Parent is set to nullptr
     *          - isSelected and isDisplayed is set to false
     *          - Document list is not copied
     *          - Parent container is set to nullptr
     *          - Childrens (items, sucessor, components, branches) was not copied (you must copy the CT_TTreeGroup that will set the structure)
     */
    CT_TOPFNodeGroup(const CT_TOPFNodeGroup& other) = default;

    /**
     * @brief Set ID readed in OPF File
     */
    void setOPFID(const size_t &id);

    /**
     * @brief Set the matrix readed in OPF File
     */
    void setOPFMatrix(const QMatrix4x4 &matrix);

    /**
     * @brief Returns the matrix readed in OPF File
     */
    QMatrix4x4 opfMatrix() const;

    /**
     * @brief Returns the specific model for CT_TOPFNodeGroup
     */
    CT_OutOPFNodeGroupModel<CT_TOPFNodeGroup>* opfModel() const;

    CT_ITEM_COPY_IMP(CT_TOPFNodeGroup)

private:
    QMatrix4x4  m_opfMatrix;
};

#endif // CT_TOPFNODEGROUP_H
