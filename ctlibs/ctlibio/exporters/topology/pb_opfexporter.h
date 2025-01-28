#ifndef PB_OPFEXPORTER_H
#define PB_OPFEXPORTER_H

#include "ctlibio/ctlibio_global.h"

#include "ct_exporter/abstract/ct_abstractexporter.h"

class QTextStream;
class CT_TTreeGroup;
class CT_TNodeGroup;
class CT_AbstractSingularItemDrawable;
class CT_MeshModel;
class CT_ItemAttributeList;
class CT_AbstractItemAttribute;
class CT_Mesh;

class CTLIBIO_EXPORT PB_OPFExporter : public CT_AbstractExporter
{
    Q_OBJECT
    using SuperClass = CT_AbstractExporter;

public:
    PB_OPFExporter(int subMenuLevel = 0);
    PB_OPFExporter(const PB_OPFExporter& other);

    QString displayableName() const final;

    bool isExportEachItemInSeparateFileOptionnal() const final;

    void setTreeGroupsToExport(const QList<const CT_TTreeGroup*>& list);

    CT_EXPORTER_DECL_COPY(PB_OPFExporter)

protected:
    void internalDeclareInputModels(CT_ExporterInModelStructureManager& manager) override;

    ExportReturn internalExportToFile() override;

    void clearIterators() override;

    void clearAttributesClouds() override;

private:
    using HandleGroupType = CT_HandleIterationDecorator<CT_HandleInGroup<CT_TTreeGroup, 1, 1>, CT_TTreeGroup>;

    HandleGroupType                     m_hInGroup;

    HandleGroupType::const_iterator     mIteratorGroupBegin;
    HandleGroupType::const_iterator     mIteratorGroupEnd;

    QList<const CT_TTreeGroup*>         mTreeGroups;

    QList<CT_Mesh*>                     m_meshes;

    bool exportTreeGroup(const CT_TTreeGroup* topology, const QString& currentFilePath);
    bool writeAttributesBdd(QTextStream &stream, const CT_TTreeGroup* topology, const QString &prefix);
    void recursiveSearchAttributes(const DEF_CT_AbstractGroupModelOut* model, QHash<QString, QString> &out);
    bool writeMeshAndShapeBdd(QTextStream &stream, const QString &prefix);
    void recursiveSearchMesh(CT_TNodeGroup *node);
    bool writeMesh(QTextStream &stream, CT_Mesh *mesh, const int &index, const QString &prefix);
    bool writeShape(QTextStream &stream, CT_Mesh *mesh, const int &index, const QString &prefix);
    bool recursiveWriteFile(QTextStream &stream, const QString &type, CT_TNodeGroup *node, const QString &prefix);
    bool writeAttribute(QTextStream &stream, const CT_TNodeGroup* node, const CT_AbstractSingularItemDrawable* l, const CT_AbstractItemAttribute* att, const QString &prefix);
    bool writeGeometry(QTextStream &stream, CT_TNodeGroup *node, const CT_MeshModel* mesh, const QString &prefix);
};

#endif // PB_OPFEXPORTER_H
