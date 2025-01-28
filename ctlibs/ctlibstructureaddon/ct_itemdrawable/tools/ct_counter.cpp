#include "ct_counter.h"

CT_Counter::CT_Counter(int& nTurns) : _nTurns(nTurns)
{
    _currentTurn = 1;
}
