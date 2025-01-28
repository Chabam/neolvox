#ifndef CDM_PARSERACTIONMANAGER_H
#define CDM_PARSERACTIONMANAGER_H

#include "cdm_iparseraction.h"

#include <QStringList>
#include <QPair>

class COMPUTREECORESHARED_EXPORT CDM_ParserActionManager
{
public:
    CDM_ParserActionManager();
    virtual ~CDM_ParserActionManager();

    virtual bool addAction(const QString &cmd, CDM_IParserAction *action);
    CDM_IParserAction* findAction(const QString &fullCmd, QString *newCmd = nullptr) const;

    QStringList getAllActionCmd() const;

private:

    QList< QPair<QString, CDM_IParserAction*> > _actions;
};

#endif // CDM_PARSERACTIONMANAGER_H
