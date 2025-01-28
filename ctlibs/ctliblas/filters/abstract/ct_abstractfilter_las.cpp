#include "ct_abstractfilter_las.h"

#include "ct_iterator/ct_pointiterator.h"
#include "ct_pointcloudindex/ct_pointcloudindexvector.h"
#include "ct_log/ct_logmanager.h"

CT_AbstractFilter_LAS::CT_AbstractFilter_LAS(QString pluginName) : CT_AbstractFilter_XYZ(pluginName),
    _lasAttributes(nullptr)
{
}

CT_AbstractFilter_LAS::CT_AbstractFilter_LAS(const CT_AbstractFilter_LAS &other) : CT_AbstractFilter_XYZ(other),
    _lasAttributes(other._lasAttributes)
{
}

bool CT_AbstractFilter_LAS::filterPointCloudIndex()
{
    if((_lasAttributes == nullptr) || (inputPointCloudIndex() == nullptr))
        return false;

    CT_LASData lasData;

    size_t nPointsKeept = 0;
    size_t cptErrors = 0;

    CT_PointIterator itP(inputPointCloudIndex());

    while(itP.hasNext())
    {
        const size_t globalIndex = itP.next().currentGlobalIndex();

        if(_lasAttributes->getLASDataAt(globalIndex, lasData))
        {
            if(validatePoint(itP, lasData))
            {
                outputPointCloudIndex()->addIndex(globalIndex);
                ++nPointsKeept;
            }
        }
        else
        {
            ++cptErrors;
        }
    }

    //PS_LOG->addInfoMessage(LogInterface::filter, tr("%1 points conservés sur %2 points").arg(nPointsKeept).arg(size));
    if (cptErrors > 0)
        PS_LOG->addMessage(LogInterface::info, LogInterface::filter, tr("Pas d'informations LAS pour %1 points : points non conservés").arg(cptErrors));

    return true;
}

bool CT_AbstractFilter_LAS::validatePoint(const CT_PointIterator &pointIt)
{
    if(_lasAttributes != nullptr)
    {
        CT_LASData lasData;

        if(_lasAttributes->getLASDataAt(pointIt.currentGlobalIndex(), lasData))
            return validatePoint(pointIt, lasData);
    }

    return false;
}

CT_StdLASPointsAttributesContainer* CT_AbstractFilter_LAS::lasAttributes() const
{
    return _lasAttributes;
}

bool CT_AbstractFilter_LAS::setLASAttributesContainer(const CT_StdLASPointsAttributesContainer *lasAttributes)
{
    if((lasAttributes == nullptr) || lasAttributes->lasPointsAttributes().isEmpty())
        return false;

    _lasAttributes = const_cast<CT_StdLASPointsAttributesContainer*>(lasAttributes);

    return true;
}

