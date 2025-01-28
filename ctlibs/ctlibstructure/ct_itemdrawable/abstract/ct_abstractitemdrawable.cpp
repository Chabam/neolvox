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

#include "ct_abstractitemdrawable.h"

#include "ct_global/ct_structurecontext.h"
#include "ct_itemattributes/tools/ct_defaultitemattributemanager.h"
#include "ct_result/abstract/ct_abstractresult.h"

#define _USE_MATH_DEFINES
#include <math.h>

quint32& CT_AbstractItemDrawable::NEXTID()
{
    static quint32 ID = 1;

    return ID;
}

QMap<QString, QString>& CT_AbstractItemDrawable::NAMEMAP()
{
    static QMap<QString, QString> map;

    return map;
}

QMutex& CT_AbstractItemDrawable::NAMEMAP_Mutex()
{
    static QMutex mutex;

    return mutex;
}

CT_AbstractItemDrawable::CT_AbstractItemDrawable() : SuperClass()
{
    m_itemTool.m_pointer = this;
    m_id = NEXTID()++;
    m_isSelected = false;
    m_isDisplayedInADocument = false;
    m_parentItem = nullptr;
    m_baseDrawManager = nullptr;
    m_alternativeDrawManager = nullptr;
}

CT_AbstractItemDrawable::CT_AbstractItemDrawable(const CT_AbstractItemDrawable& other) : SuperClass(other),
    m_id(other.m_id),
    m_isSelected(false),
    m_isDisplayedInADocument(false),
    m_displayableName(other.m_displayableName),
    m_parentItem(nullptr),
    m_baseDrawManager(other.m_baseDrawManager),
    m_alternativeDrawManager(other.m_alternativeDrawManager)
{
    m_itemTool.m_pointer = this;
}

CT_AbstractItemDrawable::~CT_AbstractItemDrawable()
{
}

void CT_AbstractItemDrawable::setId(quint32 id)
{
    m_id = id;
}

void CT_AbstractItemDrawable::setDisplayableName(const QString &displayableName)
{
    m_displayableName = displayableName;
}

void CT_AbstractItemDrawable::setSelected(const bool& value)
{
    if(m_isSelected != value)
    {
        m_isSelected = value;

        emit selectChange(value, this);
    }
}

void CT_AbstractItemDrawable::setDisplayed(const bool& value)
{
    if(m_isDisplayedInADocument != value)
    {
        if(value)
            result()->incrementItemVisible();
        else
            result()->decrementItemVisible();

        m_isDisplayedInADocument = value;

        emit displayChange(value, this);
    }
}

bool CT_AbstractItemDrawable::addDocumentParent(const IDocumentForModel* doc)
{
    Q_ASSERT(doc != nullptr);
    if (doc == nullptr) {qDebug() << "CT_AbstractItemDrawable::addDocumentParent" << ", " << "doc == nullptr";}

    Q_ASSERT(model() != nullptr);
    if (model() == nullptr) {qDebug() << "CT_AbstractItemDrawable::addDocumentParent" << ", " << "model() == nullptr";}

    auto ret = m_documentsWhereThisItemIs.insert(const_cast<IDocumentForModel*>(doc));

    // if it doesn't already exist in the collection
    if(ret.second == true) {
        setDisplayed(true);
        model()->incrementVisibilityInDocument(doc);
        return true;
    }

    return false;
}

void CT_AbstractItemDrawable::removeDocumentParent(const IDocumentForModel* doc)
{
    Q_ASSERT(doc != nullptr);
    if (doc == nullptr) {qDebug() << "CT_AbstractItemDrawable::removeDocumentParent" << ", " << "doc == nullptr";}

    Q_ASSERT(model() != nullptr);
    if (model() == nullptr) {qDebug() << "CT_AbstractItemDrawable::removeDocumentParent" << ", " << "model() == nullptr";}

    if(m_documentsWhereThisItemIs.erase(const_cast<IDocumentForModel*>(doc)) != 0) {
        model()->decrementVisibilityInDocument(doc);

        if(m_documentsWhereThisItemIs.empty())
        {
            setDisplayed(false);
            setSelected(false);
        }
    }
}

QString CT_AbstractItemDrawable::name() const
{
    return metaObject()->className();
}

QString CT_AbstractItemDrawable::staticName()
{
    return staticMetaObject.className();
}

QString CT_AbstractItemDrawable::displayableName() const
{
    // TODO : MK 19.11.18 check if this is ok ??
    if(!m_displayableName.isEmpty())
        return m_displayableName;

    Q_ASSERT(model() != nullptr);
    if (model() == nullptr) {qDebug() << "CT_AbstractItemDrawable::displayableName" << ", " << "model() == nullptr";}

    return QString("%1_%2").arg(model()->displayableName()).arg(QVariant(id()).toString());

    //return QString("%1_%2").arg(name()).arg(QVariant(id()).toString());
}

QString CT_AbstractItemDrawable::staticType()
{
    return "ItemDrawable/CT_AbstractItemDrawable";
}

CT_AbstractItemDrawable* CT_AbstractItemDrawable::parentItem() const
{
    return m_parentItem;
}

bool CT_AbstractItemDrawable::isInDocument(const IDocumentForModel* doc) const
{
    return (m_documentsWhereThisItemIs.find(const_cast<IDocumentForModel*>(doc)) != m_documentsWhereThisItemIs.end());
}

void CT_AbstractItemDrawable::setAlternativeDrawManager(const CT_AbstractItemDrawableDrawManager* drawManager)
{
    m_alternativeDrawManager = const_cast<CT_AbstractItemDrawableDrawManager*>(drawManager);

    if((m_alternativeDrawManager != nullptr) && (m_alternativeDrawManager->drawConfiguration() == nullptr))
        m_alternativeDrawManager->initDrawConfiguration();
}

const CT_AbstractItemDrawableDrawManager* CT_AbstractItemDrawable::alternativeDrawManager() const
{
    return m_alternativeDrawManager;
}

const CT_AbstractItemDrawableDrawManager* CT_AbstractItemDrawable::baseDrawManager() const
{
    return m_baseDrawManager;
}

void CT_AbstractItemDrawable::setParentItem(const CT_AbstractItemDrawable* parent)
{
    m_parentItem = const_cast<CT_AbstractItemDrawable*>(parent);
}

void CT_AbstractItemDrawable::setBaseDrawManager(const CT_AbstractItemDrawableDrawManager* drawManager)
{
    m_baseDrawManager = const_cast<CT_AbstractItemDrawableDrawManager*>(drawManager);

    if((m_baseDrawManager != nullptr) && (m_baseDrawManager->drawConfiguration() == nullptr))
        m_baseDrawManager->initDrawConfiguration();
}

bool CT_AbstractItemDrawable::isSelected() const
{
    return m_isSelected;
}

bool CT_AbstractItemDrawable::isDisplayed() const
{
    return m_isDisplayedInADocument;
}

bool CT_AbstractItemDrawable::doesBoundingBoxAppearToBeValid() const
{
    if(hasBoundingBox()) {
        Eigen::Vector3d min, max;
        boundingBox(min, max);

        return (min(0) <= max(0)) &&
                (min(1) <= max(1)) &&
                (min(2) <= max(2));
    }

    return false;
}

void CT_AbstractItemDrawable::boundingBox(Eigen::Vector3d &min, Eigen::Vector3d &max) const
{
    min(0) = 0;
    min(1) = 0;
    min(2) = 0;

    max(0) = 0;
    max(1) = 0;
    max(2) = 0;
}

QString CT_AbstractItemDrawable::info()
{
    return QString("%1 %2").arg(id()).arg(name());
}

void CT_AbstractItemDrawable::draw(GraphicsViewInterface& view, PainterInterface& painter)
{
    if(m_alternativeDrawManager != nullptr)
    {
        m_alternativeDrawManager->preDraw(view, painter, *this);
        m_alternativeDrawManager->draw(view, painter, *this);
        m_alternativeDrawManager->postDraw(view, painter, *this);
    }
    else if(m_baseDrawManager != nullptr)
    {
        m_baseDrawManager->preDraw(view, painter, *this);
        m_baseDrawManager->draw(view, painter, *this);
        m_baseDrawManager->postDraw(view, painter, *this);
    }
}

CT_ItemDrawableConfiguration* CT_AbstractItemDrawable::drawConfiguration()
{
    if(m_alternativeDrawManager != nullptr)
        return (CT_ItemDrawableConfiguration*)m_alternativeDrawManager->drawConfiguration();

    if(m_baseDrawManager != nullptr)
        return (CT_ItemDrawableConfiguration*)m_baseDrawManager->drawConfiguration();

    return nullptr;
}

QList<CT_ItemDrawableConfiguration*> CT_AbstractItemDrawable::dependantDrawConfigurations() const
{
    QSet<CT_ItemDrawableConfiguration*> set;

    const auto visitor = [&set](const CT_OutAbstractModel* childModel) -> bool {
        CT_AbstractItemDrawable* child = dynamic_cast<CT_AbstractItemDrawable*>(childModel->prototype());

        if(child != nullptr)
            set.insert(child->drawConfiguration());

        return true;
    };

    model()->recursiveVisitOutChildrens(visitor);

    return set.values();
}

void CT_AbstractItemDrawable::addNameTypeCorresp(QString type, QString name)
{
    QMutexLocker locker(&NAMEMAP_Mutex());

    NAMEMAP().insert(type, name);
}

QString CT_AbstractItemDrawable::nameFromType(QString type)
{
    QMutexLocker locker(&NAMEMAP_Mutex());

    return NAMEMAP().value(type);
}

bool CT_AbstractItemDrawable::visitDefaultAttributes(const IItemDrawableToolForModel::ItemAttributesVisitor& visitor) const
{
    const auto visitorAdapter = [&visitor](const CT_AbstractItemAttribute* itemAttribute) -> bool {
        return visitor(itemAttribute);
    };

    return PS_DIAM->visitItemAttributesForType(type(), visitorAdapter);
}

bool CT_AbstractItemDrawable::recursiveVisitChildrensOfTypeItem(const ItemVisitor& visitor) const
{
    const auto visitorAdapter = [&visitor](const CT_AbstractItemDrawable* child) -> bool {
        if(!visitor(child))
            return false;

        return child->recursiveVisitChildrensOfTypeItem(visitor);
    };

    return visitChildrensOfTypeItem(visitorAdapter);
}

IItemDrawableToolForModel* CT_AbstractItemDrawable::itemToolForModel() const
{
    return &const_cast<ItemToolForModel&>(m_itemTool);
}
