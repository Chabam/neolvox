#ifndef CT_ACTIONSELECTITEMDRAWABLEGV_H
#define CT_ACTIONSELECTITEMDRAWABLEGV_H

#include "ctlibstdactions/ctlibstdactions_global.h"

#include "ct_actions/abstract/ct_abstractactionforgraphicsview.h"
#include "ctlibstdactions/action/tools/ct_actionselecttool.h"

class CTLIBSTDACTIONS_EXPORT CT_ActionSelectItemDrawableGV : public CT_AbstractActionForGraphicsView
{
    Q_OBJECT
public:

    enum SelectionTool {
        Point = 0,
        Rectangle,
        Polygon
    };

    CT_ActionSelectItemDrawableGV();
    ~CT_ActionSelectItemDrawableGV() override;

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

    CT_ACTION_DECL_COPY(CT_ActionSelectItemDrawableGV)

private:
    CT_ActionSelectTool*                            m_selectTool;

private slots:
    void redrawOverlay();
};

#endif // CT_ACTIONSELECTITEMDRAWABLEGV_H
