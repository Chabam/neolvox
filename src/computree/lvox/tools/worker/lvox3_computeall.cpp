#include "lvox3_computeall.h"

#include <QThread>
#include <QMutex>
#include <QMutexLocker>

//Compute all is the worker manager (thread manager) for computehits,computebefore,computetheoriticals

LVOX3_ComputeAll::LVOX3_ComputeAll() : LVOX3_Worker()
{
    m_mutex = new QMutex(QMutex::Recursive);
}

LVOX3_ComputeAll::~LVOX3_ComputeAll()
{
    qDeleteAll(m_workers.begin(), m_workers.end());
    delete m_mutex;
}

void LVOX3_ComputeAll::addWorker(int startIndex, LVOX3_Worker *worker)
{
    m_workers.insert(startIndex, worker);
}

void LVOX3_ComputeAll::doTheJob()
{
    QList<int> keys = m_workers.uniqueKeys();

    setProgressRange(0, m_workers.values().size()*100);

    m_nMaxThread = qMax(1, QThread::idealThreadCount());

    m_progressOffset = 0;



    foreach (int key, keys) {

        m_nCurrentThread = 0;
        m_nCurrentWorkerFinished = 0;

        m_currentWorkers = m_workers.values(key);

        foreach (LVOX3_Worker* worker, m_currentWorkers) {
            prepareAWorker(worker);
            startAWorker(worker);
        }

        waitForFinished();

        foreach (LVOX3_Worker* worker, m_currentWorkers) {
            disconnect(this, NULL, worker, NULL);
            disconnect(worker, NULL, this, NULL);
        }

        if(mustCancel())
            return;

        m_progressOffset += m_currentWorkers.size()*100;
    }
}

void LVOX3_ComputeAll::startWorker(LVOX3_Worker *worker)
{
    worker->compute();
}

void LVOX3_ComputeAll::prepareAWorker(LVOX3_Worker *worker)
{
    QThread* workerThread = new QThread();
    worker->moveToThread(workerThread);

    connect(workerThread, SIGNAL(started()), worker, SLOT(compute()));
    connect(worker, SIGNAL(finished()), this, SLOT(workerFinished()), Qt::DirectConnection);
    connect(worker, SIGNAL(finished()), workerThread, SLOT(quit()), Qt::DirectConnection);
    connect(workerThread, SIGNAL(finished()), workerThread, SLOT(deleteLater()), Qt::DirectConnection);

    connect(this, SIGNAL(cancelRequested()), worker, SLOT(cancel()), Qt::DirectConnection);
    connect(worker, SIGNAL(progressChanged(int)), this, SLOT(progressFromWorkerChanged()), Qt::DirectConnection);
}

void LVOX3_ComputeAll::startAWorker(LVOX3_Worker *worker)
{
    //worker->compute();
    QMutexLocker locker(m_mutex);

    if(m_nCurrentThread < m_nMaxThread) {
        ++m_nCurrentThread;
        worker->thread()->start();
    }
}

void LVOX3_ComputeAll::waitForFinished()
{
    int size = m_currentWorkers.size();

    while(m_nCurrentWorkerFinished < size)
        QThread::msleep(300);
}

void LVOX3_ComputeAll::progressFromWorkerChanged()
{
    int progress = m_progressOffset;

    foreach (LVOX3_Worker* worker, m_currentWorkers) {
        progress += worker->getProgress();
    }

    setProgress(progress);
}

void LVOX3_ComputeAll::workerFinished()
{
    QMutexLocker locker(m_mutex);
    --m_nCurrentThread;

    int n = m_currentWorkers.size();

    for(int i=0; (i<n) && (m_nCurrentThread < m_nMaxThread); ++i) {
        LVOX3_Worker* worker = m_currentWorkers.at(i);
        if(!worker->isFinished()
                && !worker->thread()->isRunning()) {
            startAWorker(worker);
        }
    }

    ++m_nCurrentWorkerFinished;
}
