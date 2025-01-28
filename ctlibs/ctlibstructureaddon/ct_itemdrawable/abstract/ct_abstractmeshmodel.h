#ifndef CT_ABSTRACTMESHMODEL_H
#define CT_ABSTRACTMESHMODEL_H

#include "ctlibstructureaddon_global.h"

#include "ct_itemdrawable/abstract/ct_abstractgeometricalitem.h"

#include "ct_accessibility/ct_iaccesspointcloud.h"
#include "ct_accessibility/ct_iaccessfacecloud.h"
#include "ct_accessibility/ct_iaccessedgecloud.h"

#include "ct_handle/ct_handleoutedgecolor.h"
#include "ct_handle/ct_handleoutedgenormal.h"
#include "ct_handle/ct_handleoutedgescalar.h"

#include "ct_handle/ct_handleoutfacecolor.h"
#include "ct_handle/ct_handleoutfacenormal.h"
#include "ct_handle/ct_handleoutfacescalar.h"

#include "ct_handle/ct_handleinedgecolor.h"
#include "ct_handle/ct_handleinedgenormal.h"
#include "ct_handle/ct_handleinedgescalar.h"

#include "ct_handle/ct_handleinfacecolor.h"
#include "ct_handle/ct_handleinfacenormal.h"
#include "ct_handle/ct_handleinfacescalar.h"

/**
 * @brief A singular item that can manage a mesh
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_AbstractMeshModel : public CT_AbstractGeometricalItem, public CT_IAccessPointCloud, public CT_IAccessFaceCloud, public CT_IAccessEdgeCloud
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_AbstractMeshModel, CT_AbstractSingularItemDrawable, Mesh model)
    using SuperClass = CT_AbstractGeometricalItem;

public:
    CT_AbstractMeshModel();

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
     *
     *        What is initialized differently :
     *          - Parent is set to nullptr
     *          - isSelected and isDisplayed is set to false
     *          - Document list is not copied
     */
    CT_AbstractMeshModel(const CT_AbstractMeshModel& other) = default;

    /**
     * @brief Draw only faces
     */
    virtual void drawFaces(GraphicsViewInterface &view, PainterInterface &painter) = 0;

    /**
     * @brief Begin draw multiple faces
     */
    virtual void beginDrawMultipleFace(GraphicsViewInterface &view, PainterInterface &painter) = 0;

    /**
     * @brief Draw only face at index 'index'
     */
    virtual void drawFaceAt(const size_t &index, GraphicsViewInterface &view, PainterInterface &painter) = 0;

    /**
     * @brief Finish draw multiple faces
     */
    virtual void endDrawMultipleFace(GraphicsViewInterface &view, PainterInterface &painter) = 0;

    /**
     * @brief Draw only edges
     */
    virtual void drawEdges(GraphicsViewInterface &view, PainterInterface &painter) = 0;

    /**
     * @brief Begin draw multiple edges
     */
    virtual void beginDrawMultipleEdge(GraphicsViewInterface &view, PainterInterface &painter) = 0;

    /**
     * @brief Draw only edge at index 'index'
     */
    virtual void drawEdgeAt(const size_t &index, GraphicsViewInterface &view, PainterInterface &painter) = 0;

    /**
     * @brief Finish draw multiple edges
     */
    virtual void endDrawMultipleEdge(GraphicsViewInterface &view, PainterInterface &painter) = 0;
};

#endif // CT_ABSTRACTMESHMODEL_H
