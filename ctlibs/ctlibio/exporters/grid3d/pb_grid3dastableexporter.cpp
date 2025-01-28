#include "pb_grid3dastableexporter.h"

#include <QFileInfo>

PB_Grid3DAsTableExporter::PB_Grid3DAsTableExporter(int subMenuLevel) : SuperClass(subMenuLevel)
{
    addNewExportFormat(FileFormat("txt", tr("Fichiers txt")));
    setToolTip(tr("Exporte des grilles 3D au format table ASCII. Les champs suivants sont exportés :<br>"
                  "- X  : Coordonnée X de la cellule dans la grille (ligne)<br>"
                  "- Y  : Coordonnée Y de la cellule dans la grille (colonne)<br>"
                  "- Z  : Coordonnée Z de la cellule dans la grille (niveau Z)<br>"
                  "- Valeur  : Valeur dans la cellule<br>"));
}

PB_Grid3DAsTableExporter::PB_Grid3DAsTableExporter(const PB_Grid3DAsTableExporter& other) : SuperClass(other),
    mItems(other.mItems)
{
}


QString PB_Grid3DAsTableExporter::displayableName() const
{
    return tr("Grilles 3D, table (X,Y,Z,Valeur)");
}

bool PB_Grid3DAsTableExporter::isExportEachItemInSeparateFileOptionnal() const
{
    // we must export each item in a file (no choice for user)
    return false;
}

void PB_Grid3DAsTableExporter::setItemsToExport(const QList<const CT_AbstractGrid3D*>& items)
{
    setMustUseModels(false);
    mItems = items;
}

void PB_Grid3DAsTableExporter::internalDeclareInputModels(CT_ExporterInModelStructureManager& manager)
{
    manager.addGroupToRootGroup(m_hInGroup);
    manager.addItemToGroup(m_hInGroup, m_hInItem, tr("Item à exporter"));
}

CT_AbstractExporter::ExportReturn PB_Grid3DAsTableExporter::internalExportToFile()
{
    const QFileInfo exportPathInfo = QFileInfo(filePath());
    const QString prePath = exportPathInfo.path() + "/" + exportPathInfo.baseName();

    if(mustUseModels())
    {
        if(mIteratorItemBegin == mIteratorItemEnd)
        {
            auto iterator = m_hInItem.iterateInputs(m_handleResultExport);
            mIteratorItemBegin = iterator.begin();
            mIteratorItemEnd = iterator.end();
        }

        // write data
        if(mIteratorItemBegin != mIteratorItemEnd)
        {
            if(!exportItem(*mIteratorItemBegin, prePath, QString()))
                return ErrorWhenExport;

            ++mIteratorItemBegin;
        }

        return (mIteratorItemBegin == mIteratorItemEnd) ? NoMoreItemToExport : ExportCanContinue;
    }
    else
    {
        int cpt = 0;
        for(const CT_AbstractGrid3D* item : mItems)
        {
            if(!exportItem(item, prePath, QString("_%1").arg(cpt)))
                return ErrorWhenExport;

            ++cpt;
        }
    }

    return NoMoreItemToExport;
}

void PB_Grid3DAsTableExporter::clearIterators()
{
    mIteratorItemBegin = HandleItemType::const_iterator();
    mIteratorItemEnd = mIteratorItemBegin;
}

void PB_Grid3DAsTableExporter::clearAttributesClouds()
{
}

bool PB_Grid3DAsTableExporter::exportItem(const CT_AbstractGrid3D* item, const QString& prePath, const QString& indice)
{
    const QString filePath = QString("%1%2.asc").arg(prePath).arg(indice);

    QFile file(filePath);

    if(!file.open(QFile::Text | QFile::WriteOnly))
        return false;

    QTextStream stream(&file);

    // write header
    int xdim = item->xdim();
    int ydim = item->ydim();
    int zdim = item->zdim();

    stream << "Xcorner\tYcorner\tZcorner\tXcenter\tYcenter\tZcenter\tColX\tRowY\tLevZ\tValue\n";

    // write data
    for (int xx = 0 ; xx < xdim ; xx++)
    {
        for (int yy = 0 ; yy < ydim ; yy++)
        {
            for (int zz = 0 ; zz < zdim ; zz++)
            {
                size_t sIndex = 0;
                item->index(xx, yy, zz, sIndex);

                Eigen::Vector3d corner = Eigen::Vector3d(0.0, 0.0, 0.0);
                item->getCellBottomLeftCorner(xx, yy, zz, corner);

                stream << corner(0) << "\t";
                stream << corner(1) << "\t";
                stream << corner(2) << "\t";

                stream << item->getCellCenterX(xx) << "\t";
                stream << item->getCellCenterY(yy) << "\t";
                stream << item->getCellCenterZ(zz) << "\t";
                stream << xx << "\t";
                stream << yy << "\t";
                stream << zz << "\t";
                stream << item->valueAtIndexAsString(sIndex);
                stream << "\n";
            }
        }
    }

    file.close();

    return true;
}
