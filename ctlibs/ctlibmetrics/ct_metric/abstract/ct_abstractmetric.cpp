#include "ct_abstractmetric.h"

#include "ct_log/ct_logmanager.h"

CT_AbstractMetric::CT_AbstractMetric(QString pluginName) : CT_AbstractConfigurableElement(pluginName)
{
}

CT_AbstractMetric::CT_AbstractMetric(const CT_AbstractMetric& other) : CT_AbstractConfigurableElement(other)
{
}

CT_AbstractMetric::~CT_AbstractMetric()
{
    clearAttributesRegistered();
}

QString CT_AbstractMetric::getDetailledDisplayableName() const
{
    QString result = getShortDisplayableName();

    if (m_attributes.size() > 0)
    {
        result += tr(" : ");

        AttributesContainerIterator it(m_attributes);

        while(it.hasNext())
        {
            AbstractAttributeObject *attributeObject = it.next().value();
            result.append(attributeObject->_displayableName);

            if(it.hasNext())
                result += ", ";
        }
    }
    return result;
}


void CT_AbstractMetric::finalizeConfiguration()
{
    clearAttributesRegistered();
    createAttributes();
}

void CT_AbstractMetric::declareOutputItemAttributeModels(CT_MetricOutModelStructureManager& manager)
{
    AttributesContainerIterator it(m_attributes);

    while(it.hasNext())
    {
        AbstractAttributeObject* attributeObject = it.next().value();
        attributeObject->declareOutputItemAttributeModels(manager);
    }
}

void CT_AbstractMetric::computeMetric(CT_AbstractSingularItemDrawable *item)
{
    if((item == nullptr) || (item->result() == nullptr))
        return;

    computeMetric();

    AttributesContainerIterator it(m_attributes);

    while (it.hasNext())
    {
        AbstractAttributeObject* attributeObject = it.next().value();
        attributeObject->addItemAttributeInItem(item);
    }
}

void CT_AbstractMetric::saveSettings(SettingsWriterInterface &writer) const
{
    Q_UNUSED(writer)
}

bool CT_AbstractMetric::restoreSettings(SettingsReaderInterface &reader)
{
    Q_UNUSED(reader)

    return true;
}

void CT_AbstractMetric::clearAttributesRegistered()
{
    qDeleteAll(m_attributes.begin(), m_attributes.end());
    m_attributes.clear();
}

CT_AbstractMetric::AbstractAttributeObject* CT_AbstractMetric::getAttributeObject(void* classMember, CT_AbstractCategory::ValueType type)
{
    CT_AbstractMetric::AbstractAttributeObject *attributeObject = m_attributes.value(classMember, nullptr);

    if(attributeObject == nullptr)
    {
        PS_LOG->addErrorMessage(LogInterface::metric, tr("Impossible de trouver l'attribut recherché (métrique : %2) !").arg(getShortDisplayableName()));
        return nullptr;
    }

    if(attributeObject->valueType() != type)
    {
        PS_LOG->addErrorMessage(LogInterface::metric, tr("Mauvais type pour l'attribut recherché (%1 au lieu de %2) !").arg(CT_AbstractCategory::valueTypeToString(type)).arg(attributeObject->valueType()));
        return nullptr;
    }

    return attributeObject;
}

const CT_AbstractMetric::AttributesContainer &CT_AbstractMetric::attributes() const
{
    return m_attributes;
}

bool CT_AbstractMetric::internalInsertAttributeObject(void *key, CT_AbstractMetric::AbstractAttributeObject *obj)
{
    if(m_attributes.contains(key))
        return false;

    m_attributes.insert(key, obj);
    return true;
}

bool CT_AbstractMetric::removeAttribute(void *key)
{
    AttributesContainerMutableIterator it(m_attributes);

    while(it.hasNext()) {
        it.next();

        if(it.key() == key)
        {
            delete it.value();
            it.remove();

            return true;
        }
    }

    return false;
}
