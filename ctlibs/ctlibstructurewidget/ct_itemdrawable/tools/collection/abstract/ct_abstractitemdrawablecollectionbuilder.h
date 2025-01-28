#ifndef CT_ABSTRACTITEMDRAWABLECOLLECTIONBUILDER_H
#define CT_ABSTRACTITEMDRAWABLECOLLECTIONBUILDER_H

#include "ctlibstructurewidget_global.h"

class CT_VirtualAbstractStep;
class CT_ItemDrawableCollectionHierarchyStep;

/**
 * @brief A class that define the interface for a builder that search other itemdrawable to export
 *        from a step.
 */
class CTLIBSTRUCTUREWIDGET_EXPORT CT_AbstractItemDrawableCollectionBuilder
{
public:
    /**
     * @brief Construct a builder
     * @param mutuallyExclusive : set to true if you want that results of this builder was mutually exclusive when it was checked
     * @param excludeColorsOfDocument : set to true if you want that when a result of this builder is checked the colors of document was unchecked
     * @param excludeNormalsOfDocument : set to true if you want that when a result of this builder is checked the normals of document was unchecked
     */
    CT_AbstractItemDrawableCollectionBuilder(bool mutuallyExclusive, bool excludeColorsOfDocument = false, bool excludeNormalsOfDocument = false);
    CT_AbstractItemDrawableCollectionBuilder(const CT_AbstractItemDrawableCollectionBuilder& other) = default;

    virtual ~CT_AbstractItemDrawableCollectionBuilder() {}

    /**
     * @brief Returns true if result in the collection must be mutually exclusive, false otherwise. true by default.
     */
    bool isMutuallyExclusive() const;

    /**
     * @brief Returns true if result in the collection must exclude colors of document when it was checked, false otherwise. false by default.
     */
    bool mustExcludeColorsOfDocuments() const;

    /**
     * @brief Returns true if result in the collection must exclude normals of document when it was checked, false otherwise. false by default.
     */
    bool mustExcludeNormalsOfDocuments() const;

    /**
     * @brief Search elements from step passed in parameter.
     */
    virtual bool buildFrom(const CT_VirtualAbstractStep *step, bool findOnlyModels) = 0;

    /**
     * @brief Returns the list of results sorted by step
     */
    virtual const QList<CT_ItemDrawableCollectionHierarchyStep>& collection() const = 0;

    /**
     * @brief Returns a full copy of this object
     */
    virtual CT_AbstractItemDrawableCollectionBuilder* copy() const = 0;

private:
    bool    m_mutuallyExclusive;
    bool    m_excludeColorsOfDocument;
    bool    m_excludeNormalsOfDocument;
};

#endif // CT_ABSTRACTITEMDRAWABLECOLLECTIONBUILDER_H
