#ifndef PB_ACTIONPICKITEMSINLISTOPTIONS_H
#define PB_ACTIONPICKITEMSINLISTOPTIONS_H

#include "ct_actions/view/abstract/ct_gabstractactionoptions.h"

#include "ctlibstdactions/ctlibstdactions_global.h"

namespace Ui {
class PB_ActionPickItemsInListOptions;
}

class CTLIBSTDACTIONS_EXPORT PB_ActionPickItemsInListOptions : public CT_GAbstractActionOptions
{
    Q_OBJECT
    using SuperClass = CT_GAbstractActionOptions;

public:

    explicit PB_ActionPickItemsInListOptions(const CT_AbstractAction* action);
    ~PB_ActionPickItemsInListOptions();

    QColor getColor() const;
    float getMaxDist() const;
    bool shouldAutoCenterCamera() const;

private:
    Ui::PB_ActionPickItemsInListOptions *ui;

signals:
    void parametersChanged();
    void allClicked();
    void noneClicked();

private slots:
    void on_pb_color_clicked();
    void on_dsb_maxDist_valueChanged(double arg1);
    void on_pb_all_clicked();
    void on_pb_none_clicked();

private:
    QColor _itemColor;
};

#endif // PB_ACTIONPICKITEMSINLISTOPTIONS_H
