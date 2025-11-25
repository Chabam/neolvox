#include <atomic>
#include <cmath>
#include <mutex>
#include <ostream>
#include <sstream>
#include <thread>

#include <lvox/logger/progress.hpp>

namespace lvox
{

Progress::Progress(size_t goal, std::ostream& ostream, unsigned char progress_size)
    : m_goal{goal}
    , m_progress_size{progress_size}
    , m_current_count{0}
    , m_current_percent{0}
    , m_ostream{ostream}
    , m_thread{}
{
}

Progress::~Progress()
{
    m_ostream << std::endl;
}

void Progress::print()
{
    const unsigned int new_percent = std::ceil(
        (static_cast<float>(m_current_count.load(std::memory_order_relaxed)) / m_goal) * 100.f
    );

    if (m_current_percent > new_percent)
        return;

    m_current_percent = new_percent;

    const unsigned int compressed_progress = (m_current_percent / 100.f) * m_progress_size;
    std::ostringstream oss;
    for (unsigned int i = 0; i < m_progress_size + 2; ++i)
    {
        oss << '\b';
    }
    oss << "[";
    for (unsigned int i = 0; i < compressed_progress; ++i)
    {
        oss << "=";
    }
    for (unsigned int i = 0; i < (m_progress_size - compressed_progress); ++i)
    {
        oss << " ";
    }
    oss << "]";

    m_ostream << oss.str();
    m_ostream.flush();
}

void Progress::increase_progression_by(size_t i)
{
    m_current_count.fetch_add(i, std::memory_order_relaxed);
}

bool Progress::completed() const
{
    return m_current_percent >= 100U;
}

} // namespace lvox
