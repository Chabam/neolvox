#include "ct_actionsseparator.h"

CT_ActionsSeparator::CT_ActionsSeparator(const QString& title)
{
    m_title = title;
}

CT_ActionsSeparator::~CT_ActionsSeparator()
{
    qDeleteAll(m_actions.begin(), m_actions.end());
}

bool CT_ActionsSeparator::addAction(CT_AbstractAction *action)
{
    if(action != nullptr)
        m_actions.append(action);

    return (action != nullptr);
}

QString CT_ActionsSeparator::title() const
{
    return m_title;
}

QList<CT_AbstractAction*> CT_ActionsSeparator::actions() const
{
    return m_actions;
}
