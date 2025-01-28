/****************************************************************************

 Copyright (C) 2010-2012 the Office National des Forêts (ONF), France
                     and the Association de Recherche Technologie et Sciences (ARTS), Ecole Nationale Supérieure d'Arts et Métiers (ENSAM), Cluny, France.
                     All rights reserved.

 Contact : alexandre.piboule@onf.fr

 Developers : Michaël KREBS (ARTS/ENSAM)

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

#include "ct_abstractresult.h"
#include <QDebug>

CT_AbstractResult::CT_AbstractResult() :
    m_parentStep(nullptr),
    m_numberOfItemsVisible(0),
    m_clearFromMemoryProgressValue(100)
{
    m_mutexBusy = new QMutex(QMutex::Recursive);
    m_resultTool.m_pointer = this;
}

CT_AbstractResult::CT_AbstractResult(const CT_AbstractResult& other) : PrototypeModelInterfaceInheritTool<CT_AbstractItem, IResultForModel>(other)
{
    m_parentStep = nullptr;
    m_displayableName = other.m_displayableName;
    m_numberOfItemsVisible = 0;
    m_clearFromMemoryProgressValue = 100;
    m_mutexBusy = new QMutex(QMutex::Recursive);
    m_resultTool.m_pointer = this;
}

CT_AbstractResult::~CT_AbstractResult()
{
    delete m_mutexBusy;
}

void CT_AbstractResult::setBusy(bool busy)
{
    if(busy)
    {
        m_mutexBusy->lock();

        emit busyStateChanged(true);
    }
    else
    {
        m_mutexBusy->unlock();

        emit busyStateChanged(false);
    }
}

bool CT_AbstractResult::isBusy() const
{
    bool l = m_mutexBusy->tryLock();

    if(l)
        m_mutexBusy->unlock();

    return !l;
}

IStepForModel* CT_AbstractResult::parentStep() const
{
    return m_parentStep;
}

bool CT_AbstractResult::isVisible() const
{
    return (m_numberOfItemsVisible > 0);
}

QString CT_AbstractResult::displayableName() const
{
    if(m_displayableName.isEmpty())
        return type();

    return m_displayableName;
}

QString CT_AbstractResult::toolTip() const
{
    return displayableName();
}

QString CT_AbstractResult::type() const
{
    return staticType();
}

QString CT_AbstractResult::staticType()
{
    return QString("Result/CT_AbstractResult");
}

void CT_AbstractResult::setParentStep(const IStepForModel* step)
{
    m_parentStep = const_cast<IStepForModel*>(step);
}

void CT_AbstractResult::setDisplayableName(const QString& name)
{
    m_displayableName = name.simplified();
}

int CT_AbstractResult::clearFromMemoryProgressValue() const
{
    return (100-m_clearFromMemoryProgressValue);
}

bool CT_AbstractResult::isClearedFromMemory() const
{
    return (m_clearFromMemoryProgressValue > 0);
}

void CT_AbstractResult::clearFromMemory()
{
    setClearFromMemoryProgress(1);

    internalClearFromMemory();

    setClearFromMemoryProgress(100);
}

void CT_AbstractResult::setComplete()
{
    Q_ASSERT(m_clearFromMemoryProgressValue != 0);
    if (m_clearFromMemoryProgressValue == 0) {qDebug() << "CT_AbstractResult::setComplete" << ", " << "m_clearFromMemoryProgressValue == 0";}

    m_clearFromMemoryProgressValue = 0;
}

void CT_AbstractResult::incrementItemVisible()
{
    ++m_numberOfItemsVisible;

    if(m_numberOfItemsVisible == 1)
        emit visibilityChange(isVisible());
}

void CT_AbstractResult::decrementItemVisible()
{
    --m_numberOfItemsVisible;

    if(m_numberOfItemsVisible == 0)
        emit visibilityChange(isVisible());
}

IPrototypeToolForModel*CT_AbstractResult::prototypeToolForModel() const
{
    return &const_cast<ResultToolForModel&>(m_resultTool);
}

IResultToolForModel* CT_AbstractResult::resultToolForModel() const
{
    return &const_cast<ResultToolForModel&>(m_resultTool);
}

void CT_AbstractResult::setClearFromMemoryProgress(int progress)
{
    if((progress > 0)
        && (progress != m_clearFromMemoryProgressValue))
    {
        m_clearFromMemoryProgressValue = progress;

        emit clearFromMemoryInProgress(100-progress);
    }
}
