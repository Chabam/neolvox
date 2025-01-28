/****************************************************************************

 Copyright (C) 2010-2012 the Office National des Forêts (ONF), France
                     and the Association de Recherche Technologie et Sciences (ARTS), Ecole Nationale Suprieure d'Arts et Métiers (ENSAM), Cluny, France.
                     All rights reserved.

 Contact : alexandre.piboule@onf.fr

 Developers : Michal KREBS (ARTS/ENSAM)

 This file is part of PluginShared library 2.0.

 PluginShared is free library: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 PluginShared is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with PluginShared.  If not, see <http://www.gnu.org/licenses/lgpl.html>.

*****************************************************************************/

#ifndef CT_ABSTRACTSHAPE2D_H
#define CT_ABSTRACTSHAPE2D_H

#include "ct_abstractitemdrawablewithoutpointcloud.h"
#include "ct_shape2ddata/ct_shape2ddata.h"

/**
  * Représente une forme géomtrique 2D
  */
class CTLIBSTRUCTUREADDON_EXPORT CT_AbstractShape2D : public CT_AbstractItemDrawableWithoutPointCloud
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_AbstractShape2D, CT_AbstractItemDrawableWithoutPointCloud, 2D shape)

    using SuperClass = CT_AbstractItemDrawableWithoutPointCloud;

public:
    CT_AbstractShape2D();

    /**
     * @brief Construct with a data. Cannot be nullptr !
     */
    CT_AbstractShape2D(CT_Shape2DData* data);

    /**
     * @brief Copy constructor.
     *
     *        What is copied :
     *          - Pointer of the result and model of the original item.
     *          - Unique ID
     *          - Pointer of base and alternative draw manager
     *          - Displayable name
     *          - Center coordinates
     *          - Default Color
     *          - Min and Max coordinates (bounding box)
     *          - Data 2D
     *          - Z Value and if z value is defined
     *
     *        What is initialized differently :
     *          - Parent is set to nullptr
     *          - isSelected and isDisplayed is set to false
     *          - Document list is not copied
     */
    CT_AbstractShape2D(const CT_AbstractShape2D& other);

    ~CT_AbstractShape2D() override;

    void setCenterX(double x) override;
    void setCenterY(double y) override;
    void setCenterZ(double z) override;
    void setCenterCoordinate(const Eigen::Vector3d &center) override;

    void setZValue(double z);

    double zValue() const;

    bool isZValueDefined() const;

    const CT_Shape2DData* getPointerData() const;
    const CT_Shape2DData& getData() const;
    const Eigen::Vector2d& getCenter() const;

    void setBoundingBox(const Eigen::Vector3d& min, const Eigen::Vector3d& max) override;

    static double Z_PLANE_FOR_2D_SHAPES;

private:
    CT_Shape2DData*     _data;
    double              _zValue;
    bool                _zValueDefined;

protected:
    CT_Shape2DData* getDataNotConst() const;

    template<typename DataT>
    DataT* dataConstCastAs() const {
        return static_cast<DataT*>(getDataNotConst());
    }

    CT_DEFAULT_IA_BEGIN(CT_AbstractShape2D)
    CT_DEFAULT_IA_V2(CT_AbstractShape2D, CT_AbstractCategory::staticInitDataZ(), &CT_AbstractShape2D::zValue, QObject::tr("Z"))
    CT_DEFAULT_IA_END(CT_AbstractShape2D)
};

#endif // CT_ABSTRACTSHAPE2D_H
