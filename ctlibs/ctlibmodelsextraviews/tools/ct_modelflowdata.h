#ifndef CT_MODELFLOWDATA_H
#define CT_MODELFLOWDATA_H

#include <nodes/NodeData>

#include "ct_model/abstract/ct_abstractmodel.h"

using QtNodes::NodeData;
using QtNodes::NodeDataType;

class CT_ModelFlowData : public NodeData
{
public:
    CT_ModelFlowData(const CT_AbstractModel* model = nullptr, const QString& id = QString());

    NodeDataType type() const override;

    const CT_AbstractModel* model() const;

private:
    const CT_AbstractModel* mModel;
    const QString           mId;
};

#endif // CT_MODELFLOWDATA_H
