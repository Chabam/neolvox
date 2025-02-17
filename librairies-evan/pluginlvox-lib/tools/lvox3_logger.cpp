#include "lvox3_logger.h"
#include "ct_log/ct_logmanager.h"
#include "qdebug.h"

void LVOX3_Logger::logDebug(std::string debugMsg) {
    PS_LOG->addDebugMessage(LogInterface::step, QString::fromStdString(debugMsg));
    //qDebug().noquote() << QString::fromStdString(debugMsg);
}

void LVOX3_Logger::logInfo(std::string debugMsg) {
    PS_LOG->addInfoMessage(LogInterface::step, QString::fromStdString(debugMsg));
    //CT_LogManager::staticInstance()->addInfoMessage(LogInterface::step, QString::fromStdString(debugMsg));

}

void LVOX3_Logger::logWarning(std::string debugMsg) {
    PS_LOG->addWarningMessage(LogInterface::step, QString::fromStdString(debugMsg));
    // qDebug().noquote() << QString::fromStdString(debugMsg);
}

void LVOX3_Logger::logError(std::string debugMsg) {
    PS_LOG->addErrorMessage(LogInterface::step, QString::fromStdString(debugMsg));
    //qDebug() << QString::fromStdString(debugMsg);
}
