#include "ct_actionselectitemdrawablegvoptions.h"
#include "ui_ct_actionselectitemdrawablegvoptions.h"
#include "documentinterface.h"

CT_ActionSelectItemDrawableGVOptions::CT_ActionSelectItemDrawableGVOptions(const CT_AbstractAction *action, CT_ActionSelectTool *selectTool) :
    CT_GAbstractActionOptions(action),
    ui(new Ui::CT_ActionSelectItemDrawableGVOptions)
{
    ui->setupUi(this);
    m_selectTool = selectTool;
}

CT_ActionSelectItemDrawableGVOptions::~CT_ActionSelectItemDrawableGVOptions()
{
    delete ui;
}

void CT_ActionSelectItemDrawableGVOptions::setAvailableModes(bool item, bool points, bool faces, bool edges)
{
    if (item)        {ui->radioButtonItems->setChecked(true);}
    else if (points) {ui->radioButtonPoints->setChecked(true);}
    else if (faces)  {ui->radioButtonFaces->setChecked(true);}
    else if (edges)  {ui->radioButtonEdges->setChecked(true);}

    int nb = item + points + faces + edges;

    if (nb < 2)
    {
        ui->radioButtonItems->setVisible(false);
        ui->radioButtonPoints->setVisible(false);
        ui->radioButtonFaces->setVisible(false);
        ui->radioButtonEdges->setVisible(false);
        ui->line_3->setVisible(false);
    } else {
        ui->radioButtonItems->setVisible(item);
        ui->radioButtonPoints->setVisible(points);
        ui->radioButtonFaces->setVisible(faces);
        ui->radioButtonEdges->setVisible(edges);
        ui->line_3->setVisible(true);
    }
}

void CT_ActionSelectItemDrawableGVOptions::setAvailableSelectionModes(bool one, bool rect, bool poly, bool replace, bool add, bool remove)
{
    int nb = one + rect + poly;

    if (nb < 2)
    {
        ui->toolButtonSelectOne->setVisible(false);
        ui->toolButtonSelectMulti->setVisible(false);
        ui->toolButtonSelectMultiByPolygon->setVisible(false);
    } else {
        ui->toolButtonSelectOne->setVisible(one);
        ui->toolButtonSelectMulti->setVisible(rect);
        ui->toolButtonSelectMultiByPolygon->setVisible(poly);
    }

    nb = replace + add + remove;

    if (nb < 2)
    {
        ui->toolButtonReplaceMode->setVisible(false);
        ui->toolButtonAddMode->setVisible(false);
        ui->toolButtonRemoveMode->setVisible(false);
    } else {
        ui->toolButtonReplaceMode->setVisible(replace);
        ui->toolButtonAddMode->setVisible(add);
        ui->toolButtonRemoveMode->setVisible(remove);
    }
}

GraphicsViewInterface::SelectionMode CT_ActionSelectItemDrawableGVOptions::selectionMode() const
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

    return static_cast<GraphicsViewInterface::SelectionMode>(mode);
}

CT_ActionSelectTool::SelectionTool CT_ActionSelectItemDrawableGVOptions::selectionTool() const
{
    if(ui->toolButtonSelectMulti->isChecked())
        return CT_ActionSelectTool::Rectangle;
    else if(ui->toolButtonSelectMultiByPolygon->isChecked())
        return CT_ActionSelectTool::Polygon;

    return CT_ActionSelectTool::Point;
}

void CT_ActionSelectItemDrawableGVOptions::on_buttonGroupType_buttonReleased(int id)
{
    Q_UNUSED(id)

    m_selectTool->setSelectionMode(selectionMode());
    m_selectTool->setSelectionTool(selectionTool());
}

void CT_ActionSelectItemDrawableGVOptions::on_buttonGroupMode_buttonReleased(int id)
{
    Q_UNUSED(id)

    m_selectTool->setSelectionMode(selectionMode());
    m_selectTool->setSelectionTool(selectionTool());
}

void CT_ActionSelectItemDrawableGVOptions::on_toolButtonToggleSelection_clicked()
{
    m_selectTool->toggleSelection();
}

void CT_ActionSelectItemDrawableGVOptions::on_buttonGroupSelection_buttonReleased(int id)
{
    Q_UNUSED(id)

    m_selectTool->setSelectionMode(selectionMode());
    m_selectTool->setSelectionTool(selectionTool());
}

void CT_ActionSelectItemDrawableGVOptions::setSelectionMode(GraphicsViewInterface::SelectionMode mode)
{
    if(mode != GraphicsViewInterface::NONE)
    {
        GraphicsViewInterface::SelectionMode m = m_selectTool->selectionModeToBasic(mode);

        if ((m == GraphicsViewInterface::SELECT) || (m == GraphicsViewInterface::SELECT_ONE))
            ui->toolButtonReplaceMode->setChecked(true);
        else if ((m == GraphicsViewInterface::ADD) || (m == GraphicsViewInterface::ADD_ONE))
            ui->toolButtonAddMode->setChecked(true);
        else if ((m == GraphicsViewInterface::REMOVE) || (m == GraphicsViewInterface::REMOVE_ONE))
            ui->toolButtonRemoveMode->setChecked(true);
    }
}

void CT_ActionSelectItemDrawableGVOptions::setSelectionTool(CT_ActionSelectItemDrawableGV::SelectionTool tool)
{
    if(tool == CT_ActionSelectItemDrawableGV::Rectangle)
        ui->toolButtonSelectMulti->setChecked(true);
    else if(tool == CT_ActionSelectItemDrawableGV::Polygon)
        ui->toolButtonSelectMultiByPolygon->setChecked(true);
    else
        ui->toolButtonSelectOne->setChecked(true);
}
