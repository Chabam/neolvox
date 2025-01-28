#ifndef CDM_ABSTRACTPARSERACTION_H
#define CDM_ABSTRACTPARSERACTION_H

#include "cdm_iparseraction.h"

class COMPUTREECORESHARED_EXPORT CDM_AbstractParserAction : public CDM_IParserAction
{
public:
    CDM_AbstractParserAction();

    bool execute(const QString &fullCmd);
    QString getExecuteError() const;
    QString getExecuteResponse() const;

protected:

    virtual bool internalExecute(const QString &fullCmd) = 0;

    void setExecuteError(const QString &errorStr);
    void setExecuteResponse(const QString &validStr);

private:

    QString _errorStr;
    QString _validStr;
};

#endif // CDM_ABSTRACTPARSERACTION_H
