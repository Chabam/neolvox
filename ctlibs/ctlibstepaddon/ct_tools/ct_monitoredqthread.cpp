#include "ct_monitoredqthread.h"

CT_MonitoredQThread::CT_MonitoredQThread() : QThread()
{
    _progress = 0;
}
