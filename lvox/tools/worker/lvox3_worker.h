/**
 * @author Michael Krebs (AMVALOR)
 * @date 25.01.2017
 * @version 1
 */
#ifndef LVOX3_WORKER_H
#define LVOX3_WORKER_H

#include <QObject>
#include "lvox_sharedfortest.h"

/*!
 * @brief Do a job
 */
class LVOX3_Worker : public QObject
{
    Q_OBJECT

public:
    LVOX3_Worker();
    virtual ~LVOX3_Worker() {}

    /**
     * @brief Returns true if must cancel
     */
    bool mustCancel() const;

    /**
     * @brief Returns the current progress (always between [0;100])
     */
    int getProgress() const;

    /**
     * @brief Returns true if the worker is finished
     */
    bool isFinished() const;

public slots:
    /**
     * @brief Do the job
     */
    void compute();

    /**
     * @brief Cancel the job
     */
    void cancel();

protected:
    /**
     * @brief Do the job, you must inherit this method in your class
     */
    virtual void doTheJob() = 0;

    /**
     * @brief Set the progress (between [getProgressRangeMin();getProgressRangeMax()])
     */
    void setProgress(int newProgress);

    /**
     * @brief Set the progression range
     */
    void setProgressRange(int min, int max);

    /**
     * @brief Returns the progress range min
     */
    int getProgressRangeMin() const;

    /**
     * @brief Returns the progress range max
     */
    int getProgressRangeMax() const;

    /**
     * @brief Returns the progress range
     */
    int getProgressRange() const;

private:
    int     m_progress;
    int     m_progressMin;
    int     m_progressMax;
    int     m_progressRange;
    bool    m_cancel;
    bool    m_finished;

signals:
    /**
     * @brief Emitted when the progression changed (always between [0;100])
     */
    void progressChanged(int progress);

    /**
     * @brief Emitted when the user want to cancel the process
     */
    void cancelRequested();

    /**
     * @brief Emitted when the job is finished
     */
    void finished();
};

#endif // LVOX3_WORKER_H
