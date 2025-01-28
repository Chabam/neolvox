#ifndef IPLYGENERICCONFIGURATION_H
#define IPLYGENERICCONFIGURATION_H

#include "readers/tools/ply/plyheader.h"

class IPlyGenericConfiguration {
public:
    IPlyGenericConfiguration() { m_elementIndex = -1; }
    virtual ~IPlyGenericConfiguration() {}

    /**
     * @brief Set the header to use
     */
    virtual void setHeader(const PlyHeader& header) { m_header = header; }

    /**
     * @brief Set the element to use
     */
    virtual void setPlyElement(const int& elementIndex) { m_elementIndex = elementIndex; resetUI(); }

    /**
     * @brief Returns the element used
     */
    int getPlyElement() const { return m_elementIndex; }

    /**
     * @brief Returns true if all channels is set
     */
    virtual bool isValid() const = 0;

    /**
     * @brief Convert to widget
     */
    virtual QWidget* toWidget() const = 0;

protected:
    PlyHeader                   m_header;
    int                         m_elementIndex;

    /**
     * @brief Call when the ply element change to reset the ui
     */
    virtual void resetUI() = 0;

    /**
     * @brief Returns a score to know if this property is the "xxx" channel or not
     */
    static int staticFindProperty(const PlyProperty& property, const QString &wordToFind) {
        if(property.getDisplayableName().indexOf(wordToFind) != -1)
            return 1;

        return 0;
    }
};

#endif // IPLYGENERICCONFIGURATION_H
