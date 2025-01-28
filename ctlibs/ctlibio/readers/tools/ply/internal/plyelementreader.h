#ifndef PLYELEMENTREADER_H
#define PLYELEMENTREADER_H

#include "plyelement.h"
#include "readers/tools/ply/internal/setter/plyabstractvaluessetter.h"
#include "readers/tools/ply/iplyreaderlistener.h"
#include "plyreadertools.h"

#include <QHashIterator>
#include <QSet>
#include <QVector>
#include <QDebug>

/**
 * @brief This class can be used to read an entire element.
 */
class PlyElementReader {
public:
    PlyElementReader(const PlyElement& el);

    /**
     * @brief Set a listener
     */
    void setListener(const IPlyReaderListener* listener);

    /**
     * @brief Add a setter for a specified property.
     *
     *        When a property is readed the method "setValue" of the setter will be called
     *        with the channelIndex and the value. Per example setter->setValue(valueOfPropertyReaded, channelIndexOfPropertyReaded);
     *
     * @param propertyIndex : index of the property to read
     * @param channelIndex : index of the channel in the setter where to set the value of the readed property
     * @param setter : setter to use
     * @return false if one index is < 0 or setter is nullptr
     */
    bool addSetterForProperty(const int& propertyIndex,
                              const int& channelIndex,
                              const PlyAbstractValuesSetter* setter);

    /**
     * @bried Read the entire element and pass values to all setters added before.
     * @param stream : the streamer to use to get values
     * @warning The streamer must be positionned at the first property of the element !
     */
    template<typename Streamer>
    bool process(Streamer& stream) {
        setProgress(0);

        size_t size = m_element.getNumberOfElements();

        if(size == 0)
            return true;

        size_t current = 0;

        // if we don't have setters we don't have to read this element
        if(m_uniqueSetters.isEmpty()) {
            // if we can skip it (we can skip if we know how many bytes to jump)
            if(skipElement(stream))
                return true;

            QVector<PlyProperty> properties = m_element.getProperties().toVector();
            int nProperties = properties.size();
            int propertyIndex;

            // otherwise we read all property to skip this element
            while(current != size)
            {
                for(propertyIndex = 0; propertyIndex < nProperties; ++propertyIndex)
                    PlyReaderTools::staticReadProperty<Streamer>(stream, properties[propertyIndex]);

                ++current;
                setProgress(int((current*100)/size));
            }

            return true;
        }

        informSetterBeginningOfRead();

        QVector<PlyProperty> properties = m_element.getProperties().toVector();
        int nProperties = properties.size();
        int propertyIndex;
        double value;

        SetterCollection::iterator end = m_settersByProperty.end();

        while(current != size)
        {
            for(propertyIndex = 0; propertyIndex < nProperties; ++propertyIndex) {
                value = PlyReaderTools::staticReadProperty<Streamer>(stream, properties[propertyIndex]);

                Q_ASSERT(!qIsNaN(value));
                if (qIsNaN(value)) {qDebug() << "PlyElementReader::process" << ", " <<  "qIsNaN(value)"; return false;}

                passValueToSetter(value,
                                  propertyIndex,
                                  end);
            }

            informSetterOneLineReaded();

            ++current;
            setProgress(int((current*100)/size));
        }

        informSetterEndOfRead();

        return true;
    }

    /**
     * @brief Call it to skip the element.
     *
     *        Call "process" method is a best way because the method will check
     *        if no setters was set and if so the element will be skipped.
     *
     * @return false if this element cannot be skipped.
     */
    template<typename Streamer>
    bool skipElement(Streamer& stream) const
    {
        // compute the size to jump
        qint64 jump = 0;

        QList<PlyProperty> properties = m_element.getProperties();
        int nProperties = properties.size();

        for(int propertyIndex = 0; propertyIndex < nProperties; ++propertyIndex) {
            const PlyProperty& property = properties[propertyIndex];

            if(property.isASingleValue()) {
                const qint8 nBytes = PlyReaderTools::staticPropertySizeInByte(property);

                Q_ASSERT(nBytes != -1);
                if (nBytes == -1) {qDebug() << "PlyElementReader::skipElement" << ", " <<  "nBytes == -1"; return false;}

                jump += nBytes;
            } else {
                return false; // cannot skip a list because we don't know how many values is in the list
            }
        }

        jump *= m_element.getNumberOfElements();

        Q_ASSERT(stream.skipRawData(jump) == jump);
        if (stream.skipRawData(jump) != jump) {qDebug() << "PlyElementReader::skipElement" << ", " <<  "stream.skipRawData(jump) != jump"; return false;}

        return true;
    }

private:
    struct SetterInfo {
        PlyAbstractValuesSetter*    setter;
        int                         channelIndex;
    };

    typedef QMultiHash<int, SetterInfo>     SetterCollection;
    typedef QHashIterator<int, SetterInfo>  SetterCollectionIterator;

    PlyElement                      m_element;
    SetterCollection                m_settersByProperty;
    QSet<PlyAbstractValuesSetter*>  m_uniqueSetters;
    IPlyReaderListener*             m_listener;

    /**
     * @brief Call the appropriate method to all setter to inform them that the element will be readed
     */
    void informSetterBeginningOfRead();

    /**
     * @brief Call the appropriate method to all setter to inform them that one element (one line) has been readed
     */
    void informSetterOneLineReaded();

    /**
     * @brief Call the appropriate method to all setter to inform them that the element has been totally readed
     */
    void informSetterEndOfRead();

    /**
     * @brief Call to pass the readed value to all setters for the specified property
     * @param value : value to pass to setters
     * @param propertyIndex : index of the property readed
     * @param end : end of the "m_settersByProperty" collection
     */
    void passValueToSetter(const double& value,
                           const int& propertyIndex,
                           const SetterCollection::iterator& end);

    /**
     * @brief Returns true if we must stop
     */
    bool mustStop() const;

    /**
     * @brief Set the progression
     */
    void setProgress(int p);
};

#endif // PLYELEMENTREADER_H
