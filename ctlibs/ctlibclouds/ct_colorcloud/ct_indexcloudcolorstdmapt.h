#ifndef CT_INDEXCLOUDCOLORSTDMAPT_H
#define CT_INDEXCLOUDCOLORSTDMAPT_H

#include "ct_cloudindex/ct_cloudindexstdmapt.h"
#include "ct_colorcloud/abstract/ct_abstractmodifiableindexcloudcolormap.h"
#include "ct_color.h"

class CT_AbstractColorCloud;

/**
 * @brief Represent a cloud of colors that use a QMap with key of type "size_t" --> the index of a point, face or edge
 *        and the value of type "CT_Color" --> the color
 */
template<typename T>
class CT_IndexCloudColorStdMapT : virtual public CT_CloudIndexStdMapT<T, CT_Color>, virtual public CT_AbstractModifiableIndexCloudColorMap
{
public:
    CT_IndexCloudColorStdMapT();

    /**
     * @brief Insert an index in the cloud with a color (if the index already exist the value will be ignored)
     *
     * @param newIndex : index to add
     * @param color : color to add
     */
    void insertIndexAndColor(const size_t &newIndex, const CT_Color &color);

    /**
     * @brief Returns the color for the key "index" ---> point, face or edge index ( indexAt(cI, index); )
     * @return If index was not found the default value is returned.
     */
    CT_Color* colorAtGlobalIndex(const size_t &index, const CT_Color *defaultValue = nullptr) const;

    /**
     * @brief Returns the color for the key "index" ---> index >= 0 && index <= indexSize()
     * @return If index was not found the default value is returned.
     */
    CT_Color* colorAt(const size_t &index, const CT_Color *defaultValue = nullptr) const;

    /**
     * @brief Copy colors from this map to the ColorCloud passed in parameter
     */
    void copyColors(CT_AbstractColorCloud *cc) const;

    /**
     * @brief Copy colors froms keys of 'indexes' to the ColorCloud passed in parameter
     * @param eraseKeys : true if you want to erase keys founded
     */
    void copyColorsOfKeys(CT_AbstractColorCloud *cc, const std::vector<size_t> &indexes, bool eraseKeys = false);

    /**
     * @brief Copy colors froms keys of 'CT_AbstractCloudIndex' to the ColorCloud passed in parameter
     * @param eraseKeys : true if you want to erase keys founded
     */
    void copyColorsOfCloudIndex(CT_AbstractColorCloud *cc, const QList<CT_AbstractCloudIndex*> &lci, bool eraseKeys = false);

    /**
     * @brief Remove the index from the cloud
     * @param index : index to remove
     */
    void removeIndex(const size_t &index);

    /**
     * @brief Removes all indexes in the cloud index
     */
    void clear();

    /**
     * @brief Returns a copy of this cloud index
     */
    CT_AbstractCloud* copy() const;
};

#include "ct_colorcloud/ct_indexcloudcolorstdmapt.hpp"

#endif // CT_INDEXCLOUDCOLORSTDMAPT_H
