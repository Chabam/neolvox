#ifndef CT_ACTIONSSEPARATOR_H
#define CT_ACTIONSSEPARATOR_H

#include "ctlibplugin_global.h"

#include "ct_actions/abstract/ct_abstractaction.h"

class CTLIBPLUGIN_EXPORT CT_ActionsSeparator
{
public:
    CT_ActionsSeparator(const QString& title = "");
    virtual ~CT_ActionsSeparator();

    bool addAction(CT_AbstractAction* action);

    QString title() const;

    QList<CT_AbstractAction*> actions() const;

private:

    QList<CT_AbstractAction*>     m_actions;
    QString                     m_title;
};

#endif // CT_ACTIONSSEPARATOR_H
