#ifndef LOGGER_H
#define LOGGER_H
#include <string>
namespace lvox {
class Logger
{
public:
    virtual void logDebug(std::string debugMsg) = 0;
    virtual void logInfo(std::string debugMsg) = 0;
    virtual void logWarning(std::string debugMsg) = 0;
    virtual void logError(std::string debugMsg) = 0;
};
}
#endif // LOGGER_H
