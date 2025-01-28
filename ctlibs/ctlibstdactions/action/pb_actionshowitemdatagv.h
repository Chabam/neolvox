#ifndef PB_ACTIONSHOWITEMDATAGV_H
#define PB_ACTIONSHOWITEMDATAGV_H

#include "ctlibstdactions/action/ct_actionselectitemdrawablegv.h"
#include "ct_itemdrawable/abstract/ct_abstractsingularitemdrawable.h"

class CTLIBSTDACTIONS_EXPORT PB_ActionShowItemDataGV : public CT_ActionSelectItemDrawableGV
{
    Q_OBJECT
    using SuperClass = CT_ActionSelectItemDrawableGV;

public:
    PB_ActionShowItemDataGV();

    QString title() const override;
    QString description() const override;
    QIcon icon() const override;
    QString type() const override;

    bool mouseReleaseEvent(QMouseEvent *e) override;
    void drawOverlay(GraphicsViewInterface &view, QPainter &painter) override;

    CT_ACTION_DECL_COPY(PB_ActionShowItemDataGV)

private:
    QPoint                              m_mousePoint;
    CT_AbstractSingularItemDrawable*    _previousSelectedItem;
};

#endif // PB_ACTIONSHOWITEMDATAGV_H
