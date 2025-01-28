#ifndef PLYABSTRACTVALUESSETTER_H
#define PLYABSTRACTVALUESSETTER_H

#include <QtGlobal>

/**
 * @brief A PlyElement contains X properties and in the file it was Y elements. Per
 *        example :
 *
 *        element myElement 8
 *        property float a      { property }
 *        property float b      { property }
 *        property float c      { property }
 *        property float d      { property }
 *
 *        X = 4 property
 *        Y = 8 elements
 *
 *        This class will receive values from specific property (per example property a and b) in the
 *        "setValue" method.
 */
class PlyAbstractValuesSetter {
public:
    PlyAbstractValuesSetter(const size_t& size) : m_size(size) {
    }

    virtual ~PlyAbstractValuesSetter() {}

    /**
     * @brief Called when the reader begin to read the entire element
     */
    virtual void beginReadElement() { }

    /**
     * @brief Called to set a value to a specified channel (called when a property is readed)
     * @param value : value to set
     * @param channelIndex : channel index is per example 0 for x coordinate of a vertex
     */
    virtual void setValue(const double& value, const int& channelIndex) = 0;

    /**
     * @brief Called when one line of the element is readed (called when a line is readed and all properties was set to this object)
     */
    virtual void oneLineOfElementReaded() { }

    /**
     * @brief Called when the entire element is readed
     */
    virtual void endReadElement() { }

protected:
    size_t      m_size;
};

#endif // PLYABSTRACTVALUESSETTER_H
