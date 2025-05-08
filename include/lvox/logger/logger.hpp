#ifndef LVOX_LOGGER_HPP
#define LVOX_LOGGER_HPP

#include <chrono>
#include <format>
#include <iostream>
#include <string>
#include <string_view>

namespace lvox
{

struct Logger
{
    enum class Level
    {
        Verbose,
        Debug,
        Info,
        Warn,
        Error
    };

    Logger(const std::string& category, std::ostream& ostream = std::cout);

    template <typename... Args>
    auto log(Level level, std::string_view text, const Args&... args) -> void
    {
        if (g_lowest_enabled_level > level)
        {
            return;
        }

        constexpr auto WHITE          = "\033[0m";
        auto           color          = level_to_color(level);
        const auto     level_text     = level_to_text(level);
        std::string    formatted_text = std::vformat(text, std::make_format_args(args...));
        namespace ch                  = std::chrono;
        auto current_time =
            ch::current_zone()->to_local(ch::floor<ch::milliseconds>(ch::system_clock::now()));
        m_stream << std::vformat(
                        "{:%T} {}[{}][{}]{} {}",
                        std::make_format_args(
                            current_time, color, level_text, m_category, WHITE, formatted_text
                        )
                    )
                 << std::endl;
    }

    template <typename... Args>
    auto verbose(std::string_view text, const Args... args) -> void
    {
        return log(Level::Verbose, text, args...);
    }

    template <typename... Args>
    auto debug(std::string_view text, const Args... args) -> void
    {
        return log(Level::Debug, text, args...);
    }

    template <typename... Args>
    auto info(std::string_view text, const Args... args) -> void
    {
        return log(Level::Info, text, args...);
    }

    template <typename... Args>
    auto warn(std::string_view text, const Args... args) -> void
    {
        return log(Level::Warn, text, args...);
    }

    template <typename... Args>
    auto error(std::string_view text, const Args... args) -> void
    {
        return log(Level::Error, text, args...);
    }

    auto add_subcategory(const std::string& category) -> void { m_category += "|" + category; }

    static auto set_global_level(Level level) -> void;

  private:
    static const char* level_to_text(Level level);
    static const char* level_to_color(Level level);

    std::string   m_category;
    std::ostream& m_stream;

#ifdef LVOX_DEBUG
    inline static Level g_lowest_enabled_level = Level::Debug;
#else
    inline static Level g_lowest_enabled_level = Level::Info;
#endif // LVOX_DEBUG
    //
};

} // namespace lvox

#endif // LVOX_LOGGER_HPP
