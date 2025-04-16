#include <ostream>
#include <lvox/logger/logger.hpp>
#include <stdexcept>
#include <string>

namespace lvox
{

Logger::Logger(const std::string& category, Level lowest_enabled_level, std::ostream& ostream)
    : m_category{category}
    , m_lowest_enabled_level{lowest_enabled_level}
    , m_stream{ostream}
{
}

auto Logger::level_to_text(Level level) -> const char*
{
    switch (level)
    {
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

auto Logger::level_to_color(Level level) -> const char*
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

} // namespace rt
