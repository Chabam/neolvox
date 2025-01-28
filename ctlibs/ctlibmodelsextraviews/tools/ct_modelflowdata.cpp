#include "ct_modelflowdata.h"

CT_ModelFlowData::CT_ModelFlowData(const CT_AbstractModel* model, const QString& id) :
    mModel(model),
    mId(id)
{
}

QtNodes::NodeDataType CT_ModelFlowData::type() const
{
    if(mModel == nullptr)
        return NodeDataType{};

    return NodeDataType {mId, mModel->displayableName()};
}

const CT_AbstractModel* CT_ModelFlowData::model() const
{
    return mModel;
}
