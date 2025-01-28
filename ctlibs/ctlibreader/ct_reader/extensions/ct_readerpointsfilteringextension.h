#ifndef CT_READERPOINTSFILTERINGEXTENSION_H
#define CT_READERPOINTSFILTERINGEXTENSION_H

#include "ctlibreader_global.h"
#include "ct_itemdrawable/abstract/ct_abstractitemdrawablewithpointcloud.h"

#include <functional>

class CT_Point;

/**
 * Class that offer the points filtering extension to your reader
 */
class CTLIBREADER_EXPORT CT_ReaderPointsFilteringExtension
{
public:
    using filterPointsFunction = std::function<bool(const CT_Point&)>;

    CT_ReaderPointsFilteringExtension();
    virtual ~CT_ReaderPointsFilteringExtension() {}

    /**
     * @brief Set the filtering function (can be a lambda expression per example)
     */
    void setPointsFilter(filterPointsFunction f);

    /**
     * @brief Return the filtering function
     */
    filterPointsFunction pointsFilter() const;

    /**
     * @brief Return true if the filtering function has been set
     */
    bool filterSet() const {return m_pointsFilter != nullptr;}

    /**
     * @brief Return true if points must be filtered (removed), false otherwise
     */
    virtual bool isPointFiltered(const CT_Point& pt) const;

    /**
     * @brief Return scenes created by the reader (call after internalReadFile)
     */
    CT_AbstractItemDrawableWithPointCloud* scene() {return m_readScene;}

private:
    filterPointsFunction    m_pointsFilter;

protected:
    CT_AbstractItemDrawableWithPointCloud*      m_readScene;

};

#endif // CT_READERPOINTSFILTERINGEXTENSION_H
