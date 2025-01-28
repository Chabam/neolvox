#ifndef CT_CONFIGURABLEWIDGETTODIALOG_H
#define CT_CONFIGURABLEWIDGETTODIALOG_H

#include "ctlibwidget_global.h"

class CT_AbstractConfigurableWidget;

/**
 * @brief Class tools to show a CT_AbstractConfigurableWidget in a QDialog
 */
class CTLIBWIDGET_EXPORT CT_ConfigurableWidgetToDialog
{
public:
    CT_ConfigurableWidgetToDialog();

    /**
     * @brief Show a dialog with "Apply" and "Cancel" button that was mapped to "updateElement" slot of the widget
     * @param widget : the widget to insert in the dialog
     * @return the QDialog return code
     */
    static int exec(CT_AbstractConfigurableWidget *widget);
};

#endif // CT_CONFIGURABLEWIDGETTODIALOG_H
