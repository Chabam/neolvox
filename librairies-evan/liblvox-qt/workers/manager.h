#ifndef WORKERMANAGER_H
#define WORKERMANAGER_H

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <stdint.h>
#include <thread>
#include <vector>

#include "workerbase.h"

/**
 * @class WorkerManager
 * @brief WorkerManager class extends WorkerBase and serves as a manager for multiple worker threads.
 *
 * The WorkerManager is responsible for managing a pool of worker threads and coordinating their execution.
 * Users can add worker tasks to the WorkerManager, which will be executed concurrently by the available worker threads.

 */
class WorkerManager : public WorkerBase
{
public:
    /**
     * @brief run() override function from WorkerBase. This function is executed when the WorkerManager is started.
     */
    virtual void run() override;

private:
    /**
     * @brief getIdealThreadCount() calculates and returns the ideal thread count for the system.
     * @return The ideal thread count based on the system's capabilities.
     */
    size_t getIdealThreadCount();

    /**
     * @brief addWorker() adds a custom worker task to the WorkerManager's pool.
     * @param worker A unique pointer to a WorkerBase object representing the custom worker task.
     */
    void addWorker(std::unique_ptr<WorkerBase> worker);

    std::vector<std::unique_ptr<WorkerBase>> _workers; /*!< Vector to hold unique pointers to custom worker tasks. */
    std::condition_variable _cv;                      /*!< Condition variable for thread coordination. */
    int _progressOffset;                              /*!< Offset value for tracking progress. */
    std::mutex* _mutex;                               /*!< Pointer to a mutex for thread synchronization. */
    size_t _nCurrentThread;                           /*!< Current number of active threads. */
    size_t _nCurrentWorkerFinished;                   /*!< Number of workers that have finished processing. */
    size_t _nMaxThread;                               /*!< Maximum number of threads used by the WorkerManager. */
};

#endif // WORKERMANAGER_H
