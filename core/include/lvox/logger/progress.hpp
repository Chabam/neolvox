#ifndef LVOX_PROGRESS_HPP
#define LVOX_PROGRESS_HPP

#include <atomic>
#include <iostream>
#include <thread>

namespace lvox
{
class Progress
{
    public:
    Progress(size_t goal, std::ostream& ostream = std::cout, unsigned char progress_size = 25);
    ~Progress();

    void print();
    void increase_progression_by(size_t i);
    bool completed() const;
    void stop() { m_current_percent = 100; };

    private:
    size_t m_goal;
    unsigned char m_progress_size;
    unsigned char m_current_percent;
    std::atomic_uint64_t m_current_count;
    std::ostream& m_ostream;
    std::jthread m_thread;
};
}

#endif // LVOX_PROGRESS_HPP
