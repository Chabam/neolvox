#include "ct_gabstractactionoptions.h"

CT_GAbstractActionOptions::CT_GAbstractActionOptions(const CT_AbstractAction* action) :
    ActionOptionsInterface()
{
    m_action = const_cast<CT_AbstractAction*>(action);
}

CT_AbstractAction* CT_GAbstractActionOptions::action() const
{
    return m_action;
}
