#ifndef CT_ABSTRACTMETRIC_H
#define CT_ABSTRACTMETRIC_H

#include "ctlibmetrics_global.h"
#include "ct_element/abstract/ct_abstractconfigurableelement.h"
#include "ct_metric/tools/ct_metricoutmodelstructuremanager.h"
#include "ct_itemdrawable/abstract/ct_abstractsingularitemdrawable.h"
#include "ct_handle/ct_handleoutstditemattribute.h"

/**
 * @brief A metric can export only attributes (but multiple attributes of different type)
 */
class CTLIBMETRICS_EXPORT CT_AbstractMetric : public CT_AbstractConfigurableElement
{
    Q_OBJECT

public:
    CT_AbstractMetric(QString pluginName);
    CT_AbstractMetric(const CT_AbstractMetric &other);
    ~CT_AbstractMetric() override;

    /**
     * @brief Returns the name of this metric + name of all attributes declared
     */
    QString getDetailledDisplayableName() const override;

    /**
     * @brief Call this method before declare output models to clear and declare attributes
     */
    void finalizeConfiguration() override;

    /**
     * @brief Call it to declare all output item attribute models.
     */
    void declareOutputItemAttributeModels(CT_MetricOutModelStructureManager& manager);

    /**
     * @brief Call it to declare all output models in a group of your result.
     * @param manager : The manager of output models
     * @param hParentItem : The parent item where to add output models. It can be an input or an output handle
     *                       but must be of type CT_HandleXXXSingularItem
     */
    template<typename HandleItem>
    void declareOutputItemAttributeModelsInItem(CT_OutModelStructureManager& manager, HandleItem& hParentItem) {
        CT_MetricOutModelStructureManager rManager = CT_MetricOutModelStructureManager::createFromHandle(manager, hParentItem);
        this->declareOutputItemAttributeModels(rManager);
    }

    /**
     * @brief Call this method to compute metrics and add attributes to item passed in parameter
     */
    void computeMetric(CT_AbstractSingularItemDrawable* item);

    /**
     * @brief This method is called when the core will save settings of this object
     */
    void saveSettings(SettingsWriterInterface& writer) const override;

    /**
     * @brief This method is called when the core will restore settings of this object
     */
    bool restoreSettings(SettingsReaderInterface& reader) override;

protected:
    /**
     * @brief Abstract class that declare a method to create a attribute and
     *        save all necessary element to create the attribute later
     */
    class CTLIBMETRICS_EXPORT AbstractAttributeObject
    {
    public:
        AbstractAttributeObject()
        {
            _attributeInstance = nullptr;
        }

        AbstractAttributeObject(QString displayableName,
                                QString category)
        {
            _displayableName = displayableName;
            _categoryName = category;
            _attributeInstance = nullptr;
        }

        AbstractAttributeObject(const AbstractAttributeObject &other)
        {
            _displayableName = other._displayableName;
            _categoryName = other._categoryName;
            _attributeInstance = nullptr;
        }

        virtual ~AbstractAttributeObject() {
            delete _attributeInstance;
        }

        virtual CT_AbstractCategory::ValueType valueType() const = 0;

        virtual void declareOutputItemAttributeModels(CT_MetricOutModelStructureManager& manager) = 0;

        virtual void addItemAttributeInItem(CT_AbstractSingularItemDrawable* item) = 0;

        /**
         * @brief Returns a copy of this object
         */
        virtual AbstractAttributeObject* copy() const = 0;

        QString                                 _displayableName;
        QString                                 _categoryName;
        CT_AbstractItemAttribute*               _attributeInstance;
    };

    /**
     * @brief Class that inherit from AbstractAttributeObject but know the type of the attribute to create
     */
    template<typename VType>
    class AttributeObject : public AbstractAttributeObject
    {
    public:
        AttributeObject() : AbstractAttributeObject(), mHOutItemAttribute(nullptr) {}
        AttributeObject(QString displayableName, QString category) : AbstractAttributeObject(displayableName, category), mHOutItemAttribute(nullptr) {}
        AttributeObject(const AttributeObject &other) : AbstractAttributeObject(other), mHOutItemAttribute(nullptr) {}
        ~AttributeObject() override {
            delete mHOutItemAttribute;
        }

        CT_AbstractCategory::ValueType valueType() const final
        {
            return CT_AbstractCategory::staticValueTypeToCategoryType<VType>();
        }

        void declareOutputItemAttributeModels(CT_MetricOutModelStructureManager& manager) final
        {
            delete mHOutItemAttribute;
            mHOutItemAttribute = new CT_HandleOutStdItemAttribute<VType>();

            manager.addItemAttribute(*mHOutItemAttribute, _categoryName, _displayableName);
        }

        void addItemAttributeInItem(CT_AbstractSingularItemDrawable* item) final
        {
            if(_attributeInstance != nullptr) {
                item->addItemAttribute(*mHOutItemAttribute, _attributeInstance);
                _attributeInstance = nullptr;
            }
        }

        AbstractAttributeObject* copy() const override {
            return new AttributeObject<VType>(*this);
        }

        CT_HandleOutStdItemAttribute<VType>* mHOutItemAttribute;
    };

    typedef QMap<void*, CT_AbstractMetric::AbstractAttributeObject*>                    AttributesContainer;
    typedef QMapIterator<void*, CT_AbstractMetric::AbstractAttributeObject*>            AttributesContainerIterator;
    typedef QMutableMapIterator<void*, CT_AbstractMetric::AbstractAttributeObject*>     AttributesContainerMutableIterator;

    /**
     * @brief Redeclare this method in your class and call "addAttribute" method for each attribute you want to declare
     */
    virtual void createAttributes() = 0;

    /**
     * @brief Redeclare this method in your class to do computing of metrics and call method "setAttribute(...)" to set results
     */
    virtual void computeMetric() = 0;

    /**
     * @brief Declare a attribute that you will set in your compute method
     * @example : addAttribute(m_classMemberThatWillBeComputedInComputeMetrics, CT_AbstractCategory::DATA_DIMENSION, tr("my attribute that was nice"));
     * @param classMember : a variable member of your class that will be used in "computeMetrics" method.
     * @param categoryName : the name of the category of the attribute (example : CT_AbstractCategory::DATA_AREA)
     * @param displayableName : a displayable name for this attribute (displayed in gui), must not be empty !
     */
    template<typename VType>
    void addAttribute(VType &classMember, const QString &categoryName, const QString &displayableName) {
        if(!m_attributes.contains(static_cast<void*>(&classMember)))
            m_attributes.insert(static_cast<void*>(&classMember), new CT_AbstractMetric::AttributeObject<VType>(displayableName, categoryName));
    }

    /**
     * @brief Call setAttribute(....) in your "compute" method to set results of attributes declared with "addAttribute" method
     * @param classMemberAndValue : the variable member of your class that was filled with the good value and that you used in method "addAttribute"
     */
    template<typename VType>
    void setAttributeValue(VType &classMemberAndValue) {
        AbstractAttributeObject* attributeObject = getAttributeObject(static_cast<void*>(&classMemberAndValue), CT_AbstractCategory::staticValueTypeToCategoryType<VType>());

        if(attributeObject != nullptr)
        {
            delete attributeObject->_attributeInstance;
            attributeObject->_attributeInstance = new CT_StdItemAttributeT<VType>(attributeObject->_categoryName, classMemberAndValue);
        }
    }

    /**
     * @brief Clear all attributes declared
     */
    virtual void clearAttributesRegistered();

    /**
     * @brief Find and returns the attribute object  with a unique name and a type
     * @param classMember : the variable member of your class declared with method "addAttribute"
     * @param type : the type of the attribute (must be the same that when it was declared with method "addAttribute"
     * @return nullptr if attribute was not found or type is different
     */
    AbstractAttributeObject* getAttributeObject(void* classMember, CT_AbstractCategory::ValueType type);

    /**
     * @brief Returns attributes added in "createAttributes" method
     */
    const AttributesContainer& attributes() const;

private:
    AttributesContainer                 m_attributes;

protected:

    /**
     * @brief Insert a attribute object
     * @return false if key already exist
     */
    bool internalInsertAttributeObject(void *key, CT_AbstractMetric::AbstractAttributeObject *obj);

    /**
     * @brief Remove an attribute declared with method "addAttribute"
     */
    virtual bool removeAttribute(void* key);
};

#endif // CT_AbstractMetric_H
