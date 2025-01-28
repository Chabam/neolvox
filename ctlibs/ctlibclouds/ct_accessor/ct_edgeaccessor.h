#ifndef CT_EDGEACCESSOR_H
#define CT_EDGEACCESSOR_H

#include "ct_defines.h"
#include "ct_mesh/ct_edge.h"

/**
 * @brief Use this class to access edge in the global edge cloud
 */
class CTLIBCLOUDS_EXPORT CT_EdgeAccessor
{
public:
    CT_EdgeAccessor();

    /**
     * @brief Return a const reference to the edge at 'globalIndex'
     */
    const CT_Edge& constEdgeAt(const size_t &globalIndex) const;

    /**
     * @brief Return a reference to the edge at 'globalIndex'
     */
    CT_Edge& edgeAt(const size_t &globalIndex);

    /**
     * @brief Returns the size of the global edge cloud
     */
    size_t size() const;

private:
    CT_AbstractEdgeCloud   *m_eCloud;
};

#endif // CT_EDGEACCESSOR_H
