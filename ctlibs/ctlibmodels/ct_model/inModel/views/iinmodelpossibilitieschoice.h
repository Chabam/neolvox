#ifndef IINMODELPOSSIBILITIESCHOICE_H
#define IINMODELPOSSIBILITIESCHOICE_H

#include "ctlibmodels_global.h"

#include <QWidget>

class CT_InStdResultModelPossibility;

class CTLIBMODELS_EXPORT IInModelPossibilitiesChoice : public QWidget
{
    Q_OBJECT

public:
    IInModelPossibilitiesChoice(QWidget* parent = nullptr) : QWidget(parent) {}
    virtual ~IInModelPossibilitiesChoice() {}

    /**
     * @brief Returns true if the user cannot change anything but just display the treeview
     */
    virtual bool isReadOnly() const = 0;

public slots:
    /**
     * @brief Set which "input result model possibility" to display
     * @param possibility : possibility to use to construct the treeview
     */
    virtual void setInResultModelPossibility(const CT_InStdResultModelPossibility* possibility) = 0;

    /**
     * @brief Set to true if you want that the user cannot change anything but just display the treeview
     */
    virtual void setReadOnly(bool enabled) = 0;

    virtual int getContentHeight() {return 0;}

};

#endif // IINMODELPOSSIBILITIESCHOICE_H
