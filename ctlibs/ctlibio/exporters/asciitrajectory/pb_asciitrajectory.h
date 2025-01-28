#ifndef PB_ASCIITRAJECTORY_H
#define PB_ASCIITRAJECTORY_H

#include "ctlibio/ctlibio_global.h"

#include "ct_exporter/abstract/ct_abstractexporter.h"
#include "ct_itemdrawable/ct_scanpath.h"

class CTLIBIO_EXPORT PB_AsciiTrajectory : public CT_AbstractExporter
{
    Q_OBJECT
    using SuperClass = CT_AbstractExporter;

public:
    PB_AsciiTrajectory(int subMenuLevel = 0);
    PB_AsciiTrajectory(const PB_AsciiTrajectory& other);

    QString displayableName() const override;

    void setItemsToExport(const QList<const CT_ScanPath*>& items);

    virtual bool isExportEachItemInSeparateFileOptionnal() const override;

    CT_EXPORTER_DECL_COPY(PB_AsciiTrajectory)

protected:
    void internalDeclareInputModels(CT_ExporterInModelStructureManager& manager) override;

    ExportReturn internalExportToFile() override;

    void clearIterators() override;

    void clearAttributesClouds() override;

private:
    using HandleItemType = CT_HandleInSingularItem<CT_ScanPath>;

    CT_HandleInStdGroup<>                                           m_hInGroup;
    HandleItemType                                                  m_traj;

    HandleItemType::const_iterator                                  mIteratorItemBegin;
    HandleItemType::const_iterator                                  mIteratorItemEnd;

    QList<const CT_ScanPath*>                                       mItems;

    int                                                             _nExported;

    void exportItem(const CT_ScanPath* item, QTextStream &stream, const int& nExported, const int& totalToExport);
};

#endif // PB_ASCIITRAJECTORY_H
