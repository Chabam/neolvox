#ifndef LVOX3_LOGGER_H
#define LVOX3_LOGGER_H

#include "liblvox/tools/logger.h"


class LVOX3_Logger : public lvox::Logger
{
public:
    LVOX3_Logger() = default;

    void logDebug(std::string debugMsg) override;
    void logInfo(std::string debugMsg) override;
    void logWarning(std::string debugMsg) override;
    void logError(std::string debugMsg) override;
};

#endif // LVOX3_LOGGER_H
