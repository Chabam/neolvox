#include <ostream>
#include <stdexcept>
#include <string>

#include <lvox/logger/logger.hpp>

namespace lvox
{

Logger::Logger(const std::string& category, std::ostream& ostream)
    : m_category{category}
    , m_stream{ostream}
{
}

void Logger::set_global_level(Level lowest_level)
{
    g_lowest_enabled_level = lowest_level;
}

const char* Logger::level_to_text(Level level)
{
    switch (level)
    {
    case Level::Verbose:
        return "VERBOSE";
        break;
    case Level::Debug:
        return "DEBUG";
        break;
    case Level::Info:
        return "INFO";
        break;
    case Level::Warn:
        return "WARN ";
        break;
    case Level::Error:
        return "ERROR";
        break;
    default:
        throw std::runtime_error("Invalid log level provided");
        break;
    }
}

const char* Logger::level_to_color(Level level)
{
    switch (level)
    {
    case Level::Debug:
        return "\x1B[32m"; // GREEN
        break;
    case Level::Info:
        return "\x1B[34m"; // BLUE
        break;
    case Level::Warn:
        return "\x1B[33m"; // YELLOW
        break;
    case Level::Error:
        return "\x1B[31m"; // RED
        break;
    default:
        throw std::runtime_error("Invalid log level provided");
        break;
    }
}

} // namespace lvox
