#include "pb_csvexporterconfiguration.h"

#include "ct_model/outModel/abstract/ct_outabstractitemattributemodel.h"
#include "ct_model/outModel/abstract/ct_outabstractsingularitemmodel.h"

PB_CSVExporterConfiguration::PB_CSVExporterConfiguration(const PB_CSVExporterConfiguration &other) : _list(other._list), _columns(other._columns)
{
}

PB_CSVExporterConfiguration::PB_CSVExporterConfiguration(const QList<const CT_OutAbstractSingularItemModel *> &list) : _list(list)
{
}

void PB_CSVExporterConfiguration::setList(const QList<const CT_OutAbstractSingularItemModel *> &list)
{
    _list = list;
}

const QList<const CT_OutAbstractSingularItemModel*>& PB_CSVExporterConfiguration::list() const
{
    return _list;
}

void PB_CSVExporterConfiguration::addColumn(const CT_OutAbstractSingularItemModel *sItem, const CT_OutAbstractItemAttributeModel *ia)
{
    _columns.append(qMakePair(const_cast<CT_OutAbstractSingularItemModel*>(sItem), const_cast<CT_OutAbstractItemAttributeModel*>(ia)));
}

const QList<QPair<const CT_OutAbstractSingularItemModel *, const CT_OutAbstractItemAttributeModel *> > &PB_CSVExporterConfiguration::getColumns() const
{
    return _columns;
}

void PB_CSVExporterConfiguration::clearColumns()
{
    _columns.clear();
}
