#ifndef CT_ACTIONSELECTITEMDRAWABLEGVOPTIONS_H
#define CT_ACTIONSELECTITEMDRAWABLEGVOPTIONS_H

#include "ct_actions/view/abstract/ct_gabstractactionoptions.h"

#include "ctlibstdactions/ctlibstdactions_global.h"
#include "ctlibstdactions/action/ct_actionselectitemdrawablegv.h"
#include "ctlibstdactions/action/tools/ct_actionselecttool.h"

namespace Ui {
class CT_ActionSelectItemDrawableGVOptions;
}

class CTLIBSTDACTIONS_EXPORT CT_ActionSelectItemDrawableGVOptions : public CT_GAbstractActionOptions
{
    Q_OBJECT

public:
    explicit CT_ActionSelectItemDrawableGVOptions(const CT_AbstractAction *action, CT_ActionSelectTool* selectTool);
    ~CT_ActionSelectItemDrawableGVOptions();

    void setAvailableModes(bool item, bool points, bool faces, bool edges);
    void setAvailableSelectionModes(bool one, bool rect, bool poly, bool replace, bool add, bool remove);

    GraphicsViewInterface::SelectionMode selectionMode() const;
    CT_ActionSelectTool::SelectionTool selectionTool() const;

private:
    Ui::CT_ActionSelectItemDrawableGVOptions *  ui;
    CT_ActionSelectTool*                        m_selectTool;

private slots:
    void on_buttonGroupSelection_buttonReleased(int id);
    void on_buttonGroupType_buttonReleased(int id);
    void on_buttonGroupMode_buttonReleased(int id);
    void on_toolButtonToggleSelection_clicked();

public slots:
    void setSelectionMode(GraphicsViewInterface::SelectionMode mode);
    void setSelectionTool(CT_ActionSelectItemDrawableGV::SelectionTool tool);
};

#endif // CT_ACTIONSELECTITEMDRAWABLEGVOPTIONS_H
