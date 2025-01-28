#include "ct_gabstractactionforgraphicsviewoptions.h"

CT_GAbstractActionForGraphicsViewOptions::CT_GAbstractActionForGraphicsViewOptions(const CT_AbstractAction *action) : CT_GAbstractActionOptions(action)
{
}

void CT_GAbstractActionForGraphicsViewOptions::setDrawing3DChanged()
{
    emit drawing3DChanged();
}
