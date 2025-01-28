#ifndef CT_ABSTRACTACTIONFORGRAPHICSVIEW_H
#define CT_ABSTRACTACTIONFORGRAPHICSVIEW_H

#include "ct_actions/abstract/ct_abstractaction.h"
#include "graphicsviewinterface.h"

class PainterInterface;

#include <QPainter>

/**
 * @brief Represent an action for a 3D graphics view
 */
class CTLIBACTION_EXPORT CT_AbstractActionForGraphicsView : public CT_AbstractAction
{
    Q_OBJECT

public:
    CT_AbstractActionForGraphicsView();

    /**
     * @brief Define the graphics view where occurs event. (A document can have multiple graphics view)
     *        Your action is copied and each copy is set to a GraphicsView
     */
    void setGraphicsView(const GraphicsViewInterface *view);

    /**
     * @brief Return the graphics view of the action
     */
    GraphicsViewInterface* graphicsView() const;

    /**
     * @brief This method is called when the action is active and when the signal "drawing3DChanged" of
     *        the action is emitted. You can overwrite them if you want to draw some ItemDrawable without
     *        add them in the document. If you want to access objects in the graphics view to modify it you
     *        must not forget to call the method "setNew3DElementDraw(X)" for each item or element you draw.
     *
     * @warning Elements drawn by an action is not exportable or selectable.
     */
    virtual void draw(GraphicsViewInterface &view, PainterInterface &painter) { Q_UNUSED(view) Q_UNUSED(painter) }

    /**
     * @brief This method is called (always after draw(...)) when the action is active and when the graphics
     *        view is updated. You can overwrite them if you want to draw some 2D element like text or polygons, etc...
     */
    virtual void drawOverlay(GraphicsViewInterface &view, QPainter &painter) {  Q_UNUSED(view) Q_UNUSED(painter) }

protected:

    /**
     * @brief Register a CT_GAbstractActionOptions to be managed automatically. The method
     *        hideOptions() and showOptions() hide and show all options registered.
     *        This method also manage the signal drawing3DChanged() emitted by the options to call
     *        automatically the method setDrawing3DChanged() of this action.
     */
    void registerOption(CT_GAbstractActionOptions *options);

    /**
     * @brief Called when the graphics view has changed. You can inherit from this method if you want
     *        to do something when the action is added/removed from a graphics view
     */
    virtual void graphicsViewChangedEvent() {}

protected slots:

    /**
     * @brief Call this method when your "draw" method must draw something else that previously
     */
    void setDrawing3DChanged();

    /**
     * @brief Call this method in your "draw" method for each element you draw. This is important
     *        if you want to visit objects of a specific element with the method "visitObjectsOfAction"
     *        from the GraphicsViewInterface.
     */
    void setNew3DElementDraw(const quint32& uniqueIndex);

private:
    GraphicsViewInterface   *m_gView;

signals:
    /**
     * @brief Emitted when the 3D drawing of the action was changed
     */
    void drawing3DChanged();

    /**
     * @brief Emitted when a new 3D element is draw by this action
     */
    void new3DElementDraw(quint32 uniqueIndex);
};

#endif // CT_ABSTRACTACTIONFORGRAPHICSVIEW_H
