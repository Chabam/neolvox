#include "ct_standardnormalcloudregistered.h"

CT_AbstractNormalCloud* CT_StandardNormalCloudRegistered::abstractNormalCloud() const
{
    return cloudT();
}

CT_StandardNormalCloudRegistered::CT_StandardNormalCloudRegistered(CT_NormalCloudStdVector *normalCloud) : CT_StdCloudRegisteredT<CT_NormalCloudStdVector>(normalCloud)
{
}
