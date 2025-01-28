#include "pb_grid3dexporter.h"

#include <QFileInfo>

PB_Grid3DExporter::PB_Grid3DExporter(int subMenuLevel) : SuperClass(subMenuLevel)
{
    addNewExportFormat(FileFormat("GRD3D", tr("Fichiers Grilles 3D (ASCII)")));
    setToolTip(tr("Export des Grilles 3D au format ASCII, inspiré du format ASCII ESRI GRID pour les rasters (1 fichier par grille)"));
}

PB_Grid3DExporter::PB_Grid3DExporter(const PB_Grid3DExporter& other) : SuperClass(other),
    mItems(other.mItems)
{
}

QString PB_Grid3DExporter::displayableName() const
{
    return tr("Grilles 3D, ACSII");
}

bool PB_Grid3DExporter::isExportEachItemInSeparateFileOptionnal() const
{
    // we must export each item in a file (no choice for user)
    return false;
}

void PB_Grid3DExporter::setItemsToExport(const QList<const CT_AbstractGrid3D*>& items)
{
    setMustUseModels(false);
    mItems = items;
}

void PB_Grid3DExporter::internalDeclareInputModels(CT_ExporterInModelStructureManager& manager)
{
    manager.addGroupToRootGroup(m_hInGroup);
    manager.addItemToGroup(m_hInGroup, m_hInItem, tr("Item à exporter"));
}

CT_AbstractExporter::ExportReturn PB_Grid3DExporter::internalExportToFile()
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

void PB_Grid3DExporter::clearIterators()
{
    mIteratorItemBegin = HandleItemType::const_iterator();
    mIteratorItemEnd = mIteratorItemBegin;
}

void PB_Grid3DExporter::clearAttributesClouds()
{
}

bool PB_Grid3DExporter::exportItem(const CT_AbstractGrid3D* item, const QString& prePath, const QString& indice)
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

    stream << "ncols\t" << QVariant::fromValue(xdim).toString() << "\n";
    stream << "nrows\t" << QVariant::fromValue(ydim).toString() << "\n";
    stream << "nzlev\t" << QVariant::fromValue(zdim).toString() << "\n";

    stream << "xllcorner\t" << QVariant(item->minX()).toString() << "\n";
    stream << "yllcorner\t" << QVariant(item->minY()).toString() << "\n";
    stream << "zllcorner\t" << QVariant(item->minZ()).toString() << "\n";

    stream << "cellsize\t" << QVariant(item->resolution()).toString() << "\n";
    stream << "NODATA_value\t" << item->NAAsString() << "\n";

    // write data
    for (int zz = 0 ; zz < zdim ; zz++)
    {
        for (int yy = 0 ; yy < ydim ; yy++)
        {
            for (int xx = 0 ; xx < xdim ; xx++)
            {
                size_t sIndex = 0;
                item->index(xx, ydim - yy - 1, zz, sIndex);
                stream << item->valueAtIndexAsString(sIndex);

                if (xx < xdim - 1) {stream << "\t";}
            }
            stream << "\n";
        }
        stream << "\n";
    }

    file.close();


    return true;
}
