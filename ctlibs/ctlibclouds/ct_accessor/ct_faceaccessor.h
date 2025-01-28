#ifndef CT_FACEACCESSOR_H
#define CT_FACEACCESSOR_H

#include "ct_defines.h"
#include "ct_mesh/ct_face.h"

/**
 * @brief Use this class to access face in the global face cloud
 */
class CTLIBCLOUDS_EXPORT CT_FaceAccessor
{
public:
    CT_FaceAccessor();

    /**
     * @brief Return a const reference to the face at 'globalIndex'
     */
    const CT_Face& constFaceAt(const size_t &globalIndex) const;

    /**
     * @brief Return a reference to the face at 'globalIndex'
     */
    CT_Face& faceAt(const size_t &globalIndex);

    /**
     * @brief Returns the size of the global face cloud
     */
    size_t size() const;

private:
    CT_AbstractFaceCloud   *m_fCloud;
};

#endif // CT_FACEACCESSOR_H
