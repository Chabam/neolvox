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

#ifndef CT_CONTAINER_H
#define CT_CONTAINER_H

#include "ct_itemdrawable/abstract/ct_abstractsingularitemdrawable.h"
#include "ct_itemdrawable/tools/drawmanager/ct_standardcontainerdrawmanager.h"

/*!
 * \class CT_Container
 * \ingroup PluginShared_Items
 * \brief <b>Collection of ItemDrawables of the same type</b>
 *
 * \note Not for direct use by plugin developpers
 *
 */
class CTLIBCORE_EXPORT CT_Container : public CT_AbstractSingularItemDrawable
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_Container, CT_AbstractItemDrawable, Container)

    using SuperClass = CT_AbstractSingularItemDrawable;

public:
    using ItemVisitor = std::function<bool(const CT_AbstractItemDrawable*)>;

    CT_Container();

    /**
     * @brief Copy constructor.
     *
     *        What is copied :
     *          - Pointer of the result and model of the original item.
     *          - ID
     *          - Pointer of base and alternative draw manager
     *          - Pointer of item drawable
     *          - Pointer of item drawable that must be removed later
     *
     *        What is initialized differently :
     *          - Parent is set to NULL
     *          - isSelected and isDisplayed is set to false
     *          - Document list is not copied
     *          - AutoDelete is set to false
     */
    CT_Container(const CT_Container& other);
    ~CT_Container();

    /**
     * @brief Add a Item to this container (to the list)
     * @param item : the item to add
     */
    void addItemDrawable(CT_AbstractItemDrawable* item);

    /**
     * @brief Remove the item passed in parameter from this container. It will be deleted from memory if isAutoDelete() return true.
     * @param item : the item to remove
     * @return false if the item is not in this container
     */
    bool removeItemDrawable(CT_AbstractItemDrawable* item);

    /**
     * @brief Returns the number of item that will not be removed later
     */
    int nItems() const;

    /**
     * @brief Remove the item passed in parameter from this container. Item will not be deleted from memory.
     * @param item : the item to remove
     * @return false if the item is not in this container
     */
    bool removeItemDrawableWithoutAutoDelete(CT_AbstractItemDrawable* item);

    /**
     * @brief Visit all item drawable (that will not be removed later) of this container
     * @param visitor : the visitor to use
     * @return Returns true if no items has been visited otherwise returns the result of the visitor.
     */
    bool visitItemDrawable(const ItemVisitor& visitor) const;

    /**
     * @brief Update the center
     */
    void updateCenter();

    /**
     * @brief Called from Result or parent item to inform that this item will be removed from the result passed in parameter
     */
    void willBeRemovedFromResult(const CT_AbstractResult* res) override;

    CT_AbstractItemDrawable* copy(const CT_OutAbstractItemModel* newModelToUse, const CT_AbstractResult* newResult) const override;

    virtual QList<CT_ItemDrawableConfiguration*> getDependantViewConfigurations();

    /**
     * @brief Inform the container that all items of this container will be removed later
     * @warning Not intended for direct use by plugin developper
     */
    void setAllItemDrawableBeRemovedLater();

    /**
     * @brief Inform the container that the item of this container will be removed later
     * @warning Not intended for direct use by plugin developper
     */
    void setItemDrawableBeRemovedLater(const CT_AbstractItemDrawable* item);

    /**
     * @brief Inform the container that it must undo to remove later the item passed in parameter
     * @warning Not intended for direct use by plugin developper
     */
    void undoItemDrawableBeRemovedLater(const CT_AbstractItemDrawable* item);

private:
    QList<CT_AbstractItemDrawable*>                 m_itemNotBeRemovedLater;
    QList<CT_AbstractItemDrawable*>                 m_itemToBeRemovedLater;
    QString                                         m_defaultItemDrawableType;

    const static CT_StandardContainerDrawManager    CONTAINER_DRAW_MANAGER;
};

#endif // CT_CONTAINER_H
