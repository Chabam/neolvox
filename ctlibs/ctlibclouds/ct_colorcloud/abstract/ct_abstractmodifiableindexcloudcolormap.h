#ifndef CT_ABSTRACTMODIFIABLEINDEXCLOUDCOLORMAP_H
#define CT_ABSTRACTMODIFIABLEINDEXCLOUDCOLORMAP_H

#include "ct_color.h"
#include "ct_colorcloud/abstract/ct_abstractcolorcloud.h"

class CT_AbstractCloudIndex;

class CTLIBCLOUDS_EXPORT CT_AbstractModifiableIndexCloudColorMap
{
public:
    CT_AbstractModifiableIndexCloudColorMap();
    virtual ~CT_AbstractModifiableIndexCloudColorMap() {}

    /**
     * @brief Insert an index in the cloud with a color (if the index already exist the value will be ignored)
     *
     * @param newIndex : index to add
     * @param color : color to add
     */
    virtual void insertIndexAndColor(const size_t &newIndex, const CT_Color &color) = 0;

    /**
     * @brief Returns the color for the key "index" ---> point, face or edge index ( indexAt(cI, index); )
     * @return If index was not found the default value is returned.
     */
    virtual CT_Color* colorAtGlobalIndex(const size_t &index, const CT_Color *defaultValue = nullptr) const = 0;

    /**
     * @brief Returns the color for the key "index" ---> index >= 0 && index <= indexSize()
     * @return If index was not found the default value is returned.
     */
    virtual CT_Color* colorAt(const size_t &index, const CT_Color *defaultValue = nullptr) const = 0;

    /**
     * @brief Copy colors from this map to the ColorCloud passed in parameter
     */
    virtual void copyColors(CT_AbstractColorCloud *cc) const = 0;

    /**
     * @brief Copy colors froms keys of 'indexes' to the ColorCloud passed in parameter
     * @param eraseKeys : true if you want to erase keys founded
     */
    virtual void copyColorsOfKeys(CT_AbstractColorCloud *cc, const std::vector<size_t> &indexes, bool eraseKeys = false) = 0;

    /**
     * @brief Copy colors froms keys of 'CT_AbstractCloudIndex' to the ColorCloud passed in parameter
     * @param eraseKeys : true if you want to erase keys founded
     */
    virtual void copyColorsOfCloudIndex(CT_AbstractColorCloud *cc, const QList<CT_AbstractCloudIndex*> &lci, bool eraseKeys = false) = 0;

    /**
     * @brief Remove the index from the cloud
     * @param index : index to remove
     */
    virtual void removeIndex(const size_t &index) = 0;

    /**
     * @brief Removes all indexes in the cloud index
     */
    virtual void clear() = 0;
};

#endif // CT_ABSTRACTMODIFIABLEINDEXCLOUDCOLORMAP_H
