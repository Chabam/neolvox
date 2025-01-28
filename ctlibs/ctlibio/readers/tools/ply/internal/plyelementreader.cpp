#include "plyelementreader.h"

PlyElementReader::PlyElementReader(const PlyElement &el)
{
    m_element = el;
    m_listener = nullptr;
}

void PlyElementReader::setListener(const IPlyReaderListener *listener)
{
    m_listener = const_cast<IPlyReaderListener*>(listener);
}

bool PlyElementReader::addSetterForProperty(const int &propertyIndex,
                                            const int &channelIndex,
                                            const PlyAbstractValuesSetter* setter)
{
    SetterInfo info;
    info.setter = const_cast<PlyAbstractValuesSetter*>(setter);
    info.channelIndex = channelIndex;

    m_settersByProperty.insert(propertyIndex, info);
    m_uniqueSetters.insert(info.setter);

    return true;
}

void PlyElementReader::informSetterBeginningOfRead()
{
    QSetIterator<PlyAbstractValuesSetter*> it(m_uniqueSetters);

    while(it.hasNext())
        it.next()->beginReadElement();
}

void PlyElementReader::informSetterOneLineReaded()
{
    QSetIterator<PlyAbstractValuesSetter*> it(m_uniqueSetters);

    while(it.hasNext())
        it.next()->oneLineOfElementReaded();
}

void PlyElementReader::informSetterEndOfRead()
{
    QSetIterator<PlyAbstractValuesSetter*> it(m_uniqueSetters);

    while(it.hasNext())
        it.next()->endReadElement();
}

void PlyElementReader::passValueToSetter(const double &value,
                                         const int &propertyIndex,
                                         const SetterCollection::iterator &end)
{
    SetterCollection::iterator it = m_settersByProperty.find(propertyIndex);

    while ((it != end)
           && (it.key() == propertyIndex)) {

        const SetterInfo& info = it.value();
        info.setter->setValue(value, info.channelIndex);

        ++it;
    }
}

bool PlyElementReader::mustStop() const
{
    if(m_listener != nullptr)
        return m_listener->plyReadMustStop();

    return false;
}

void PlyElementReader::setProgress(int p)
{
    if(m_listener != nullptr)
        return m_listener->plyProgressChanged(p);
}
