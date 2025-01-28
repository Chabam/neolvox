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

#ifndef CT_ABSTRACTSHAPE_H
#define CT_ABSTRACTSHAPE_H

#include "ct_abstractitemdrawablewithoutpointcloud.h"
#include "ct_shapedata/ct_shapedata.h"

/**
  * Reprsente une forme gomtrique 2D ou 3D
  */
class CTLIBSTRUCTUREADDON_EXPORT CT_AbstractShape : public CT_AbstractItemDrawableWithoutPointCloud
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_AbstractShape, CT_AbstractItemDrawableWithoutPointCloud, 3D shape)

    using SuperClass = CT_AbstractItemDrawableWithoutPointCloud;

public:
    CT_AbstractShape();

    /**
     * @brief Construct with a data. Cannot be nullptr !
     */
    CT_AbstractShape(CT_ShapeData* data);

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
     *          - Data
     *
     *        What is initialized differently :
     *          - Parent is set to nullptr
     *          - isSelected and isDisplayed is set to false
     *          - Document list is not copied
     */
    CT_AbstractShape(const CT_AbstractShape& other);

    void setCenterX(double x) override;
    void setCenterY(double y) override;
    void setCenterZ(double z) override;
    void setCenterCoordinate(const Eigen::Vector3d& center) override;

    double centerX() const override;
    double centerY() const override;
    double centerZ() const override;

    const CT_ShapeData* getPointerData() const;
    const CT_ShapeData& getData() const;
    const Eigen::Vector3d& getCenter() const;
    const Eigen::Vector3d& getDirection() const;
    double getDirectionX() const;
    double getDirectionY() const;
    double getDirectionZ() const;

private:
    CT_ShapeData*   _data;

protected:
    CT_ShapeData* getDataNotConst() const;

    template<typename DataT>
    DataT* dataConstCastAs() const {
        return static_cast<DataT*>(getDataNotConst());
    }
};

#endif // CT_ABSTRACTSHAPE_H
