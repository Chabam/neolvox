/**
 * @author Michael Krebs (AMVALOR)
 * @date 25.01.2017
 * @version 1
 */
#ifndef CT_SHOOTINGPATTERND_H
#define CT_SHOOTINGPATTERND_H

#include "ct_itemdrawable/abstract/ct_abstractitemdrawablewithoutpointcloud.h"
#include "ct_itemdrawable/tools/scanner/ct_shootingpattern.h"

/**
 * @brief A shooting pattern that can be drawn and access with a model.
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_ShootingPatternD : public CT_AbstractItemDrawableWithoutPointCloud
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_ShootingPatternD, CT_AbstractItemDrawableWithoutPointCloud, Shooting pattern)
    using SuperClass = CT_AbstractItemDrawableWithoutPointCloud;

public:
    CT_ShootingPatternD();
    CT_ShootingPatternD(CT_ShootingPattern* pattern);
    CT_ShootingPatternD(const CT_ShootingPatternD& other);
    ~CT_ShootingPatternD();


    /**
     * @brief Returns the shooting pattern
     */
    CT_ShootingPattern* getShootingPattern() const;

    /**
     * @brief Returns the origin of all shot
     */
    const Eigen::Vector3d& getOrigin() const;

    /**
     * @brief Returns the number of shots
     */
    size_t getNumberOfShots() const;

    /**
     * @brief Returns the shot at index "index"
     */
    CT_Shot getShotAt(const size_t& index) const;

    /**
     * @brief Returns the shot at index "index"
     */
    void getShotAt(const size_t& index, CT_Shot& shot) const;

    //***********************************************************************************//
    //      Virtual/redefined methods from CT_AbstractItemDrawableWithoutPointCloud      //
    //***********************************************************************************//

    CT_ITEM_COPY_IMP(CT_ShootingPatternD)

private:
    CT_ShootingPattern* m_pattern;

    CT_DEFAULT_IA_BEGIN(CT_ShootingPatternD)
    CT_DEFAULT_IA_V2(CT_ShootingPatternD, CT_AbstractCategory::staticInitDataSize(), &CT_ShootingPatternD::getNumberOfShots, QObject::tr("Number of shots"))
    CT_DEFAULT_IA_END(CT_ShootingPatternD)
};

#endif // CT_SHOOTINGPATTERND_H
