/**
 * @author Michael Krebs (AMVALOR)
 * @date 25.01.2017
 * @version 1
 */
#ifndef LVOX3_COMPUTEALL_H
#define LVOX3_COMPUTEALL_H

#include "lvox3_worker.h"

#include <QMultiMap>

class QMutex;

/**
 * @brief Use this class to manage multiple worker
 */
class LVOX3_ComputeAll : public LVOX3_Worker
{
    Q_OBJECT

public:
    LVOX3_ComputeAll();
    ~LVOX3_ComputeAll();

    /**
     * @brief Add a worker to manage
     * @param startIndex : worker with the same index will be started at the same time. And worker
     *                     from index+1 was not started before all workers on current index was finished successfully.
     * @param worker : worker to add
     */
    void addWorker(int startIndex, LVOX3_Worker* worker);

protected:
    /**
     * @brief Do the job
     */
    void doTheJob();

private:
    QMultiMap<int, LVOX3_Worker*>   m_workers;
    QList<LVOX3_Worker*>            m_currentWorkers;
    int                             m_progressOffset;
    QMutex*                         m_mutex;
    int                             m_nMaxThread;
    int                             m_nCurrentThread;
    int                             m_nCurrentWorkerFinished;

    /**
     * @brief Called by QtConcurrent to start the worker
     */
    static void startWorker(LVOX3_Worker* worker);

    /**
     * @brief Prepare a worker to start it in a thread
     */
    void prepareAWorker(LVOX3_Worker *worker);

    /**
     * @brief Start a worker and update m_nCurrentThread variable
     */
    void startAWorker(LVOX3_Worker *worker);

    /**
     * @brief Wait while all worker has finished her job
     */
    void waitForFinished();

private slots:
    /**
     * @brief Call when the progression of a worker changed
     */
    void progressFromWorkerChanged();

    /**
     * @brief Called when a worker has finished
     */
    void workerFinished();
};

#endif // LVOX3_COMPUTEALL_H
