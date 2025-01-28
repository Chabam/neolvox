#ifndef CDM_IPARSERACTION_H
#define CDM_IPARSERACTION_H

#include "computreeCore_global.h"

#include <QString>

class COMPUTREECORESHARED_EXPORT CDM_IParserAction
{
public:
    virtual ~CDM_IParserAction() {}

    virtual bool execute(const QString &fullCmd) = 0;
    virtual QString getExecuteError() const = 0;
    virtual QString getExecuteResponse() const = 0;
};

#endif // CDM_IPARSERACTION_H
