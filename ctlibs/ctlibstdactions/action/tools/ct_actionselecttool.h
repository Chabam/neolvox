#ifndef CT_ACTIONSELECTTOOL_H
#define CT_ACTIONSELECTTOOL_H

#include <QRect>

#include "graphicsviewinterface.h"

#include "ctlibstdactions/ctlibstdactions_global.h"
#include "ctlibstdactions/action/tools/polygonforpicking.h"
#include "ctlibstdactions/action/tools/rectangleforpicking.h"

class CT_ActionSelectItemDrawableGVOptions;

class CTLIBSTDACTIONS_EXPORT CT_ActionSelectTool : public QObject
{
    Q_OBJECT
public:

    enum SelectionTool {
        Point = 0,
        Rectangle,
        Polygon
    };

    CT_ActionSelectTool();
    ~CT_ActionSelectTool();

    void init(GraphicsViewInterface* view, CT_ActionSelectItemDrawableGVOptions* option);

    bool mousePressEvent(QMouseEvent *e);
    bool mouseMoveEvent(QMouseEvent *e);
    bool mouseReleaseEvent(QMouseEvent *e);
    bool wheelEvent(QWheelEvent *e);

    bool keyPressEvent(QKeyEvent *e);
    bool keyReleaseEvent(QKeyEvent *e);

    void drawOverlay(GraphicsViewInterface &view, QPainter &painter);

    /**
     * @brief change the selection mode for this action. The mode will be set to view
     *        when the mousePressEvent() method is called
     * @return false if we are between mousePressEvent() and mouseReleaseEvent()
     */
    bool setSelectionMode(GraphicsViewInterface::SelectionMode mode);

    /**
     * @brief Set the selection tool to used
     */
    bool setSelectionTool(CT_ActionSelectTool::SelectionTool tool);

    /**
     * @brief Toggle the current selection
     */
    void toggleSelection();

    /**
     * @brief Returns the current selection mode used
     */
    GraphicsViewInterface::SelectionMode selectionMode() const;

    /**
     * @brief Return the current selection mode but only between [NONE;REMOVE_ONE]
     */
    GraphicsViewInterface::SelectionMode selectionModeToBasic(GraphicsViewInterface::SelectionMode mode) const;

    /**
     * @brief Returns the current selection tool used
     */
    CT_ActionSelectTool::SelectionTool selectionTool() const;

    /**
     * @brief Returns true if the user has begin to paint a rectangle or a polygon, etc....
     */
    bool hasUserBeginAnAction() const;

private:
    GraphicsViewInterface*                          m_view;
    CT_ActionSelectItemDrawableGVOptions*           m_option;


    AMKgl::RectangleForPicking                      m_rectangleTools;
    AMKgl::PolygonForPicking                        m_polygonTools;
    int                                             m_status;
    GraphicsViewInterface::SelectionMode            m_selectionMode;
    CT_ActionSelectTool::SelectionTool              m_selectionTool;
    QPoint                                          m_lastMousePos;
    Qt::KeyboardModifiers                           m_keyModifiers;

    void setKeyModifiers(Qt::KeyboardModifiers m);
    void pick();

signals:
    /**
     * @brief Emitted when the selection mode changed
     */
    void selectionModeChanged(GraphicsViewInterface::SelectionMode mode);
    void needRedrawOverlay();
    void pickingFinished();

private slots:
    void redrawOverlay();

};

#endif // CT_ACTIONSELECTTOOL_H
