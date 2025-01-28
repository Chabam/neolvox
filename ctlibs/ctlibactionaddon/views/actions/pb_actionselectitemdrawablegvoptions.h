#ifndef PB_ACTIONSELECTITEMDRAWABLEGVOPTIONS_H
#define PB_ACTIONSELECTITEMDRAWABLEGVOPTIONS_H

#include "ct_actions/view/abstract/ct_gabstractactionoptions.h"
#include "actions/pb_actionselectitemdrawablegv.h"

namespace Ui {
class PB_ActionSelectItemDrawableGVOptions;
}

class PB_ActionSelectItemDrawableGVOptions : public CT_GAbstractActionOptions
{
    Q_OBJECT

public:
    explicit PB_ActionSelectItemDrawableGVOptions(const PB_ActionSelectItemDrawableGV *action);
    ~PB_ActionSelectItemDrawableGVOptions();

    GraphicsViewInterface::SelectionMode selectionMode() const;
    PB_ActionSelectItemDrawableGV::SelectionTool selectionTool() const;

//    PB_ActionSelectItemDrawableGV::SelectionDrawMode drawMode() const;

private:
    Ui::PB_ActionSelectItemDrawableGVOptions *ui;

private slots:
    void on_buttonGroupSelection_buttonReleased(int id);
    void on_buttonGroupType_buttonReleased(int id);
    void on_buttonGroupMode_buttonReleased(int id);
    void on_toolButtonToggleSelection_clicked();

public slots:
    void setSelectionMode(GraphicsViewInterface::SelectionMode mode);
    void setSelectionTool(PB_ActionSelectItemDrawableGV::SelectionTool tool);
};

#endif // PB_ACTIONSELECTITEMDRAWABLEGVOPTIONS_H
