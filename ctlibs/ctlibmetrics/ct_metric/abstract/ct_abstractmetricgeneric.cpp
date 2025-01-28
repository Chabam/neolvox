#include "ct_abstractmetricgeneric.h"

CT_AbstractMetricGeneric::~CT_AbstractMetricGeneric()
{
    qDeleteAll(m_attributesVaB.begin(), m_attributesVaB.end());
}

CT_AbstractVaB* CT_AbstractMetricGeneric::findAttributeValueVaBFromDisplayableName(const QString &dName) const
{
    AttributesContainerIterator it(m_attributesVaB);

    while(it.hasNext()) {
        it.next();

        if(it.value()->_displayableName == dName)
            return static_cast<CT_AbstractVaB*>(it.key());
    }

    return nullptr;
}

void CT_AbstractMetricGeneric::saveSettings(SettingsWriterInterface &writer) const
{
    SuperClass::saveSettings(writer);

    QListIterator<CT_AbstractVaB*> it(m_vab);

    while(it.hasNext()) {
        CT_AbstractVaB* vab = it.next();
        const int pID = writer.addParameter(this, "VaB", m_attributesVaB.value(vab)->_displayableName);
        writer.addParameterInfo(this, pID, "used", vab->used);
    }
}

bool CT_AbstractMetricGeneric::restoreSettings(SettingsReaderInterface &reader)
{
    if(!SuperClass::restoreSettings(reader))
        return false;

    QListIterator<CT_AbstractVaB*> it(m_vab);

    while(it.hasNext())
        it.next()->used = false;

    QVariant value;

    const int n = reader.parameterCount(this, "VaB");

    for(int i=0; i<n; ++i) {
        if(const int pID = reader.parameter(this, "VaB", value)) {

            const QString vabDisplayableName = value.toString();

            CT_AbstractVaB* vab = findAttributeValueVaBFromDisplayableName(vabDisplayableName);

            if(vab != nullptr) {
                if(reader.parameterInfo(this, pID, "used", value))
                    vab->used = value.toBool();
            }
        }
    }

    return true;
}

CT_AbstractConfigurableWidget* CT_AbstractMetricGeneric::createConfigurationWidget()
{
    if(m_vab.isEmpty())
        return nullptr;

    CT_GenericConfigurableWidget *wid = new CT_GenericConfigurableWidget();
    addAllVaBToWidget(wid);

    return wid;
}

void CT_AbstractMetricGeneric::addAllVaBToWidget(CT_GenericConfigurableWidget* wid) const
{
    QListIterator<CT_AbstractVaB*> it(m_vab);

    while(it.hasNext()) {
        CT_AbstractVaB *vab = it.next();
        wid->addBool("", "", m_attributesVaB.value(vab, nullptr)->_displayableName, vab->used);
    }
}

void CT_AbstractMetricGeneric::createAttributes()
{
    QListIterator<CT_AbstractVaB*> it(m_vab);

    while(it.hasNext()) {
        CT_AbstractVaB *vab = it.next();

        if(vab->used)
            internalInsertAttributeObject(vab->valueToVoidPtr(), m_attributesVaB.value(vab, nullptr)->copy());
    }
}
