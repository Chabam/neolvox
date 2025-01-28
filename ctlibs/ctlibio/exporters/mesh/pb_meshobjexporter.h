#ifndef PB_MESHOBJEXPORTER_H
#define PB_MESHOBJEXPORTER_H

#include "ctlibio/ctlibio_global.h"

#include "ct_exporter/abstract/ct_abstractexporter.h"
#include "ct_itemdrawable/ct_meshmodel.h"

class CTLIBIO_EXPORT PB_MeshObjExporter : public CT_AbstractExporter
{
    Q_OBJECT
    using SuperClass = CT_AbstractExporter;

public:
    PB_MeshObjExporter(int subMenuLevel = 0);
    PB_MeshObjExporter(const PB_MeshObjExporter& other);

    QString displayableName() const final;

    void setMeshesToExport(const QList<const CT_MeshModel*>& list);

    CT_EXPORTER_DECL_COPY(PB_MeshObjExporter)

protected:
    void internalDeclareInputModels(CT_ExporterInModelStructureManager& manager) final;

    ExportReturn internalExportToFile() final;

    void clearIterators() final;

    void clearAttributesClouds() final;

private:
    using HandleItemType = CT_HandleInSingularItem<CT_MeshModel>;

    CT_HandleInStdGroup<>               m_hInGroup;
    HandleItemType                      m_hInItem;

    HandleItemType::const_iterator      mIteratorItemBegin;
    HandleItemType::const_iterator      mIteratorItemEnd;

    QList<const CT_MeshModel*>          mMeshes;

    bool exportMesh(const CT_MeshModel* item, QTextStream& stream, const int& nExported, const int& totalToExport, size_t& ptNb, size_t& lastPointCount);
};

#endif // PB_MESHOBJEXPORTER_H
