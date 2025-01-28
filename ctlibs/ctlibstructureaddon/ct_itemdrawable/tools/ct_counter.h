#ifndef CT_COUNTER_H
#define CT_COUNTER_H

#include "ctlibstructureaddon_global.h"

#include <QString>

class CTLIBSTRUCTUREADDON_EXPORT CT_Counter
{
public:
    CT_Counter(int &nTurns);

    inline void setNTurns(int n) {_nTurns = n;}
    inline void setCurrentTurn(int n) {_currentTurn = n;}
    inline void setTurnName(QString turnName) {_turnName = turnName;}

    inline int nTurns() const {return _nTurns;}
    inline int currentTurn() const {return _currentTurn;}
    inline QString turnName() const {return _turnName;}
    inline bool hasNextTurn() {return _currentTurn < _nTurns;}
    inline bool isFinished() {return _currentTurn >= _nTurns;}

    inline void beginNextTurn() {++_currentTurn;}

private:
    int&        _nTurns;
    int         _currentTurn;
    QString     _turnName;

};

#endif // CT_COUNTER_H
