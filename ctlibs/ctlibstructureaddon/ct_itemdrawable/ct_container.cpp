/****************************************************************************

 Copyright (C) 2010-2012 the Office National des Forêts (ONF), France
                     and the Association de Recherche Technologie et Sciences (ARTS), Ecole Nationale Suprieure d'Arts et Métiers (ENSAM), Cluny, France.
                     All rights reserved.

 Contact : alexandre.piboule@onf.fr

 Developers : Michal KREBS (ARTS/ENSAM)

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

#include "ct_container.h"

const CT_StandardContainerDrawManager CT_Container::CONTAINER_DRAW_MANAGER;

CT_Container::CT_Container() : SuperClass()
{
    setBaseDrawManager(&CONTAINER_DRAW_MANAGER);
}

CT_Container::CT_Container(const CT_Container& other) : SuperClass(other),
    m_itemNotBeRemovedLater(other.m_itemNotBeRemovedLater),
    m_itemToBeRemovedLater(other.m_itemToBeRemovedLater),
    m_defaultItemDrawableType(other.m_defaultItemDrawableType)
{
    setAutoDelete(false);
}

CT_Container::~CT_Container()
{
    if(isAutoDelete()) {
        qDeleteAll(m_itemNotBeRemovedLater.begin(), m_itemNotBeRemovedLater.end());
        qDeleteAll(m_itemToBeRemovedLater.begin(), m_itemToBeRemovedLater.end());
    }
}

void CT_Container::addItemDrawable(CT_AbstractItemDrawable* item)
{
    m_itemNotBeRemovedLater.append(item);
}

bool CT_Container::removeItemDrawable(CT_AbstractItemDrawable* item)
{
    if(m_itemNotBeRemovedLater.removeOne(item) || m_itemToBeRemovedLater.removeOne(item))
    {
        if(isAutoDelete())
            delete item;

        return true;
    }

    return false;
}

int CT_Container::nItems() const
{
    return m_itemNotBeRemovedLater.size();
}

bool CT_Container::removeItemDrawableWithoutAutoDelete(CT_AbstractItemDrawable* item)
{
    return (m_itemNotBeRemovedLater.removeOne(item) || m_itemToBeRemovedLater.removeOne(item));
}

bool CT_Container::visitItemDrawable(const CT_Container::ItemVisitor& visitor) const
{
    for(const CT_AbstractItemDrawable* item : m_itemNotBeRemovedLater) {
        if(!visitor(item))
            return false;
    }

    return true;
}

void CT_Container::setItemDrawableBeRemovedLater(const CT_AbstractItemDrawable *item)
{
    if(m_itemNotBeRemovedLater.removeOne(const_cast<CT_AbstractItemDrawable*>(item)))
        m_itemToBeRemovedLater.append(const_cast<CT_AbstractItemDrawable*>(item));
}

void CT_Container::undoItemDrawableBeRemovedLater(const CT_AbstractItemDrawable *item)
{
    if(m_itemToBeRemovedLater.removeOne(const_cast<CT_AbstractItemDrawable*>(item)))
        m_itemNotBeRemovedLater.append(const_cast<CT_AbstractItemDrawable*>(item));
}

void CT_Container::updateCenter()
{
    const int size = m_itemNotBeRemovedLater.size();

    Eigen::Vector3d c(0,0,0);

    if(size > 0) {

        for(const CT_AbstractItemDrawable* item : m_itemNotBeRemovedLater)
        {
            c(0) += item->getCenterX();
            c(1) += item->getCenterY();
            c(2) += item->getCenterZ();
        }

        c /= double(size);
    }

    setCenterCoordinate(c);
}

CT_AbstractItemDrawable* CT_Container::copy(const CT_OutAbstractItemModel* newModelToUse, const CT_AbstractResult* newResult) const
{
    CT_Container* container = new CT_Container(*this);
    container->setModel(newModelToUse);
    container->setResult(newResult);

    return container;
}

QList<CT_ItemDrawableConfiguration*> CT_Container::getDependantViewConfigurations()
{
    QList<CT_ItemDrawableConfiguration*> finalList;

    if(!m_itemNotBeRemovedLater.isEmpty())
    {
        finalList.append(m_itemNotBeRemovedLater.first()->getViewConfiguration());
        finalList.append(m_itemNotBeRemovedLater.first()->getDependantViewConfigurations());
    }

    return finalList;
}

void CT_Container::setAllItemDrawableBeRemovedLater()
{
    m_itemToBeRemovedLater.append(m_itemNotBeRemovedLater);
    m_itemNotBeRemovedLater.clear();
}

void CT_Container::willBeRemovedFromResult(const CT_AbstractResult* res)
{
    for(CT_AbstractItemDrawable* item : m_itemNotBeRemovedLater)
    {
        item->willBeRemovedFromResult(res);
    }

    for(CT_AbstractItemDrawable* item : m_itemToBeRemovedLater)
    {
        item->willBeRemovedFromResult(res);
    }
}
