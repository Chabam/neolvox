#ifndef CT_LASPOINTINFO_H
#define CT_LASPOINTINFO_H

#include "ctliblas/tools/las/ct_lasdefine.h"

#include "ct_itemdrawable/abstract/ct_abstractpointattributesscalar.h"
#include "ct_itemdrawable/ct_pointsattributescolor.h"

#include <QPair>

/**
 * @brief Class tool to backup attributes for a point
 */
class CTLIBLAS_EXPORT CT_LasPointInfo
{
public:
    CT_LasPointInfo();

    /**
     * @brief Set the attribute of type "type" for this point. If already exist it will be replaced.
     * @param type : the type represented by "scalar"
     * @param scalar : attribute for the type "type"
     * @param index : the index in "scalar" to get the information for this point
     * @return False if the type is not known by this class
     */
    bool setAttribute(CT_LasDefine::LASPointAttributesType type, const CT_AbstractPointAttributesScalar *scalar, const size_t &index);
    void setColorAttribute(const CT_PointsAttributesColor *color, const size_t &index);


private:
    // all attributes (in the same order that in CT_LasDefine::LASPointAttributesType)
    QPair<const CT_AbstractPointAttributesScalar*, size_t>  m_i;
    QPair<const CT_AbstractPointAttributesScalar*, size_t>  m_rn;
    QPair<const CT_AbstractPointAttributesScalar*, size_t>  m_nor;
    QPair<const CT_AbstractPointAttributesScalar*, size_t>  m_cf;
    QPair<const CT_AbstractPointAttributesScalar*, size_t>  m_sc;
    QPair<const CT_AbstractPointAttributesScalar*, size_t>  m_sdf;
    QPair<const CT_AbstractPointAttributesScalar*, size_t>  m_efl;
    QPair<const CT_AbstractPointAttributesScalar*, size_t>  m_c;
    QPair<const CT_AbstractPointAttributesScalar*, size_t>  m_sar;
    QPair<const CT_AbstractPointAttributesScalar*, size_t>  m_ud;
    QPair<const CT_AbstractPointAttributesScalar*, size_t>  m_psID;
    QPair<const CT_AbstractPointAttributesScalar*, size_t>  m_gpsT;
    QPair<const CT_AbstractPointAttributesScalar*, size_t>  m_red;
    QPair<const CT_AbstractPointAttributesScalar*, size_t>  m_green;
    QPair<const CT_AbstractPointAttributesScalar*, size_t>  m_blue;
    QPair<const CT_PointsAttributesColor*, size_t>          m_color;
    QPair<const CT_AbstractPointAttributesScalar*, size_t>  m_nir;
    QPair<const CT_AbstractPointAttributesScalar*, size_t>  m_wpdi;
    QPair<const CT_AbstractPointAttributesScalar*, size_t>  m_bowd;
    QPair<const CT_AbstractPointAttributesScalar*, size_t>  m_wpsb;
    QPair<const CT_AbstractPointAttributesScalar*, size_t>  m_rpwl;
};

#endif // CT_LASPOINTINFO_H
