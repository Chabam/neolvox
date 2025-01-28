#include "cdm_abstractparseraction.h"

CDM_AbstractParserAction::CDM_AbstractParserAction()
{
}

bool CDM_AbstractParserAction::execute(const QString &fullCmd)
{
    setExecuteError("");
    setExecuteResponse("");

    bool ret = internalExecute(fullCmd);

    if(!ret && getExecuteError().isEmpty())
        setExecuteError("UNKNOWN ERROR");

    return ret;
}

QString CDM_AbstractParserAction::getExecuteError() const
{
    return _errorStr;
}

QString CDM_AbstractParserAction::getExecuteResponse() const
{
    return _validStr;
}

void CDM_AbstractParserAction::setExecuteError(const QString &errorStr)
{
    _errorStr = errorStr;
}

void CDM_AbstractParserAction::setExecuteResponse(const QString &validStr)
{
    _validStr = validStr;
}
