#ifndef CT_INDATATYPETOOUTDATATYPECONVERTER_H
#define CT_INDATATYPETOOUTDATATYPECONVERTER_H

#include <nodes/NodeData>
#include <memory>

using QtNodes::NodeData;

class CT_InDataTypeToOutDataTypeConverter
{
public:
  std::shared_ptr<NodeData> operator()(std::shared_ptr<NodeData> data);
};

#endif // CT_INDATATYPETOOUTDATATYPECONVERTER_H
