#include "pb_meshobjexporter.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>

#include "ct_mesh/ct_face.h"
#include "ct_iterator/ct_pointiterator.h"
#include "ct_iterator/ct_faceiterator.h"

PB_MeshObjExporter::PB_MeshObjExporter(int subMenuLevel) : SuperClass(subMenuLevel)
{
    addNewExportFormat(FileFormat("obj", tr("Fichiers .obj")));

    setToolTip(tr("Exporte un maillage dans un fichier au format OBJ (Objet 3D)"));
}

PB_MeshObjExporter::PB_MeshObjExporter(const PB_MeshObjExporter& other) : SuperClass(other),
    mMeshes(other.mMeshes)
{
}

QString PB_MeshObjExporter::displayableName() const
{
    return tr("Maillage 3D (Mesh), format OBJ");
}

void PB_MeshObjExporter::setMeshesToExport(const QList<const CT_MeshModel*>& list)
{
    setMustUseModels(false);
    mMeshes = list;
}

void PB_MeshObjExporter::internalDeclareInputModels(CT_ExporterInModelStructureManager& manager)
{
    manager.addGroupToRootGroup(m_hInGroup);
    manager.addItemToGroup(m_hInGroup, m_hInItem, tr("Maillage à exporter"));
}

CT_AbstractExporter::ExportReturn PB_MeshObjExporter::internalExportToFile()
{
    const QFileInfo exportPathInfo = QFileInfo(filePath());
    const QString path = exportPathInfo.path();
    const QString baseName = exportPathInfo.baseName();
    const QString suffix = "obj";

    const QString currentFilePath = QString("%1/%2.%4").arg(path).arg(baseName).arg(suffix);

    QFile file(currentFilePath);

    if(file.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream txtStream(&file);

        txtStream << "# " << QFileInfo(currentFilePath).fileName() << Qt::endl << Qt::endl;

        if(mustUseModels())
        {
            if(mIteratorItemBegin == mIteratorItemEnd)
            {
                auto iterator = m_hInItem.iterateInputs(m_handleResultExport);
                mIteratorItemBegin = iterator.begin();
                mIteratorItemEnd = iterator.end();
            }

            int nExported = 0;
            const int totalToExport = maximumItemToExportInFile(int(std::distance(mIteratorItemBegin, mIteratorItemEnd)));
            const int end = totalToExport*100;

            // write data
            size_t lastPointCount = 0;
            size_t ptNb = 0;

            while((mIteratorItemBegin != mIteratorItemEnd)
                  && (nExported < end))
            {
                const CT_MeshModel* item = *mIteratorItemBegin;

                exportMesh(item, txtStream, nExported, totalToExport, ptNb, lastPointCount);

                nExported += 100;
                ++mIteratorItemBegin;
            }

            file.close();
            return (mIteratorItemBegin == mIteratorItemEnd) ? NoMoreItemToExport : ExportCanContinue;
        }
        else
        {
            const int totalToExport = mMeshes.size();
            int nExported = 0;

            size_t lastPointCount = 0;
            size_t ptNb = 0;

            for(const CT_MeshModel* item : mMeshes)
            {
                exportMesh(item, txtStream, nExported, totalToExport, ptNb, lastPointCount);
                nExported += 100;
            }
        }

        file.close();
    }

    return NoMoreItemToExport;
}

void PB_MeshObjExporter::clearIterators()
{
    mIteratorItemBegin = HandleItemType::const_iterator();
    mIteratorItemEnd = mIteratorItemBegin;
}

void PB_MeshObjExporter::clearAttributesClouds()
{
}

bool PB_MeshObjExporter::exportMesh(const CT_MeshModel* item, QTextStream& stream, const int& nExported, const int& totalToExport, size_t& ptNb, size_t& lastPointCount)
{
    stream << "o " << item->id() << Qt::endl << Qt::endl;

    CT_Mesh* mesh = item->mesh();

    if(mesh != nullptr)
    {
        CT_PointIterator itP(mesh->abstractVert());
        CT_FaceIterator itF(mesh->abstractFace());

        QHash<size_t, size_t> hashTablePoint;

        size_t totalSize = itP.size();
        size_t i = 0;

        while(itP.hasNext())
        {
            const CT_Point &point = itP.next().currentPoint();
            hashTablePoint.insert(itP.cIndex(), i+1);

            stream << "v " << double(point(0)) << " " << double(point(1)) << " " << double(point(2)) << Qt::endl;

            ++i;
            ++ptNb;

            setExportProgress((((int(i)*50)/int(totalSize))+nExported)/totalToExport);
        }

        stream << Qt::endl;

        totalSize = itF.size();
        i = 0;

        while(itF.hasNext())
        {
            const CT_Face &face = itF.next().cT();

            stream << "f " << (hashTablePoint.value(face.iPointAt(0)) + lastPointCount) << " " << (hashTablePoint.value(face.iPointAt(1)) + lastPointCount) << " " << (hashTablePoint.value(face.iPointAt(2)) + lastPointCount) << Qt::endl;

            ++i;

            setExportProgress(((50+((int(i)*50)/int(totalSize)))+nExported)/totalToExport);
        }
    }

    stream << Qt::endl;
    lastPointCount = ptNb;

    return true;
}
