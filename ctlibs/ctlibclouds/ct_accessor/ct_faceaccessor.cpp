#include "ct_faceaccessor.h"

#include "ct_global/ct_cloudscontext.h"
#include "ct_global/ct_repositorymanager.h"
#include "ct_global/ct_repository.h"
#include "ct_facecloud/abstract/ct_abstractfacecloud.h"

CT_FaceAccessor::CT_FaceAccessor()
{
    m_fCloud = PS_REPOSITORY->globalFaceCloud();
}

const CT_Face& CT_FaceAccessor::constFaceAt(const size_t &globalIndex) const
{
    return m_fCloud->constTAt(globalIndex);
}

CT_Face& CT_FaceAccessor::faceAt(const size_t &globalIndex)
{
    return m_fCloud->tAt(globalIndex);
}

size_t CT_FaceAccessor::size() const
{
    return m_fCloud->size();
}
