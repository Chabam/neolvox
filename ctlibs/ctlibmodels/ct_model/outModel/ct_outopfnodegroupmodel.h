#ifndef CT_OUTOPFNODEGROUPMODEL_H
#define CT_OUTOPFNODEGROUPMODEL_H

#include "ct_model/outModel/ct_outstdgroupmodel.h"

/**
 * @brief Use this class if you will inform that you want create a OPF Node group and add it to an
 *        other output model that represent a group or a result
 */
template<class OPFGroup>
class CT_OutOPFNodeGroupModel : public CT_OutStdGroupModel<OPFGroup>
{
    typedef CT_OutStdGroupModel<OPFGroup> SuperClass;

public:
    /**
     * @brief Construct a model
     * @param displayableName : a displayable name (name displayed in gui for user information). Empty by default.
     * @param shortDescription : a short description (displayed in gui for user information). Empty by default.
     * @param detailledDescription : a detailled description (displayed in gui for user information). Empty by default.
     */
    CT_OutOPFNodeGroupModel(const QString& displayableName = "",
                            const QString& shortDescription = "",
                            const QString& detailledDescription = "",
                            OPFGroup* prototype = nullptr);

    /**
     * @brief Copy constructor
     * @warning The parent model will not be copied !
     */
    CT_OutOPFNodeGroupModel(const CT_OutOPFNodeGroupModel& other) = default;

    /**
     * @brief Set level (Scale) readed in OPF File
     */
    void setOPFLevel(const quint8 &level);

    /**
     * @brief Returns level (Scale)
     */
    quint8 opfLevel() const;

    /**
     * @brief Returns a copy of this model
     */
    CT_OutAbstractModel* copy() const override;

private:
    quint8  m_level;
};

#include "ct_outopfnodegroupmodel.hpp"

#endif // CT_OUTOPFNODEGROUPMODEL_H
