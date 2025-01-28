#ifndef CT_GABSTRACTACTIONFORGRAPHICSVIEWOPTIONS_H
#define CT_GABSTRACTACTIONFORGRAPHICSVIEWOPTIONS_H

#include "ct_gabstractactionoptions.h"

class CTLIBACTION_EXPORT CT_GAbstractActionForGraphicsViewOptions : public CT_GAbstractActionOptions
{
    Q_OBJECT

public:
    CT_GAbstractActionForGraphicsViewOptions(const CT_AbstractAction *action);

protected slots:
     /**
      * @brief Call this method when the "draw" method of the action must draw something else
      *        that previously (per example if you change a parameter in this view)
      */
     void setDrawing3DChanged();

signals:
     /**
      * @brief Emitted when the drawing of 3D elements has changed
      */
     void drawing3DChanged();
};

#endif // CT_GABSTRACTACTIONFORGRAPHICSVIEWOPTIONS_H
