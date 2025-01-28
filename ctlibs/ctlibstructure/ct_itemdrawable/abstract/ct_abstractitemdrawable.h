/****************************************************************************

 Copyright (C) 2010-2012 the Office National des ForÃªts (ONF), France
                     and the Association de Recherche Technologie et Sciences (ARTS), Ecole Nationale Suprieure d'Arts et MÃ©tiers (ENSAM), Cluny, France.
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

#ifndef CT_ABSTRACTITEMDRAWABLE_H
#define CT_ABSTRACTITEMDRAWABLE_H

#include "ct_item/abstract/ct_abstractitem.h"
#include "ct_itemdrawable/tools/drawmanager/abstract/ct_abstractitemdrawabledrawmanager.h"
#include "ct_tools/ct_staticmethodinvoker.h"
#include "ct_tools/ct_typeinfo.h"
#include "ct_model/inModel/abstract/ct_inabstractresultmodel.h"
#include "ct_model/inModel/tools/ct_instdresultmodelpossibility.h"

#include <QMap>
#include <QString>
#include <QMutex>

#include <QDebug>

#include <set>

#include <Eigen/Core>

/**
  * @brief Use this define in your class to add type information and don't forget to use "CT_TYPE_IMPL_INIT_MACRO" in your .cpp
  * @example class MyItemDrawable : public CT_AbstractSingularItemDrawable
  *          {
  *             Q_OBJECT
  *             CT_TYPE_IMPL_MACRO(CT_AbstractSingularItemDrawable, CT_AbstractItemDrawable, Singular Item)
  *
  *          public:
  *             MyItemDrawable();
  *             ....
  *          };
  */
#define CT_TYPE_IMPL_MACRO(ThisClassName, SuperClassName, Name) \
public: \
    QString name() const override {return staticName();} \
    static QString staticName() {return #Name;} \
    QString type() const override { return staticType(); } \
    static QString staticType() { QString statType = SuperClassName::staticType() + "/" + #ThisClassName; return statType; } \
private: \
    static void staticInitNameTypeCorresp() { ThisClassName::addNameTypeCorresp(staticType(), staticName()); } \
    static CT_StaticMethodInvoker INVOKER_DEFAULT_NT;

/**
  * Use this define in your .cpp BUT if you use CT_DEFAULT_IA_INIT in your .cpp it is not necessary to call this define too.
  */
#if defined(_MSC_VER)
#define CT_TYPE_IMPL_INIT_MACRO(ThisClassName) CT_StaticMethodInvoker ThisClassName::INVOKER_DEFAULT_NT = CT_StaticMethodInvoker(&ThisClassName::staticInitNameTypeCorresp);
#else
#define CT_TYPE_IMPL_INIT_MACRO(ThisClassName) CT_StaticMethodInvoker ThisClassName::INVOKER_DEFAULT_NT = CT_StaticMethodInvoker(&ThisClassName::staticInitNameTypeCorresp);
#endif

/**
  * @brief Use this define in your class (if it was templated) to add type information and don't forget to use "CT_TYPE_TEMPLATED_IMPL_XXX_INIT_MACRO" in your .cpp
  * @example class MyItemDrawable : public CT_AbstractSingularItemDrawable
  *          {
  *             Q_OBJECT
  *             CT_TYPE_TEMPLATED_IMPL_MACRO(MyClass, T, CT_AbstractItemDrawable, Item)
  *
  *          public:
  *             MyItemDrawable();
  *             ....
  *          };
  */
#define CT_TYPE_TEMPLATED_IMPL_MACRO(ThisClassName, TemplateType, SuperClassName, Name) \
public: \
    QString name() const override {return staticName();} \
    static QString staticName() { return QString(#Name).contains("%1") ? QString(#Name).arg(CT_TypeInfo::name<TemplateType>()) : QString("%1<%2>").arg(#Name).arg(CT_TypeInfo::name<TemplateType>()); } \
    QString type() const override { return staticType(); } \
    static QString staticType() { return SuperClassName::staticType() + "/" + #ThisClassName + "<" + CT_TypeInfo::name<TemplateType>() + ">"; } \
private: \
    static void staticInitNameTypeCorresp() { ThisClassName::addNameTypeCorresp(staticType(), staticName()); } \
    static CT_StaticMethodInvoker INVOKER_DEFAULT_NT;

/**
  * Use this define in your .cpp. BUT if you use CT_DEFAULT_IA_INIT_TEMPLATED_VALUES in your .hpp it is not necessary to call this define too.
  */
#define CT_TYPE_TEMPLATED_IMPL_VALUES_INIT_MACRO(ThisClassName) \
template<> \
CT_TYPE_IMPL_INIT_MACRO(ThisClassName<bool>) \
template<> \
CT_TYPE_IMPL_INIT_MACRO(ThisClassName<float>) \
template<> \
CT_TYPE_IMPL_INIT_MACRO(ThisClassName<double>) \
template<> \
CT_TYPE_IMPL_INIT_MACRO(ThisClassName<long>) \
template<> \
CT_TYPE_IMPL_INIT_MACRO(ThisClassName<unsigned long>) \
template<> \
CT_TYPE_IMPL_INIT_MACRO(ThisClassName<qint8>) \
template<> \
CT_TYPE_IMPL_INIT_MACRO(ThisClassName<quint8>) \
template<> \
CT_TYPE_IMPL_INIT_MACRO(ThisClassName<qint16>) \
template<> \
CT_TYPE_IMPL_INIT_MACRO(ThisClassName<quint16>) \
template<> \
CT_TYPE_IMPL_INIT_MACRO(ThisClassName<qint32>) \
template<> \
CT_TYPE_IMPL_INIT_MACRO(ThisClassName<quint32>) \
template<> \
CT_TYPE_IMPL_INIT_MACRO(ThisClassName<qint64>) \
template<> \
CT_TYPE_IMPL_INIT_MACRO(ThisClassName<quint64>)

/**
  * Use this define in your .cpp. BUT if you use CT_DEFAULT_IA_INIT_TEMPLATED_OPENCV in your .hpp it is not necessary to call this define too.
  */
#define CT_TYPE_TEMPLATED_IMPL_OPENCV_INIT_MACRO(ThisClassName) \
template<> \
CT_TYPE_IMPL_INIT_MACRO(ThisClassName<bool>) \
template<> \
CT_TYPE_IMPL_INIT_MACRO(ThisClassName<float>) \
template<> \
CT_TYPE_IMPL_INIT_MACRO(ThisClassName<double>) \
template<> \
CT_TYPE_IMPL_INIT_MACRO(ThisClassName<qint8>) \
template<> \
CT_TYPE_IMPL_INIT_MACRO(ThisClassName<quint8>) \
template<> \
CT_TYPE_IMPL_INIT_MACRO(ThisClassName<qint16>) \
template<> \
CT_TYPE_IMPL_INIT_MACRO(ThisClassName<quint16>) \
template<> \
CT_TYPE_IMPL_INIT_MACRO(ThisClassName<qint32>) \


/**
  * @brief Use this define in your class to add the copy method without write your own.
  * @warning You must define the copy contructor in your class !
  */
#define CT_ITEM_COPY_IMP(argClass) CT_AbstractItemDrawable* copy(const CT_OutAbstractItemModel* newModelToUse = nullptr, const CT_AbstractResult* newResult = nullptr) const override \
                                     { \
                                        argClass* cpy = new argClass(*this); \
                                        cpy->setModel(newModelToUse); \
                                        cpy->setResult(newResult); \
                                        return cpy; \
                                     }

/**
 * @brief Represent a item that can be added in a result or in another item
 */
class CTLIBSTRUCTURE_EXPORT CT_AbstractItemDrawable : public PrototypeModelInterfaceInheritTool<CT_AbstractItem, IItemDrawableForModel>
{
    Q_OBJECT
    using SuperClass = PrototypeModelInterfaceInheritTool<CT_AbstractItem, IItemDrawableForModel>;

public:
    CT_AbstractItemDrawable();

    /**
     * @brief Copy constructor.
     *
     *        What is copied :
     *          - Pointer of the result and model of the original item.
     *          - Unique ID
     *          - Pointer of base and alternative draw manager
     *          - Displayable name
     *
     *        What is initialized differently :
     *          - Parent is set to nullptr
     *          - isSelected and isDisplayed is set to false
     *          - Document list is not copied
     */
    CT_AbstractItemDrawable(const CT_AbstractItemDrawable& other);
    ~CT_AbstractItemDrawable() override;

    /**
      * @warning DON'T FORGET TO REDEFINE THIS METHOD IN YOUR ITEM !!!
      *
      * @brief Redefine this method by using CT_TYPE_IMPL_MACRO (show example on top of this file)
      */
    virtual QString type() const = 0;

    /**
      * @warning DON'T FORGET TO DEFINE THIS METHOD IN YOUR ITEM !!!
      *
      * @brief Redefine this method by using CT_TYPE_IMPL_MACRO (show example on top of this file)
      */
    static QString staticType();

    /**
     * @brief Set the id, per example if you want that the copy has the same id than the original
     * @warning Not intended for direct use by plugin developper
     */
    void setId(quint32 id);

    /**
     * @brief Returns the unique ID of this item
     */
    quint32 id() const {return m_id;}

    /**
     * @brief Returns the name of this item (the name of the class). By default call "metaObject()->className()". You can
     *        overload this method if you want to show another name
     */
    virtual QString name() const;

    /**
     * @brief Returns the name of this item (the name of the class). By default call "staticMetaObject.className()". You can
     *        redefine this method if you want to show another name
     */
    static QString staticName();

    /**
     * @brief Returns a displayable name. Use the method "setDisplayableName" to change them. By default if the displayable
     *        name is not empty return it. Otherwise if the displable name of the model is not empty return it. Otherwise
     *        return the result of the method "name".
     */
    QString displayableName() const;

    /**
     * @brief Change the displayable name of this item. Name used in GUI.
     */
    void setDisplayableName(const QString &displayableName);

    /**
     * @brief Returns information of this item that can be displayed in a tooltip per example
     */
    virtual QString info();

    /**
     * @brief Set the parent item
     * @warning Not intended for direct use by plugin developper
     */
    void setParentItem(const CT_AbstractItemDrawable *parent);

    /**
     * @brief Returns the parent item
     */
    CT_AbstractItemDrawable* parentItem() const;

    /**
     * @brief Visit all documents where this item is
     * @param visitor : the visitor to use. It must return true if he want to continue the visit or false to cancel it.
     * @return Returns true if no document has been visited otherwise returns the result of the visitor.
     */
    template<typename Visitor>
    bool visitDocumentsWhereThisItemIs(const Visitor& visitor) const {
        for(const IDocumentForModel* doc : m_documentsWhereThisItemIs) {
            if(!visitor(doc))
                return false;
        }

        return true;
    }

    /**
     * @brief Returns true if item is in the specified document
     */
    bool isInDocument(const IDocumentForModel* doc) const;

    /**
     * @brief Called from GUI to inform this item that he was added to the specified document
     * @return false if the item is already in this document
     * @warning Not intended for direct use by plugin developper
     */
    virtual bool addDocumentParent(const IDocumentForModel* doc);

    /**
     * @brief Called from GUI to inform this item that he will be removed from specified document
     * @warning Not intended for direct use by plugin developper
     */
    virtual void aboutToBeRemovedFromDocument(const IDocumentForModel*) { }

    /**
     * @brief Called from GUI to inform this item that he was removed from the specified document
     * @warning Not intended for direct use by plugin developper
     */
    virtual void removeDocumentParent(const IDocumentForModel* doc);

    /**
     * @brief Set this item selected or not in all documents.
     */
    void setSelected(const bool& value);

    /**
     * @brief Returns true if this item is selected.
     */
    bool isSelected() const;

    /**
     * @brief Call this method when the display state of this item change
     * @warning Not intended for direct use by plugin developper
     */
    void setDisplayed(const bool& value);

    /**
     * @brief Returns true it this item is displayed at least in one document
     */
    bool isDisplayed() const;

    /**
     * @brief Returns the x center coordinate of this item
     */
    virtual double centerX() const = 0;

    /**
     * @brief Returns the y center coordinate of this item
     */
    virtual double centerY() const = 0;

    /**
     * @brief Returns the z center coordinate of this item
     */
    virtual double centerZ() const = 0;

    /**
     * @brief Returns true if this item has a bounding box. By default returns false.
     *        Overload this method in your item if you want to return a bounding box.
     */
    virtual bool hasBoundingBox() const {return false;}

    /**
     * @brief Returns true if this item has a bounding box and it seems to be valid.
     */
    virtual bool doesBoundingBoxAppearToBeValid() const;

    /**
     * @brief Returns the bounding box of this item. Use method "hasBoundingBox" to konw if this item has a bounding box.
     *        Overload this method in your item if you want to return a bounding box.
     *
     * @param min : min point to be modified
     * @param max : max point to be modified
     */
    virtual void boundingBox(Eigen::Vector3d &min, Eigen::Vector3d &max) const;

    /**
     * @brief Set the draw manager that must draw the item. Typically create a static draw manager in your item class
     *        and call this method in your constructor, example :
     *
     *        setBaseDrawManager(&MY_STATIC_BASE_DRAW_MANAGER);
     */
    void setBaseDrawManager(const CT_AbstractItemDrawableDrawManager *drawManager);

    /**
     * @brief Returns the base draw manager
     */
    const CT_AbstractItemDrawableDrawManager* baseDrawManager() const;

    /**
     * @brief If you want to set an alternative draw manager from your step (per example) you can do that with
     *        this method. If the alternative draw manager is != nullptr it will be called in method draw.
     */
    void setAlternativeDrawManager(const CT_AbstractItemDrawableDrawManager *drawManager);

    /**
     * @brief Returns the alternative draw manager
     */
    const CT_AbstractItemDrawableDrawManager* alternativeDrawManager() const;

    /**
     * @brief Call the method draw of the alternative draw manager if exist otherwise call the method
     *        draw of the base draw manager if exist otherwise do nothing.
     * @param view : the view where the item will be draw
     * @param painter : painter used to draw elements
     */
    virtual void draw(GraphicsViewInterface& view, PainterInterface& painter);

    /**
     * @brief Returns the configuration of the base draw manager or the alternative if it was not nullptr. nullptr if no draw manager was set.
     */
    CT_ItemDrawableConfiguration* drawConfiguration();

    /**
     * @brief Returns for all differents children contained in the sub-hierarchy the list of draw manager its configuration
     */
    QList<CT_ItemDrawableConfiguration*> dependantDrawConfigurations() const;

    /**
     * @brief Visit default attributes of this class and class that inherit from it
     */
    bool visitDefaultAttributes(const IItemDrawableToolForModel::ItemAttributesVisitor& visitor) const;

    using ItemVisitor = std::function<bool(const CT_AbstractItemDrawable* child)>;

    /**
     * @brief Visit childrens of type CT_AbstractItemDrawable
     * @return Returns true if no childrens has been visited otherwise returns the result of the visitor.
     */
    virtual bool visitChildrensOfTypeItem(const ItemVisitor& visitor) const = 0;

    /**
     * @brief Recursively visit childrens of type CT_AbstractItemDrawable
     * @return Returns true if no childrens has been visited otherwise returns the result of the visitor.
     */
    bool recursiveVisitChildrensOfTypeItem(const ItemVisitor& visitor) const;

    /**
     * @brief Returns the tool for model
     */
    IItemDrawableToolForModel* itemToolForModel() const override;

    /**
     * @brief Call the copy contructor and set the new model and the new result if it can be changed !
     * @param newModelToUse : new model to use if if can be changed
     * @param newResult : new result to use if it can be changed
     * @return The copy of this item (see the copy constructor to now what is copied and what is initialized differently)
     */
    virtual CT_AbstractItemDrawable* copy(const CT_OutAbstractItemModel* newModelToUse = nullptr, const CT_AbstractResult* newResult = nullptr) const = 0;

    template<class T>
    T* copyAs(const CT_OutAbstractItemModel* newModelToUse = nullptr, const CT_AbstractResult* newResult = nullptr) const {
        return static_cast<T*>(this->copy(newModelToUse, newResult));
    }

    // TODO : MK 21.11.18 refactor this and check with Alexandre Piboule what it means
    static void addNameTypeCorresp(QString type, QString name);
    static QString nameFromType(QString type);

signals:
    /**
     * @brief Emitted when the selection state change
     */
    void selectChange(bool value, CT_AbstractItemDrawable* item = nullptr);

    /**
     * @brief Emitted when the display state change
     */
    void displayChange(bool value, CT_AbstractItemDrawable* item = nullptr);

private:
    class ItemToolForModel : public IItemDrawableToolForModel {
    public:
        CT_AbstractItemDrawable* m_pointer;

        QString displayableName() const override { return m_pointer->displayableName(); }

        QString nameFromType() const override { return m_pointer->nameFromType(m_pointer->type()); }

        QString itemInheritPath() const override { return m_pointer->type(); }

        bool visitItemHisDefaultAttributes(const ItemAttributesVisitor& visitor) const override {
            return m_pointer->visitDefaultAttributes(visitor);
        }

        IItemDrawableForModel* copyItem() const override {
            return m_pointer->copy(nullptr, nullptr);
        }
    };

    /**
     * @brief The tool for model
     */
    ItemToolForModel                    m_itemTool;

    /**
     * @brief A unique index
     */
    quint32                             m_id;

    /**
     * @brief A collection that contains all documents where this item is
     */
    std::set<IDocumentForModel*>        m_documentsWhereThisItemIs;

    /**
     * @brief To true if this item is selected
     */
    bool                                m_isSelected;

    /**
     * @brief To true if this item is displayed at least in one document
     */
    bool                                m_isDisplayedInADocument;

    /**
     * @brief The displayable name
     */
    QString                             m_displayableName;

    /**
     * @brief The parent item (a group, a singular item, etc...)
     */
    CT_AbstractItemDrawable*            m_parentItem;

    /**
     * @brief The default draw manager to use if the alternative is nullptr
     */
    CT_AbstractItemDrawableDrawManager* m_baseDrawManager;

    /**
     * @brief The alternative draw manager to use if is not nullptr
     */
    CT_AbstractItemDrawableDrawManager* m_alternativeDrawManager;

    static quint32&  NEXTID();

    // TODO : MK 21.11.18 refactor this and check with Alexandre Piboule what it means
    static QMap<QString, QString>& NAMEMAP();
    static QMutex& NAMEMAP_Mutex();

protected:

    template<typename InHandleType, typename Visitor>
    bool visitInModelWithPossibilitiesFromInHandle(const InHandleType& inHandle, const Visitor& visitor) const {
        const CT_InAbstractModel* inOriginalModel = inHandle.model();
        const CT_InAbstractResultModel* inOriginalResultModel = dynamic_cast<CT_InAbstractResultModel*>(inOriginalModel->rootModel());

        Q_ASSERT(inOriginalResultModel != nullptr);
        if (inOriginalResultModel == nullptr) {qDebug() << "CT_AbstractItemDrawable::visitInModelWithPossibilitiesFromInHandle" << ", " << "inOriginalResultModel == nullptr";}

        const int nResultPossibility = inOriginalResultModel->nPossibilitySelected();

        for(int i=0; i<nResultPossibility; ++i)
        {
            CT_InStdResultModelPossibility* resultP = static_cast<CT_InStdResultModelPossibility*>(inOriginalResultModel->possibilitySelectedAt(i));

            CT_InAbstractModel* inModelWithPossibilities = resultP->inResultModel()->recursiveSearchTheModelThatWasACopiedModelFromThisOriginalModel(inOriginalModel);

            if(inModelWithPossibilities != nullptr)
            {
                if(!visitor(inModelWithPossibilities))
                    return false;
            }
        }

        return true;
    }
};

#endif // CT_ABSTRACTITEMDRAWABLE_H
