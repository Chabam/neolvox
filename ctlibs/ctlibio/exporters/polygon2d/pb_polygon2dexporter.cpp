#include "pb_polygon2dexporter.h"

#include <QFileInfo>

PB_Polygon2DExporter::PB_Polygon2DExporter(int subMenuLevel) : SuperClass(subMenuLevel)
{
    addNewExportFormat(FileFormat("txt", tr("Polygones 2D (ASCII)")));
    setToolTip(tr("Exporte des Polygones 2D en format ASCII. Le fichier de sortie contient une ligne par polygones.<br>"
                  "Pour chaque polygone, sont exportés :<br>"
                  "- PolygonId : Identifiant Computree du polygone<br>"
                  "- AreaOfPolygon : Aire du polygone<br>"
                  "- Xcenter : Coordonnée X du centre de masse du polygone<br>"
                  "- Ycenter : Coordonnée Y du centre de masse du polygone<br>"
                  "- NumberOfVertice(N) : Nombre de sommets du polygone<br>"
                  "- Xvertice1 : Coordonnée X du 1er sommet<br>"
                  "- Yvertice1 : Coordonnée Y du 1er sommet<br>"
                  "- Xvertice2 : Coordonnée X du 2ième sommet<br>"
                  "- Yvertice2 : Coordonnée Y du 2ième sommet<br>"
                  "..."
                  "- XverticeN : Coordonnée X du Nième sommet<br>"
                  "- YverticeN : Coordonnée Y du Nième sommet<br>"
                  "N.B. : Le nombre de colonnes varie donc pour chaque ligne. "));
}

PB_Polygon2DExporter::PB_Polygon2DExporter(const PB_Polygon2DExporter& other) : SuperClass(other),
    mItems(other.mItems)
{
}

QString PB_Polygon2DExporter::displayableName() const
{
    return tr("Polygones 2D, ACSII");
}

void PB_Polygon2DExporter::setItemsToExport(const QList<const CT_Polygon2D*>& items)
{
    setMustUseModels(false);
    mItems = items;
}

void PB_Polygon2DExporter::internalDeclareInputModels(CT_ExporterInModelStructureManager& manager)
{
    manager.addGroupToRootGroup(m_hInGroup);
    manager.addItemToGroup(m_hInGroup, m_hInItem, tr("Item à exporter"));
}

CT_AbstractExporter::ExportReturn PB_Polygon2DExporter::internalExportToFile()
{
    const QFileInfo exportPathInfo = QFileInfo(filePath());
    const QString path = exportPathInfo.path();
    const QString baseName = exportPathInfo.baseName();
    const QString suffix = "txt";

    const QString currentFilePath = QString("%1/%2.%4").arg(path).arg(baseName).arg(suffix);

    QFile file(currentFilePath);

    if(!file.open(QFile::WriteOnly | QFile::Text))
        return ErrorWhenExport;

    QTextStream txtStream(&file);

    // write header
    txtStream << "# 2D Polygons export from Computree v3\n";
    txtStream << "# Format:\n";
    txtStream << "PolygonId\tAreaOfPolygon\tXcenter\tYcenter\tNumberOfVertice(N)\tXvertice1\tYvertice1\tXvertice2\tYvertice2\t...\tXverticeN\tYverticeN\n";

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
        while((mIteratorItemBegin != mIteratorItemEnd)
              && (nExported < end))
        {
            const CT_Polygon2D* item = *mIteratorItemBegin;

            exportItem(item, txtStream, nExported, totalToExport);

            nExported += 100;
            ++mIteratorItemBegin;
        }

        file.close();
        return (mIteratorItemBegin == mIteratorItemEnd) ? NoMoreItemToExport : ExportCanContinue;
    }
    else
    {
        const int totalToExport = mItems.size();
        int nExported = 0;

        for(const CT_Polygon2D* item : mItems)
        {
            exportItem(item, txtStream, nExported, totalToExport);
            nExported += 100;
        }
    }

    file.close();

    return NoMoreItemToExport;
}

void PB_Polygon2DExporter::clearIterators()
{
    mIteratorItemBegin = HandleItemType::const_iterator();
    mIteratorItemEnd = mIteratorItemBegin;
}

void PB_Polygon2DExporter::clearAttributesClouds()
{
}

void PB_Polygon2DExporter::exportItem(const CT_Polygon2D* item, QTextStream& stream, const int& nExported, const int& totalToExport)
{
    Q_UNUSED(nExported);
    Q_UNUSED(totalToExport);

    item->computeCentroid();

    stream << item->id() << "\t";
    stream << item->getArea() << "\t";
    stream << item->centerX() << "\t";
    stream << item->centerY() << "\t";
    stream << item->getNumberOfVertices();

    if (item->getNumberOfVertices() > 0)
    {
        const QVector<Eigen::Vector2d>& vertices = item->getVertices();

        for (int i = 0 ; i < vertices.size() ; i++)
        {
            const Eigen::Vector2d& vertice = vertices.at(i);

            stream << "\t";
            stream << vertice(0) << "\t";
            stream << vertice(1);
        }
    }
    stream << "\n";
}
