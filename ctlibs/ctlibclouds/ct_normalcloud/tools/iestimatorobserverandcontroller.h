#ifndef IESTIMATOROBSERVERANDCONTROLLER_H
#define IESTIMATOROBSERVERANDCONTROLLER_H

#include "ctlibclouds_global.h"

class CTLIBCLOUDS_EXPORT IEstimatorObserverAndController {
public:
    virtual ~IEstimatorObserverAndController() {}

    /**
     * @brief Returns true if the process must be stopped
     */
    virtual bool mustStopEstimation() const = 0;

    /**
     * @brief Set the progress of the process between 0 and 100
     */
    virtual void setEstimationProgress(int p) = 0;
};

#endif // IESTIMATOROBSERVERANDCONTROLLER_H
