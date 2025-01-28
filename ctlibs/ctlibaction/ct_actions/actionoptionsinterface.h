#ifndef ACTIONOPTIONSINTERFACE_H
#define ACTIONOPTIONSINTERFACE_H

#include <QWidget>

class CT_AbstractAction;

/**
 * @brief Represent a options widget of a action
 */
class ActionOptionsInterface : public QWidget
{
public:
    virtual ~ActionOptionsInterface() {}

    /**
     * @brief Return the action that created this widget
     */
    virtual CT_AbstractAction* action() const = 0;
};

#endif // ACTIONOPTIONSINTERFACE_H
