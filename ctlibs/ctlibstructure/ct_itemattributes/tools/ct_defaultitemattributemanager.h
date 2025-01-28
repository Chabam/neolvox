#ifndef CT_DEFAULTITEMATTRIBUTEMANAGER_H
#define CT_DEFAULTITEMATTRIBUTEMANAGER_H

#include "ct_global/ct_structurecontext.h"

#include "ct_itemattributes/ct_stditemattributewrappert.h"
#include "ct_itemattributes/ct_stditemattributet.h"

#include "ct_itemdrawable/abstract/ct_abstractitemdrawable.h"

#include "ct_model/outModel/ct_outstditemattributemodel.h"
#include "ct_model/inModel/ct_instditemattributemodel.h"

#include "ct_categories/tools/ct_categorymanager.h"

#include "ct_tools/ct_staticmethodinvoker.h"

#include <typeinfo>
#include <QHash>

/**
 * @brief Call CT_DEFAULT_IA_BEGIN in the private section of your singular item class BEFORE the use of method "CT_DEFAULT_IA_Vxxx"
 *        Call CT_DEFAULT_IA_END in the  private section of your singular item class AFTER the use of method "CT_DEFAULT_IA_Vxxx"
 *        Call CT_DEFAULT_IA_INIT in the cpp or hpp file
 *
 * @param ClassNameSI : your class name
 *
 * @example : private:
 *            CT_DEFAULT_IA_BEGIN(CT_Cylinder)
 *            CT_DEFAULT_IA_V2(CT_Cylinder, CT_AbstractCategory::staticInitDataAngle(), &CT_Cylinder::getAngle, QObject::tr("Angle"))
 *            CT_DEFAULT_IA_V2(CT_Cylinder, CT_AbstractCategory::staticInitDatR2(), &CT_Cylinder::getR2, QObject::tr("R²"))
 *            CT_DEFAULT_IA_END(CT_Cylinder)
 *
 *            cpp file:
 *
 *            CT_DEFAULT_IA_INIT(CT_Cylinder)
 *
 *            -----~ OR IF YOUR CLASS IS TEMPLATED ~------
 *
 *            CT_USE_DEFAULT_IA(CT_MetricT<T>)
 *            CT_DEFAULT_IA_V2(CT_MetricT<T>, CT_AbstractCategory::staticInitDataValue(), &CT_MetricT<T>::getValue, QObject::tr("Value"))
 *
 *            cpp file:
 *
 *            template<> CT_DEFAULT_IA_INIT(CT_MetricT<double>)
 */
#define CT_DEFAULT_IA_BEGIN(ClassNameSI) \
private: \
    static void staticInitDefaultItemAttributes() {

#define CT_DEFAULT_IA_END(ClassNameSI) \
    } \
    static const CT_StaticMethodInvoker INVOKER_DEFAULT_IA; \

/**
 * @brief Call CT_DEFAULT_IA_V2 in the private section of your singular item class
 * @param ClassNameSI : your class name
 * @param CategoryUniqueName : a category unique name (typically CT_AbstractCategory::staticInitDataXXXX())
 * @param GetterMethod_OR_Value : a pointer to a getter method of your class OR a value
 * @param DisplayableName : a displayable name for your default attribute
 *
 * @example : private:
 *            CT_DEFAULT_IA_BEGIN(CT_Cylinder)
 *            CT_DEFAULT_IA_V2(CT_Cylinder, CT_AbstractCategory::staticInitDataAngle(), &CT_Cylinder::getAngle, QObject::tr("Angle"))
 *            CT_DEFAULT_IA_END(CT_Cylinder)
 */
#define CT_DEFAULT_IA_V2(ClassNameSI, CategoryUniqueName, GetterMethod_OR_Value, DisplayableName) \
    PS_DIAM->addItemAttribute<ClassNameSI>(CategoryUniqueName, GetterMethod_OR_Value, DisplayableName);

/**
 * @brief Call CT_DEFAULT_IA_INIT in the top of your source file (.cpp) and pass the name of your singular item class
 *        between parenthesys.
 *
 * @example : CT_DEFAULT_IA_INIT(CT_Cylinder)
 */
#define CT_DEFAULT_IA_INIT(ClassNameSI) \
    const CT_StaticMethodInvoker ClassNameSI::INVOKER_DEFAULT_IA = CT_StaticMethodInvoker(&ClassNameSI::staticInitDefaultItemAttributes); \
    CT_TYPE_IMPL_INIT_MACRO(ClassNameSI)

/**
 * @brief Call CT_DEFAULT_IA_INIT_TEMPLATED in the top of your source file (.cpp) and pass the name of your singular item class
 *        between parenthesys.
 *
 *        In case of your class is templated use the macro "CT_DEFAULT_IA_INIT_TEMPLATED_VALUES"
 *        In case of your class is templated and use an OpenCV Mat type use the macro "CT_DEFAULT_IA_INIT_TEMPLATED_OPENCV"
 *
 * @example : CT_DEFAULT_IA_INIT(CT_Cylinder)
 */
#define CT_DEFAULT_IA_INIT_TEMPLATED(ClassNameSI) \
    template<> const CT_StaticMethodInvoker ClassNameSI::INVOKER_DEFAULT_IA = CT_StaticMethodInvoker(&ClassNameSI::staticInitDefaultItemAttributes); \
    template<> CT_TYPE_IMPL_INIT_MACRO(ClassNameSI)

/**
 * @brief Call CT_DEFAULT_IA_INIT_TEMPLATED_VALUES in the top of your source file (.cpp) and pass the name of your singular
 *        item templated class between parenthesys.
 *
 *        In case of your class is templated and use an OpenCV Mat type use the macro "CT_DEFAULT_IA_INIT_TEMPLATED_OPENCV"
 *
 * @example : CT_DEFAULT_IA_INIT_TEMPLATED_VALUES(CT_Grid)
 */
#define CT_DEFAULT_IA_INIT_TEMPLATED_VALUES(ClassNameSI) \
    CT_DEFAULT_IA_INIT_TEMPLATED(ClassNameSI<bool>) \
    CT_DEFAULT_IA_INIT_TEMPLATED(ClassNameSI<float>) \
    CT_DEFAULT_IA_INIT_TEMPLATED(ClassNameSI<double>) \
    CT_DEFAULT_IA_INIT_TEMPLATED(ClassNameSI<long>) \
    CT_DEFAULT_IA_INIT_TEMPLATED(ClassNameSI<unsigned long>) \
    CT_DEFAULT_IA_INIT_TEMPLATED(ClassNameSI<qint8>) \
    CT_DEFAULT_IA_INIT_TEMPLATED(ClassNameSI<quint8>) \
    CT_DEFAULT_IA_INIT_TEMPLATED(ClassNameSI<qint16>) \
    CT_DEFAULT_IA_INIT_TEMPLATED(ClassNameSI<quint16>) \
    CT_DEFAULT_IA_INIT_TEMPLATED(ClassNameSI<qint32>) \
    CT_DEFAULT_IA_INIT_TEMPLATED(ClassNameSI<quint32>) \
    CT_DEFAULT_IA_INIT_TEMPLATED(ClassNameSI<qint64>) \
    CT_DEFAULT_IA_INIT_TEMPLATED(ClassNameSI<quint64>)

/**
 * @brief Call CT_DEFAULT_IA_INIT_TEMPLATED_VALUES in the top of your source file (.cpp) and pass the name of your singular
 *        item templated class between parenthesys.
 *
 *        In case of your class is templated and use an OpenCV Mat type use the macro "CT_DEFAULT_IA_INIT_TEMPLATED_OPENCV"
 *
 * @example : CT_DEFAULT_IA_INIT_TEMPLATED_OPENCV(CT_Grid)
 */
#define CT_DEFAULT_IA_INIT_TEMPLATED_OPENCV(ClassNameSI) \
    CT_DEFAULT_IA_INIT_TEMPLATED(ClassNameSI<bool>) \
    CT_DEFAULT_IA_INIT_TEMPLATED(ClassNameSI<float>) \
    CT_DEFAULT_IA_INIT_TEMPLATED(ClassNameSI<double>) \
    CT_DEFAULT_IA_INIT_TEMPLATED(ClassNameSI<qint8>) \
    CT_DEFAULT_IA_INIT_TEMPLATED(ClassNameSI<quint8>) \
    CT_DEFAULT_IA_INIT_TEMPLATED(ClassNameSI<qint16>) \
    CT_DEFAULT_IA_INIT_TEMPLATED(ClassNameSI<quint16>) \
    CT_DEFAULT_IA_INIT_TEMPLATED(ClassNameSI<qint32>)

/**
 * @brief A class tools used by the CT_Context to register all default item attribute defined
 *        in a singular item by macro CT_REGISTER_DEFAULT_IA
 */
class CTLIBSTRUCTURE_EXPORT CT_DefaultItemAttributeManager
{
public:
    using ItemAttributesVisitor = std::function<bool(const CT_AbstractItemAttribute*)>;
    using ItemAttributeModelsVisitor = std::function<bool(const CT_OutAbstractItemAttributeModel*)>;

    CT_DefaultItemAttributeManager();
    ~CT_DefaultItemAttributeManager();

    /**
     * @brief Add an item attribute to this manager.
     * @param categoryUniqueName : the unique name to get the category in category manager
     * @param getter : a method to your singular item to get the data
     * @param displayableName : a displayable name for the model of the item attribute (displayed in GUI). If empty we will use the displayable name of the category
     */
    template <typename ItemClass, typename VType>
    void addItemAttribute(const QString& categoryUniqueName,
                          VType (ItemClass::*getter)() const,
                          const QString& displayableName = "")
    {
        static_assert(CT_AbstractCategory::staticValueTypeToCategoryType<VType>() != CT_AbstractCategory::UNKNOWN, "Error !");

        const CT_AbstractCategory* cat = staticInternalGetCategoryFromUniqueName(categoryUniqueName);

        using PrototypeType = CT_StdItemAttributeWrapperT<ItemClass, VType>;

        // Create the prototype
        PrototypeType* prototype = new PrototypeType(cat, getter);

        // Add it to the collection
        internalAddItemAttribute<ItemClass>(displayableName, prototype);
    }

    /**
     * @brief Add an item attribute to this manager.
     * @param categoryUniqueName : the unique name to get the category in category manager
     * @param data : the data that will never change
     * @param displayableName : a displayable name for the model of the item attribute (displayed in GUI). If empty we will use the displayable name of the category
     * @return true if the item attribute is created and added to this manager
     */
    template <typename ItemClass, typename VType>
    void addItemAttribute(const QString& categoryUniqueName,
                          const VType& data,
                          const QString& displayableName = "")
    {
        static_assert((CT_AbstractCategory::staticValueTypeToCategoryType<VType>() != CT_AbstractCategory::UNKNOWN) && std::is_scalar<VType>::value,
                      "You must pass an integral (constant value) ! If you pass "
                      "a member function of your class be careful that it was not a "
                      "function of a base class. If so, you must create a member function "
                      "in your class that call the function of the base class.");

        const CT_AbstractCategory* cat = staticInternalGetCategoryFromUniqueName(categoryUniqueName);

        using PrototypeType = CT_StdItemAttributeT<VType>;

        // Create the prototype
        PrototypeType* prototype = new PrototypeType(cat, data);

        // Add it to the collection
        internalAddItemAttribute<ItemClass>(displayableName, prototype);
    }

    /**
     * @brief Call it when you want to delete from this manager and from memory all default attributes
     *        added from a specific object
     * @param object : the object to use to visit item drawable registered. The object must have a method
     *                 visitRegisteredItemDrawable that accept a visitor that returns a bool and receive
     *                 a const pointer to a CT_AbstractItemDrawable.
     */
    template<typename ClassType>
    void clearDefaultAttributesFrom(const ClassType* object) {
        DefaultItemAttributesByItemCollection& thisCollection = m_collection;

        const auto visitor = [&thisCollection](const CT_AbstractItemDrawable* item) -> bool {

            const QString type = item->type();
            const int index = type.lastIndexOf("/");

            if(index != -1)
                delete thisCollection.take(type.right(type.size()-index));

            return true;
        };

        object->visitRegisteredItemDrawable(visitor);
    }

    /**
     * @brief Call it to visit all default item attributes that was added by a specific type of item
     *        and classes that he inherit.
     * @param type : Pass the result of the method "getType" of your item
     * @param visitor : the visitor to use
     * @return True if no item attribute has been visited, otherwise return the result of the visitor.
     */
    bool visitItemAttributesForType(const QString& type, const ItemAttributesVisitor& visitor) const;

    /**
     * @brief Call it to visit all default item attribute model that was added by a specific type of item
     *        and classes that he inherit.
     * @param type : Pass the result of the method "getType" of your item
     * @param visitor : the visitor to use
     * @return True if no item attribute has been visited, otherwise return the result of the visitor.
     */
    bool visitItemAttributeModelsForType(const QString& type, const ItemAttributeModelsVisitor& visitor) const;

    /**
     * @brief Call it to visit all default item attribute that was added by a specific type of item
     *        and classes that he inherit.
     * @param itemType : Pass the result of the method "getType" of your item
     * @param inModel : the input model to use to visit selected possibilities to get output models
     * @param visitor : the visitor to use
     * @return True if no item attribute has been visited, otherwise return the result of the visitor.
     */
    bool visitItemAttributesForTypeInSelectedPossibilitiesOfInModel(const QString& itemType, const CT_InAbstractItemAttributeModel* inModel, const ItemAttributesVisitor& visitor) const;

    /**
     * @brief Returns the number or item attributes for the specified item type
     */
    int nItemAttributesForType(const QString& itemType) const;

    /**
     * @brief Returns the list of item attributes that was added by a specific type of item and classes that he inherit.
     * @return An empty list if type has no default item attributes
     */
    QList<CT_AbstractItemAttribute*> itemAttributes(const QString& itemType) const;

    /**
     * @brief Returns the item attribute corresponding to the specified output model that was added by a specific type of item and classes that he inherit.
     * @return nullptr if type has no default item attributes or has no default attributes with this model
     */
    CT_AbstractItemAttribute* itemAttributeFromOutModel(const CT_OutAbstractItemAttributeModel* outModel, const QString& itemType) const;

    /**
     * @brief Returns a list of item attributes corresponding to selected possibilities of specified input model that was added by a specific type of item and classes that he inherit.
     * @return nullptr if type has no default item attributes or has no default attributes with this model
     */
    QList<CT_AbstractItemAttribute*> itemAttributesFromInModel(const CT_InAbstractItemAttributeModel* inModel, const QString& itemType) const;

    /**
     * @brief Returns the item attribute corresponding to the first selected possibility of specified input model that was added by a specific type of item and classes that he inherit.
     * @return nullptr if type has no default item attributes or has no default attributes with this model
     */
    CT_AbstractItemAttribute* firstItemAttributeFromInModel(const CT_InAbstractItemAttributeModel* inModel, const QString& itemType) const;

private:
    using DefaultItemAttributesCollection = QList<CT_OutAbstractItemAttributeModel*>;
    using DefaultItemAttributesByItemCollection = QHash<QString, DefaultItemAttributesCollection* >;

    /**
     * @brief A QHash with a QString for key (name of the class of the item) and
     *        a DefaultItemAttributesCollection for value.
     */
    DefaultItemAttributesByItemCollection    m_collection;

    /**
     * @brief Returns the category with the specified unique name from the default category manager. In debug
     *        mode check that the category is not null and if not print an assert message.
     */
    static const CT_AbstractCategory* staticInternalGetCategoryFromUniqueName(const QString& categoryUniqueName) {
        const CT_AbstractCategory* cat = PS_CATEGORY_MANAGER->findByUniqueName(categoryUniqueName);

        Q_ASSERT_X(cat != nullptr, "CT_DefaultItemAttributeManager::addItemAttribute", qPrintable(QString("You created a default item attribute but the category with name \"") + categoryUniqueName + "\" was not registered !"));
        if (cat == nullptr) {qDebug() << "CT_DefaultItemAttributeManager::staticInternalGetCategoryFromUniqueName" << ", " << "cat == nullptr";}

        return cat;
    }

    /**
     * @brief Create the item attribute model and .
     */
    template <typename ItemClass, typename PrototypeType>
    void internalAddItemAttribute(const QString& displayableName,
                                  PrototypeType* prototype) {
        const CT_AbstractCategory* cat = prototype->template categoryStaticT<CT_AbstractCategory>();

        // create the model
        CT_OutStdItemAttributeModel<PrototypeType>* model = new CT_OutStdItemAttributeModel<PrototypeType>(cat, displayableName.isEmpty() ? cat->displayableName() : displayableName, cat->description(), "", prototype);
        model->setAsDefaultItemAttributeModel();
        prototype->setModel(model);

        const QString classStaticType = ItemClass::staticType();

        QStringList classNameSplit = classStaticType.split("/");
        const QString useClassName = (classNameSplit.size() > 1) ? classNameSplit.last() : classStaticType;

        DefaultItemAttributesCollection* newL = m_collection.value(useClassName);

        if(newL == nullptr)
        {
            newL = new DefaultItemAttributesCollection();
            m_collection.insert(useClassName, newL);
        }

        newL->append(model);
    }
};

#endif // CT_DEFAULTITEMATTRIBUTEMANAGER_H
