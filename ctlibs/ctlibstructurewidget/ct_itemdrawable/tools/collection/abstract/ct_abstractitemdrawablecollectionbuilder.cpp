#include "ct_abstractitemdrawablecollectionbuilder.h"

CT_AbstractItemDrawableCollectionBuilder::CT_AbstractItemDrawableCollectionBuilder(bool mutuallyExclusive,
                                                                                   bool excludeColorsOfDocument,
                                                                                   bool excludeNormalsOfDocument) : m_mutuallyExclusive(mutuallyExclusive),
                                                                                                                  m_excludeColorsOfDocument(excludeColorsOfDocument),
                                                                                                                  m_excludeNormalsOfDocument(excludeNormalsOfDocument)
{
}

bool CT_AbstractItemDrawableCollectionBuilder::isMutuallyExclusive() const
{
    return m_mutuallyExclusive;
}

bool CT_AbstractItemDrawableCollectionBuilder::mustExcludeColorsOfDocuments() const
{
    return m_excludeColorsOfDocument;
}

bool CT_AbstractItemDrawableCollectionBuilder::mustExcludeNormalsOfDocuments() const
{
    return m_excludeNormalsOfDocument;
}
