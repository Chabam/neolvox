#include "manager.h"

void WorkerManager::addWorker(std::unique_ptr<WorkerBase> worker)
{
    _workers.push_back(std::move(worker));
}

void WorkerManager::run()
{
    _nMaxThread = std::max(static_cast<size_t>(1), static_cast<size_t>(std::thread::hardware_concurrency()));

    //std::atomic_int counter = _workers.size();
    std::vector<std::thread> threads;

    for (const auto& worker : _workers)
    {

        threads.push_back(std::thread([&](void) {
            worker->run();
            _cv.notify_one();
            return;
        }));
    }

    // Passive wait
    for (auto& thread : threads)
        thread.join();
}
