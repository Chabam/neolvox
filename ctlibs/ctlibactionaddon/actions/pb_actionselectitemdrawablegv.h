#ifndef PB_ACTIONSELECTITEMDRAWABLEGV_H
#define PB_ACTIONSELECTITEMDRAWABLEGV_H

#include <QRect>

#include "ct_actions/abstract/ct_abstractactionforgraphicsview.h"
#include "actions/tools/polygonforpicking.h"
#include "actions/tools/rectangleforpicking.h"

/**
 * @brief Action to select items, points, faces, edges, etc...
 *
 * You can use this action in your action if you want. Just call methods :
 *
 * init()
 * mousePressEvent()
 * etc.....
 *
 * in methods of your action.
 *
 * Dont forget to set the selection mode => setSelectionMode method
 */
class PB_ActionSelectItemDrawableGV : public CT_AbstractActionForGraphicsView
{
    Q_OBJECT
public:

    enum SelectionTool {
        Point = 0,
        Rectangle,
        Polygon
    };

    PB_ActionSelectItemDrawableGV();

    QString title() const override;
    QString description() const override;
    QIcon icon() const override;
    QString type() const override;

    void init() override;

    bool mousePressEvent(QMouseEvent *e) override;
    bool mouseMoveEvent(QMouseEvent *e) override;
    bool mouseReleaseEvent(QMouseEvent *e) override;
    bool wheelEvent(QWheelEvent *e) override;

    bool keyPressEvent(QKeyEvent *e) override;
    bool keyReleaseEvent(QKeyEvent *e) override;

    void drawOverlay(GraphicsViewInterface &view, QPainter &painter) override;

    CT_ACTION_DECL_COPY(PB_ActionSelectItemDrawableGV)

    /**
     * @brief change the selection mode for this action. The mode will be set to view
     *        when the mousePressEvent() method is called
     * @return false if we are between mousePressEvent() and mouseReleaseEvent()
     */
    bool setSelectionMode(GraphicsViewInterface::SelectionMode mode);

    /**
     * @brief Set the selection tool to used
     */
    bool setSelectionTool(PB_ActionSelectItemDrawableGV::SelectionTool tool);

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
    PB_ActionSelectItemDrawableGV::SelectionTool selectionTool() const;

private:
    AMKgl::RectangleForPicking                      m_rectangleTools;
    AMKgl::PolygonForPicking                        m_polygonTools;
    int                                             m_status;
    GraphicsViewInterface::SelectionMode            m_selectionMode;
    PB_ActionSelectItemDrawableGV::SelectionTool    m_selectionTool;
    QPoint                                          m_lastMousePos;
    Qt::KeyboardModifiers                           m_keyModifiers;

    void setKeyModifiers(Qt::KeyboardModifiers m);
    void pick();

private slots:
    void redrawOverlay();

signals:
    /**
     * @brief Emitted when the selection mode changed
     */
    void selectionModeChanged(GraphicsViewInterface::SelectionMode mode);
};

#endif // PB_ACTIONSELECTITEMDRAWABLEGV_H
