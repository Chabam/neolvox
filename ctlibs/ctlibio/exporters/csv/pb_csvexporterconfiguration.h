#ifndef PB_CSVEXPORTERCONFIGURATION_H
#define PB_CSVEXPORTERCONFIGURATION_H

#include <QList>
#include <QPair>

class CT_OutAbstractSingularItemModel;
class CT_OutAbstractItemAttributeModel;

class PB_CSVExporterConfiguration
{
public:
    PB_CSVExporterConfiguration(const PB_CSVExporterConfiguration& other);
    PB_CSVExporterConfiguration(const QList<const CT_OutAbstractSingularItemModel*> &list);

    void setList(const QList<const CT_OutAbstractSingularItemModel*>& list);
    const QList<const CT_OutAbstractSingularItemModel*>& list() const;

    void addColumn(const CT_OutAbstractSingularItemModel *sItem, const CT_OutAbstractItemAttributeModel *ia);

    const QList< QPair<const CT_OutAbstractSingularItemModel*, const CT_OutAbstractItemAttributeModel*> >& getColumns() const;

    void clearColumns();

private:

    QList<const CT_OutAbstractSingularItemModel*>                                                 _list;
    QList< QPair<const CT_OutAbstractSingularItemModel*, const CT_OutAbstractItemAttributeModel*> >     _columns;
};

#endif // PB_CSVEXPORTERCONFIGURATION_H
