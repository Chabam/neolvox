#include "ct_logmanageradapterforstep.h"

#include "ct_log/ct_logmanager.h"

CT_LogManagerAdapterForStep* CT_LogManagerAdapterForStep::UNIQUE_INSTANCE = nullptr;

void CT_LogManagerAdapterForStep::addMessage(const int& severity, const QString& s, const QString& filter)
{
    PS_LOG->addMessage(severity, LogInterface::step, s, filter);
}

void CT_LogManagerAdapterForStep::addTraceMessage(const QString& s, const QString& filter)
{
    PS_LOG->addTraceMessage(LogInterface::step, s, filter);
}

void CT_LogManagerAdapterForStep::addDebugMessage(const QString& s, const QString& filter)
{
    PS_LOG->addDebugMessage(LogInterface::step, s, filter);
}

void CT_LogManagerAdapterForStep::addInfoMessage(const QString& s, const QString& filter)
{
    PS_LOG->addInfoMessage(LogInterface::step, s, filter);
}

void CT_LogManagerAdapterForStep::addWarningMessage(const QString& s, const QString& filter)
{
    PS_LOG->addWarningMessage(LogInterface::step, s, filter);
}

void CT_LogManagerAdapterForStep::addErrorMessage(const QString& s, const QString& filter)
{
    PS_LOG->addErrorMessage(LogInterface::step, s, filter);
}

void CT_LogManagerAdapterForStep::addFatalMessage(const QString& s, const QString& filter)
{
    PS_LOG->addFatalMessage(LogInterface::step, s, filter);
}
