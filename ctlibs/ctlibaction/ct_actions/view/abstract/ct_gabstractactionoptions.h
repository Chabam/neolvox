#ifndef CT_GABSTRACTACTIONOPTIONS_H
#define CT_GABSTRACTACTIONOPTIONS_H

#include "ctlibaction_global.h"
#include "ct_actions/actionoptionsinterface.h"

class CTLIBACTION_EXPORT CT_GAbstractActionOptions : public ActionOptionsInterface
{
    Q_OBJECT

public:
    explicit CT_GAbstractActionOptions(const CT_AbstractAction* action);

     CT_AbstractAction* action() const;

private:
     CT_AbstractAction* m_action;
};

#endif // CT_GABSTRACTACTIONOPTIONS_H
