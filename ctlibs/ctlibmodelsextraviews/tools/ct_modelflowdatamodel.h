#ifndef CT_MODELFLOWDATAMODEL_H
#define CT_MODELFLOWDATAMODEL_H

#include "ct_model/abstract/ct_abstractmodel.h"

#include <nodes/NodeDataModel>
#include <QAbstractItemModel>

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeValidationState;

class CT_ModelFlowDataModel : public NodeDataModel
{
    Q_OBJECT

public:
    CT_ModelFlowDataModel();
    CT_ModelFlowDataModel(const QString& title, const QAbstractItemModel* model, PortType portType, QWidget* embeddedWidget = nullptr);

    QString caption() const override;

    bool portCaptionVisible(PortType, PortIndex) const override;

    QString name() const override;

    unsigned int nPorts(PortType portType) const override;

    NodeDataType dataType(PortType portType,
                          PortIndex portIndex) const override;

    void setInData(std::shared_ptr<NodeData> nodeData, PortIndex port) override { Q_UNUSED(nodeData); Q_UNUSED(port); }

    std::shared_ptr<NodeData> outData(PortIndex port) const override;

    std::shared_ptr<NodeData> inData(PortIndex port) const override;

    QWidget* embeddedWidget() override;

    PortIndex portIndexOfModel(const CT_AbstractModel* model) const;

protected:
    struct PortInfo {
        PortInfo() : mModel(nullptr) {}
        PortInfo(CT_AbstractModel* model, const QString& id) : mModel(model), mId(id) {}

        CT_AbstractModel*   mModel;
        QString             mId;
    };

    QString                                 mTitle;
    QAbstractItemModel*                     mModel;
    PortType                                mPortType;
    QHash<PortIndex,PortInfo>               mModelByPortIndex;
    QWidget*                                mEmbeddedWidget;

    void recursiveConstructPorts(QModelIndex current, PortIndex& portIndex);
};

#endif // CT_MODELFLOWDATAMODEL_H
