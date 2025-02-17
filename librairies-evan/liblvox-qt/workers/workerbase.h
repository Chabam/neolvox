#ifndef WORKER_H
#define WORKER_H

#include "../tools/logger.h"

class __declspec(dllexport) WorkerBase
{
public:
    WorkerBase(lvox::Logger* logger);
    virtual void run() = 0;

protected:
    lvox::Logger* _logger;

};

#endif // WORKER_H
