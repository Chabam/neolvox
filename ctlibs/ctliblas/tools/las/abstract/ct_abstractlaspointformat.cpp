#include "ct_abstractlaspointformat.h"

#include "ct_iterator/ct_pointiterator.h"
#include "ct_pointcloud/ct_internalpointcloud.h"

CT_AbstractLASPointFormat::CT_AbstractLASPointFormat() :
    mInitialized(false),
    mReturnNumberInitialized(false),
    mColors(nullptr)
{
    mAttributesByType.resize(CT_LasDefine::NOT_TO_USE_SIZE_OF_ENUM);
    mAttributesByType.fill(nullptr);
}

CT_AbstractLASPointFormat::~CT_AbstractLASPointFormat()
{
    clearInfos();
}

bool CT_AbstractLASPointFormat::initWrite(const AttributeByTypeCollection& attributes, const CT_PointsAttributesColor* colors)
{
    QMutexLocker locker(&mMutexInitialization);

    if(mInitialized)
        return true;

    // get type of attribute to search for point format X (from derivated class)
    QList<CT_LasDefine::LASPointAttributesType> types = typesToSearch();

    if(!types.isEmpty())
    {
        if(mReturnNumberInitialized)
            types.removeOne(CT_LasDefine::Return_Number);

        if(colors != nullptr)
        {
            bool removed = types.removeOne(CT_LasDefine::Red);
            removed |= types.removeOne(CT_LasDefine::Green);
            removed |= types.removeOne(CT_LasDefine::Blue);

            if(removed)
                mColors = colors;
        }

        // for each type of attribute
        for(CT_LasDefine::LASPointAttributesType type : types)
        {
            initType(type, attributes);
        }
    }

    mInitialized = true;

    return true;
}

const CT_AbstractPointAttributesScalar*CT_AbstractLASPointFormat::scalars(const CT_LasDefine::LASPointAttributesType& type) const
{
    return mAttributesByType[type];
}

const CT_PointsAttributesColor* CT_AbstractLASPointFormat::colors() const
{
    return mColors;
}

void CT_AbstractLASPointFormat::initType(const CT_LasDefine::LASPointAttributesType& type, const AttributeByTypeCollection& attributes)
{
    mAttributesByType[type] = attributes.value(type, nullptr);
}

void CT_AbstractLASPointFormat::clearInfos()
{
    QMutexLocker locker(&mMutexInitialization);

    mInitialized = false;
    mReturnNumberInitialized = false;
    mAttributesByType.fill(nullptr);
    mColors = nullptr;
}

bool CT_AbstractLASPointFormat::initReturnNumber(const CT_AbstractLASPointFormat::AttributeByTypeCollection& attributes)
{
    QMutexLocker locker(&mMutexInitialization);

    if(mInitialized || mReturnNumberInitialized)
        return true;

    initType(CT_LasDefine::Return_Number, attributes);

    mReturnNumberInitialized = true;
    return true;
}
