#include "pb_opfexporter.h"

#include "ct_itemdrawable/ct_ttreegroup.h"
#include "ct_itemdrawable/ct_tnodegroup.h"
#include "ct_itemdrawable/ct_topfnodegroup.h"

#include "ct_model/outModel/ct_outopfnodegroupmodel.h"

#include "ct_itemdrawable/ct_itemattributelist.h"
#include "ct_itemdrawable/ct_opfmeshmodel.h"

#include "ct_point.h"
#include "ct_mesh/ct_face.h"

#include "ct_iterator/ct_pointiterator.h"
#include "ct_iterator/ct_faceiterator.h"

#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <limits>

PB_OPFExporter::PB_OPFExporter(int subMenuLevel) : SuperClass(subMenuLevel)
{
    addNewExportFormat(FileFormat("opf", tr("Fichiers AmapStudio .opf")));

    setToolTip(tr("Exporte une topologie décrivant l'architecture de végétaux dans un fichier au format OPF, pour utilisation par le logiciel AMAPStudio.<br>"
                  "http://amapstudio.cirad.fr"));
}

PB_OPFExporter::PB_OPFExporter(const PB_OPFExporter& other) : SuperClass(other),
    mTreeGroups(other.mTreeGroups)
{
}

QString PB_OPFExporter::displayableName() const
{
    return tr("Topologie, format OPF");
}

bool PB_OPFExporter::isExportEachItemInSeparateFileOptionnal() const
{
    return false;
}

void PB_OPFExporter::setTreeGroupsToExport(const QList<const CT_TTreeGroup*>& list)
{
    setMustUseModels(false);

    mTreeGroups = list;
}

void PB_OPFExporter::internalDeclareInputModels(CT_ExporterInModelStructureManager& manager)
{
    manager.addGroupToRootGroup(m_hInGroup, tr("Groupe OPF à exporter"));
}

CT_AbstractExporter::ExportReturn PB_OPFExporter::internalExportToFile()
{
    const QFileInfo exportPathInfo = QFileInfo(filePath());
    const QString prePath = QString("%1/%2").arg(exportPathInfo.path()).arg(exportPathInfo.baseName());
    const QString suffix = "opf";

    if(mustUseModels())
    {
        if(mIteratorGroupBegin == mIteratorGroupEnd)
        {
            auto iterator = m_hInGroup.iterateInputs(m_handleResultExport);
            mIteratorGroupBegin = iterator.begin();
            mIteratorGroupEnd = iterator.end();
        }

        int nExported = 0;
        const int totalToExport = maximumItemToExportInFile(int(std::distance(mIteratorGroupBegin, mIteratorGroupEnd)));

        // write data
        while((mIteratorGroupBegin != mIteratorGroupEnd)
              && (nExported < totalToExport))
        {
            if(isStopped())
                break;

            const CT_TTreeGroup* item = *mIteratorGroupBegin;

            if(!exportTreeGroup(item, prePath + "." + suffix))
                return ErrorWhenExport;

            ++nExported;
            ++mIteratorGroupBegin;

            setExportProgress((nExported*100)/totalToExport);
        }

        return (mIteratorGroupBegin == mIteratorGroupEnd) ? NoMoreItemToExport : ExportCanContinue;
    }
    else
    {
        const int totalToExport = mTreeGroups.size();
        int nExported = 0;

        for(const CT_TTreeGroup* item : mTreeGroups)
        {
            if(isStopped())
                break;

            if(!exportTreeGroup(item, prePath + QString("_%1.").arg(nExported) + suffix))
                return ErrorWhenExport;

            ++nExported;
            setExportProgress((nExported*100)/totalToExport);
        }
    }


    return NoMoreItemToExport;
}

void PB_OPFExporter::clearIterators()
{
    mIteratorGroupBegin = HandleGroupType::const_iterator();
    mIteratorGroupEnd = mIteratorGroupBegin;
}

void PB_OPFExporter::clearAttributesClouds()
{
}

bool PB_OPFExporter::exportTreeGroup(const CT_TTreeGroup* topology, const QString& currentFilePath)
{
    QFile file(currentFilePath);

    if(file.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream txtStream(&file);

        QString prefix = "";

        txtStream << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << Qt::endl;
        txtStream << prefix << "<opf version=\"2.0\" editable=\"true\">" << Qt::endl;

        CT_TNodeGroup* root = topology->rootNode();

        bool ok = true;

        if(root != nullptr)
        {
            m_meshes.clear();

            recursiveSearchMesh(root);

            writeMeshAndShapeBdd(txtStream, prefix + "\t");
            writeAttributesBdd(txtStream, topology, prefix + "\t");

            ok = recursiveWriteFile(txtStream, "topology", root, prefix + "\t");
        }

        txtStream << prefix << "</opf>" << Qt::endl;

        file.close();

        return ok;
    }

    return false;
}

bool PB_OPFExporter::writeAttributesBdd(QTextStream &stream, const CT_TTreeGroup *topology, const QString &prefix)
{
    stream << prefix << "<attributeBDD>" << Qt::endl;

    DEF_CT_AbstractGroupModelOut* model = topology->modelStaticT<DEF_CT_AbstractGroupModelOut>();

    QHash<QString, QString> list;
    recursiveSearchAttributes(model, list);

    QHashIterator<QString, QString> it(list);

    while(it.hasNext()) {
        it.next();
        stream << prefix << "\t<attribute name=\"" << it.key() << "\" class=\"" << it.value() << "\"/>" << Qt::endl;
    }

    stream << prefix << "</attributeBDD>" << Qt::endl;

    return true;
}

void PB_OPFExporter::recursiveSearchAttributes(const DEF_CT_AbstractGroupModelOut* model, QHash<QString, QString> &out)
{
    if(model == nullptr)
        return;

    model->visitItems([&out](const CT_OutAbstractSingularItemModel* itemModel) -> bool
    {
        itemModel->visitAttributes([&out](const CT_OutAbstractItemAttributeModel* attModel) -> bool
        {
            if(!out.contains(attModel->displayableName()))
            {
                const int type = attModel->itemAttribute()->itemAttributeToolForModel()->valueType();
                out.insert(attModel->displayableName(), (type == CT_AbstractCategory::STRING) ? "String" : "Double");
            }

            return true;
        });

        return true;
    });

    model->visitGroups([this, &out](const CT_OutAbstractGroupModel* groupModel) -> bool
    {
        this->recursiveSearchAttributes(dynamic_cast<const DEF_CT_AbstractGroupModelOut*>(groupModel), out);
        return true;
    });
}

bool PB_OPFExporter::writeMeshAndShapeBdd(QTextStream &stream, const QString &prefix)
{
    if(!m_meshes.isEmpty())
    {
        stream << prefix << "<meshBDD>" << Qt::endl;

        int i = 0;

        QListIterator<CT_Mesh*> it(m_meshes);

        while(it.hasNext())
            writeMesh(stream, it.next(), i++, prefix + "\t");

        stream << prefix << "</meshBDD>" << Qt::endl;

        stream << prefix << "<shapeBDD>" << Qt::endl;

        i = 0;

        it.toFront();

        while(it.hasNext())
            writeShape(stream, it.next(), i++, prefix + "\t");

        stream << prefix << "</shapeBDD>" << Qt::endl;
    }

    return true;
}

void PB_OPFExporter::recursiveSearchMesh(CT_TNodeGroup *node)
{
    const CT_MeshModel* mesh = nullptr;

    node->visitSingularItems([&mesh](const CT_AbstractSingularItemDrawable* item) -> bool
    {
        mesh = dynamic_cast<const CT_MeshModel*>(item);
        return (mesh == nullptr);
    });

    if(mesh != nullptr)
    {
        if(!m_meshes.contains(mesh->mesh()))
            m_meshes.append(mesh->mesh());
    }

    CT_TNodeGroup* component = node->rootComponent();

    if(component != nullptr)
        recursiveSearchMesh(component);

    for(CT_TNodeGroup* branch : node->branches())
        recursiveSearchMesh(branch);

    if(component != nullptr)
    {
        while(component->successor() != nullptr)
        {
            component = component->successor();
            recursiveSearchMesh(component);
        }
    }
}

bool PB_OPFExporter::writeMesh(QTextStream &stream, CT_Mesh *mesh, const int &index, const QString &prefix)
{
    CT_AbstractPointCloudIndex *pci = mesh->abstractVert();
    CT_AbstractFaceCloudIndex *fci = mesh->abstractFace();

    if((pci != nullptr)
            && (fci != nullptr))
    {
        stream << prefix << "<mesh Id=\"" << index << "\" enableScale=\"true\">" << Qt::endl;

        CT_PointIterator itP(pci);

        size_t pBeginIndex = std::numeric_limits<size_t>::max();

        stream << prefix << "\t<points>" << Qt::endl << prefix << "\t\t";

        while(itP.hasNext())
        {
            const CT_Point &p = itP.next().currentPoint();

            stream << p(0) << " " << p(1) << " " << p(2) << " ";

            if(itP.cIndex() < pBeginIndex)
                pBeginIndex = itP.cIndex();
        }

        stream << Qt::endl << prefix << "\t</points>" << Qt::endl;

        CT_FaceIterator itF(fci);

        stream << prefix << "\t<faces>" << Qt::endl;

        int i = 0;

        while(itF.hasNext())
        {
            const CT_Face &f = itF.next().cT();

            stream << prefix << "\t\t<face Id=\"" << i << "\">" << Qt::endl;

            stream << prefix << "\t\t\t" << f.iPointAt(0)-pBeginIndex << " " << f.iPointAt(1)-pBeginIndex << " " << f.iPointAt(2)-pBeginIndex << Qt::endl;

            stream << prefix << "\t\t</face>" << Qt::endl;

            ++i;
        }

        stream << prefix << "\t</faces>" << Qt::endl;

        stream << prefix << "</mesh>" << Qt::endl;
    }

    return true;
}

bool PB_OPFExporter::writeShape(QTextStream &stream, CT_Mesh *mesh, const int &index, const QString &prefix)
{
    CT_AbstractPointCloudIndex *pci = mesh->abstractVert();
    CT_AbstractFaceCloudIndex *fci = mesh->abstractFace();

    if((pci != nullptr)
            && (fci != nullptr))
    {
        stream << prefix << "<shape Id=\"" << index << "\">" << Qt::endl;
        stream << prefix << "\t<name>Mesh" << index << "</name>" << Qt::endl;
        stream << prefix << "\t<meshIndex>" << index << "</meshIndex>" << Qt::endl;
        stream << prefix << "</shape>" << Qt::endl;
    }

    return true;
}

bool PB_OPFExporter::recursiveWriteFile(QTextStream &stream, const QString &type, CT_TNodeGroup *node, const QString &prefix)
{
    bool ok = false;

    CT_TOPFNodeGroup* opfNode = dynamic_cast<CT_TOPFNodeGroup*>(node);

    quint8 scale = 0;

    if (opfNode != nullptr)
        scale = opfNode->opfModel()->opfLevel();

    stream << prefix << "<" << type << " class=\"" << node->model()->displayableName() << "\" scale=\"" << scale << "\" id=\"" << node->id() << "\">" << Qt::endl;

    const CT_MeshModel* mesh = nullptr;

    node->visitSingularItems([this, &mesh, &stream, &node, &prefix](const CT_AbstractSingularItemDrawable* item) -> bool
    {
        const CT_MeshModel* oMesh = dynamic_cast<const CT_MeshModel*>(item);

        if(mesh == nullptr)
            mesh = oMesh;

        if(mesh == nullptr)
        {
            item->visitDefaultItemAttributes([this, &item, &stream, &node, &prefix](const CT_AbstractItemAttribute* ia) -> bool
            {
                writeAttribute(stream, node, item, ia, prefix + "\t");
                return true;
            });

            item->visitItemAttributesAdded([this, &item, &stream, &node, &prefix](const CT_AbstractItemAttribute* ia) -> bool
            {
                writeAttribute(stream, node, item, ia, prefix + "\t");
                return true;
            });
        }

        return true;
    });

    ok = writeGeometry(stream, node, mesh, prefix + "\t");

    CT_TNodeGroup *component = node->rootComponent();

    if(component != nullptr)
        ok = recursiveWriteFile(stream, "decomp", component, prefix + "\t");

    for(CT_TNodeGroup* branch : node->branches())
        ok = recursiveWriteFile(stream, "branch", branch, prefix + "\t");

    if(component != nullptr)
    {
        while(component->successor() != nullptr)
        {
            component = component->successor();
            ok = recursiveWriteFile(stream, "follow", component, prefix + "\t");
        }
    }

    stream << prefix << "</" + type + ">" << Qt::endl;

    return ok;
}

bool PB_OPFExporter::writeAttribute(QTextStream &stream, const CT_TNodeGroup *node, const CT_AbstractSingularItemDrawable *l, const CT_AbstractItemAttribute *att, const QString &prefix)
{
    Q_UNUSED(node)

    stream << prefix << "<" + att->model()->displayableName() << ">" << att->toString(l, nullptr) << "</" << att->model()->displayableName() << ">" << Qt::endl;

    return true;
}

bool PB_OPFExporter::writeGeometry(QTextStream &stream, CT_TNodeGroup *node, const CT_MeshModel *mesh, const QString &prefix)
{
    if(mesh != nullptr)
    {
        stream << prefix << "<geometry class=\"Mesh\">" << Qt::endl;
        stream << prefix << "\t<shapeIndex>" << m_meshes.indexOf(mesh->mesh()) << "</shapeIndex>" << Qt::endl;

        stream << prefix << "\t<mat>" << Qt::endl;

        CT_TOPFNodeGroup *opfNode = dynamic_cast<CT_TOPFNodeGroup*>(node);

        QMatrix4x4 matrix;
        if (opfNode != nullptr)
        {
            matrix = opfNode->opfMatrix();
        }

        stream << prefix << "\t\t" << matrix(0,0)*100.0f << "\t" << matrix(0,1)*100.0f << "\t" << matrix(0,2)*100.0f << "\t" << matrix(0,3)*100.0f << Qt::endl;
        stream << prefix << "\t\t" << matrix(1,0)*100.0f << "\t" << matrix(1,1)*100.0f << "\t" << matrix(1,2)*100.0f << "\t" << matrix(1,3)*100.0f << Qt::endl;
        stream << prefix << "\t\t" << matrix(2,0)*100.0f << "\t" << matrix(2,1)*100.0f << "\t" << matrix(2,2)*100.0f << "\t" << matrix(2,3)*100.0f << Qt::endl;

        stream << prefix << "\t</mat>" << Qt::endl;

        const CT_OPFMeshModel *opfMeshModel = dynamic_cast<const CT_OPFMeshModel*>(mesh);

        if(opfMeshModel != nullptr)
        {
            stream << prefix << "\t<dUp>" << opfMeshModel->dUp() << "</dUp>" << Qt::endl;
            stream << prefix << "\t<dDwn>" << opfMeshModel->dDown() << "</dDwn>" << Qt::endl;
        }
        else
        {
            stream << prefix << "\t<dUp>1.0</dUp>" << Qt::endl;
            stream << prefix << "\t<dDwn>1.0</dDwn>" << Qt::endl;
        }

        stream << prefix << "</geometry>" << Qt::endl;
    }

    return true;
}

