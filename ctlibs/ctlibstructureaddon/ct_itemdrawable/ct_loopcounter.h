/****************************************************************************

 Copyright (C) 2010-2012 the Office National des Forets (ONF), France
                     and the Association de Recherche Technologie et Sciences (ARTS), Ecole Nationale Suprieure d'Arts et Métiers (ENSAM), Cluny, France.
                     All rights reserved.

 Contact : alexandre.piboule@onf.fr

 Developers : Alexandre PIBOULE (ONF)

 This file is part of PluginShared library 2.0.

 PluginShared is free library: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 PluginShared is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with PluginShared.  If not, see <http://www.gnu.org/licenses/lgpl.html>.

*****************************************************************************/

#ifndef CT_LOOPCOUNTER_H
#define CT_LOOPCOUNTER_H

#include "ct_itemdrawable/abstract/ct_abstractitemdrawablewithoutpointcloud.h"
#include "ct_itemdrawable/tools/ct_counter.h"
#include <QSharedPointer>

class CT_StepBeginLoop;

/**
 * @brief Manage Step Loop counter
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_LoopCounter : public CT_AbstractItemDrawableWithoutPointCloud
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_LoopCounter, CT_AbstractItemDrawableWithoutPointCloud, Loop counter)
    using SuperClass = CT_AbstractItemDrawableWithoutPointCloud;

public:
    CT_LoopCounter();
    CT_LoopCounter(QSharedPointer<CT_Counter> counter, CT_StepBeginLoop* beginStep);
    CT_LoopCounter(const CT_LoopCounter& other) = default;

    inline int nTurns() const {return _counter->nTurns();}
    inline int currentTurn() const {return _counter->currentTurn();}
    inline QString turnName() const {return _counter->turnName();}
    inline bool hasNextTurn() {return _counter->hasNextTurn();}

    inline void beginNextTurn() {_counter->beginNextTurn();}

    inline void setCurrentTurn(int n) {_counter->setCurrentTurn(n);}

    inline void setTurnName(QString name) {_counter->setTurnName(name);}

    inline CT_StepBeginLoop* beginLoopStep() {return _beginStep;}

    CT_ITEM_COPY_IMP(CT_LoopCounter)

protected:
    QSharedPointer<CT_Counter>     _counter;
    CT_StepBeginLoop*              _beginStep;

private:
    CT_DEFAULT_IA_BEGIN(CT_LoopCounter)
    CT_DEFAULT_IA_V2(CT_LoopCounter, CT_AbstractCategory::staticInitDataValue(), &CT_LoopCounter::turnName, QObject::tr("TurnName"))
    CT_DEFAULT_IA_V2(CT_LoopCounter, CT_AbstractCategory::staticInitDataNumber(), &CT_LoopCounter::currentTurn, QObject::tr("CurrentTurn"))
    CT_DEFAULT_IA_V2(CT_LoopCounter, CT_AbstractCategory::staticInitDataNumber(), &CT_LoopCounter::nTurns, QObject::tr("NTurn"))
    CT_DEFAULT_IA_END(CT_LoopCounter)
};

#endif // CT_LOOPCOUNTER_H
