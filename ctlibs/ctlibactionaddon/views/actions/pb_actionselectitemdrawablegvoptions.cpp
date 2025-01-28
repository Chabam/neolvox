#include "pb_actionselectitemdrawablegvoptions.h"
#include "ui_pb_actionselectitemdrawablegvoptions.h"

PB_ActionSelectItemDrawableGVOptions::PB_ActionSelectItemDrawableGVOptions(const PB_ActionSelectItemDrawableGV *action) :
    CT_GAbstractActionOptions(action),
    ui(new Ui::PB_ActionSelectItemDrawableGVOptions)
{
    ui->setupUi(this);

    connect(action, SIGNAL(selectionModeChanged(GraphicsViewInterface::SelectionMode)), this, SLOT(setSelectionMode(GraphicsViewInterface::SelectionMode)));

//    ui->buttonGroupSelection->setId(ui->toolButtonSelectOne, GraphicsViewInterface::SELECT_ONE);
//    ui->buttonGroupSelection->setId(ui->toolButtonAddOne, GraphicsViewInterface::ADD_ONE);
//    ui->buttonGroupSelection->setId(ui->toolButtonRemoveOne, GraphicsViewInterface::REMOVE_ONE);
//    ui->buttonGroupSelection->setId(ui->toolButtonSelectMulti, GraphicsViewInterface::SELECT);
//    ui->buttonGroupSelection->setId(ui->toolButtonAddMulti, GraphicsViewInterface::ADD);
//    ui->buttonGroupSelection->setId(ui->toolButtonRemoveMulti, GraphicsViewInterface::REMOVE);

//    ui->buttonGroupOptimization->setId(ui->radioButtonSelectAsShown, PB_ActionSelectItemDrawableGV::SELECT_CURRENT_MODE);
//    ui->buttonGroupOptimization->setId(ui->radioButtonNORMAL, PB_ActionSelectItemDrawableGV::SELECT_NORMAL_MODE);
//    ui->buttonGroupOptimization->setId(ui->radioButtonFAST, PB_ActionSelectItemDrawableGV::SELECT_FAST_MODE);
}

PB_ActionSelectItemDrawableGVOptions::~PB_ActionSelectItemDrawableGVOptions()
{
    delete ui;
}

GraphicsViewInterface::SelectionMode PB_ActionSelectItemDrawableGVOptions::selectionMode() const
{   
    int mode = GraphicsViewInterface::NONE;

    if (ui->toolButtonSelectOne->isChecked())
    {
        if (ui->toolButtonReplaceMode->isChecked()) {
            mode = GraphicsViewInterface::SELECT_ONE;
        } else if (ui->toolButtonAddMode->isChecked()) {
            mode = GraphicsViewInterface::ADD_ONE;
        } else {
            mode = GraphicsViewInterface::REMOVE_ONE;
        }
    } else if (ui->toolButtonSelectMulti->isChecked() || ui->toolButtonSelectMultiByPolygon->isChecked()) {
        if (ui->toolButtonReplaceMode->isChecked()) {
            mode = GraphicsViewInterface::SELECT;
        } else if (ui->toolButtonAddMode->isChecked()) {
            mode = GraphicsViewInterface::ADD;
        } else {
            mode = GraphicsViewInterface::REMOVE;
        }
    } else {
        return (GraphicsViewInterface::SelectionMode)mode;
    }

    if(!ui->radioButtonItems->isChecked()) {
        mode--;
    }

    if(ui->radioButtonPoints->isChecked()) {
        mode += GraphicsViewInterface::SELECT_POINTS;
    }
    else if(ui->radioButtonFaces->isChecked()) {
        mode += GraphicsViewInterface::SELECT_FACES;
    }
    else if(ui->radioButtonEdges->isChecked()) {
        mode += GraphicsViewInterface::SELECT_EDGES;
    }

    return (GraphicsViewInterface::SelectionMode)mode;
}

PB_ActionSelectItemDrawableGV::SelectionTool PB_ActionSelectItemDrawableGVOptions::selectionTool() const
{
    if(ui->toolButtonSelectMulti->isChecked())
        return PB_ActionSelectItemDrawableGV::Rectangle;
    else if(ui->toolButtonSelectMultiByPolygon->isChecked())
        return PB_ActionSelectItemDrawableGV::Polygon;

    return PB_ActionSelectItemDrawableGV::Point;
}

//PB_ActionSelectItemDrawableGV::SelectionDrawMode PB_ActionSelectItemDrawableGVOptions::drawMode() const
//{
//    return (PB_ActionSelectItemDrawableGV::SelectionDrawMode)ui->buttonGroupOptimization->checkedId();
//}

void PB_ActionSelectItemDrawableGVOptions::on_buttonGroupType_buttonReleased(int id)
{
    Q_UNUSED(id)

    (dynamic_cast<PB_ActionSelectItemDrawableGV*>(action()))->setSelectionMode(selectionMode());
    (dynamic_cast<PB_ActionSelectItemDrawableGV*>(action()))->setSelectionTool(selectionTool());
}

void PB_ActionSelectItemDrawableGVOptions::on_buttonGroupMode_buttonReleased(int id)
{
    Q_UNUSED(id)

    (dynamic_cast<PB_ActionSelectItemDrawableGV*>(action()))->setSelectionMode(selectionMode());
    (dynamic_cast<PB_ActionSelectItemDrawableGV*>(action()))->setSelectionTool(selectionTool());
}

void PB_ActionSelectItemDrawableGVOptions::on_toolButtonToggleSelection_clicked()
{
    (dynamic_cast<PB_ActionSelectItemDrawableGV*>(action()))->toggleSelection();
}

//void PB_ActionSelectItemDrawableGVOptions::on_buttonGroupOptimization_buttonReleased(int id)
//{
//    Q_UNUSED(id)

//    (dynamic_cast<PB_ActionSelectItemDrawableGV*>(action()))->setDrawMode(drawMode());
//}

void PB_ActionSelectItemDrawableGVOptions::on_buttonGroupSelection_buttonReleased(int id)
{
    Q_UNUSED(id)

    (dynamic_cast<PB_ActionSelectItemDrawableGV*>(action()))->setSelectionMode(selectionMode());
    (dynamic_cast<PB_ActionSelectItemDrawableGV*>(action()))->setSelectionTool(selectionTool());
}

void PB_ActionSelectItemDrawableGVOptions::setSelectionMode(GraphicsViewInterface::SelectionMode mode)
{
    if(mode != GraphicsViewInterface::NONE)
    {
        GraphicsViewInterface::SelectionMode m = (dynamic_cast<PB_ActionSelectItemDrawableGV*>(action()))->selectionModeToBasic(mode);

        if ((m == GraphicsViewInterface::SELECT) || (m == GraphicsViewInterface::SELECT_ONE))
            ui->toolButtonReplaceMode->setChecked(true);
        else if ((m == GraphicsViewInterface::ADD) || (m == GraphicsViewInterface::ADD_ONE))
            ui->toolButtonAddMode->setChecked(true);
        else if ((m == GraphicsViewInterface::REMOVE) || (m == GraphicsViewInterface::REMOVE_ONE))
            ui->toolButtonRemoveMode->setChecked(true);
    }
}

void PB_ActionSelectItemDrawableGVOptions::setSelectionTool(PB_ActionSelectItemDrawableGV::SelectionTool tool)
{
    if(tool == PB_ActionSelectItemDrawableGV::Rectangle)
        ui->toolButtonSelectMulti->setChecked(true);
    else if(tool == PB_ActionSelectItemDrawableGV::Polygon)
        ui->toolButtonSelectMultiByPolygon->setChecked(true);
    else
        ui->toolButtonSelectOne->setChecked(true);
}
