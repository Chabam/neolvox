#include "pb_grid2dexporter.h"

#include <QFileInfo>

PB_Grid2DExporter::PB_Grid2DExporter(int subMenuLevel) : SuperClass(subMenuLevel)
{
    addNewExportFormat(FileFormat("ASC", tr("Fichiers Grid 2D (ASCII)")));
    setToolTip(tr("Export des rasters au format ASCII ESRI Grid  (1 fichier par raster)"));
}

PB_Grid2DExporter::PB_Grid2DExporter(const PB_Grid2DExporter& other) : SuperClass(other),
    mItems(other.mItems)
{
}

QString PB_Grid2DExporter::displayableName() const
{
    return tr("Grilles 2D, ACSII ESRI Grid");
}

bool PB_Grid2DExporter::isExportEachItemInSeparateFileOptionnal() const
{
    // we must export each item in a file (no choice for user)
    return false;
}

void PB_Grid2DExporter::setItemsToExport(const QList<const CT_AbstractImage2D*>& items)
{
    setMustUseModels(false);
    mItems = items;
}

void PB_Grid2DExporter::internalDeclareInputModels(CT_ExporterInModelStructureManager& manager)
{
    manager.addGroupToRootGroup(m_hInGroup);
    manager.addItemToGroup(m_hInGroup, m_hInItem, tr("Item à exporter"));
}

CT_AbstractExporter::ExportReturn PB_Grid2DExporter::internalExportToFile()
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
        for(const CT_AbstractImage2D* item : mItems)
        {
            if(!exportItem(item, prePath, QString("_%1").arg(cpt)))
                return ErrorWhenExport;

            ++cpt;
        }
    }

    return NoMoreItemToExport;
}

void PB_Grid2DExporter::clearIterators()
{
    mIteratorItemBegin = HandleItemType::const_iterator();
    mIteratorItemEnd = mIteratorItemBegin;
}

void PB_Grid2DExporter::clearAttributesClouds()
{
}

bool PB_Grid2DExporter::exportItem(const CT_AbstractImage2D* item, const QString& prePath, const QString& indice)
{
    const QString filePath = QString("%1%2.asc").arg(prePath).arg(indice);

    QFile file(filePath);

    if(!file.open(QFile::Text | QFile::WriteOnly))
        return false;

    QTextStream stream(&file);

    // write header
    const int xdim = item->xdim();
    const int ydim = item->ydim();

    stream << "ncols\t" << QVariant::fromValue(xdim).toString() << "\n";
    stream << "nrows\t" << QVariant::fromValue(ydim).toString() << "\n";

    stream << "xllcorner\t" << QVariant(item->minX()).toString() << "\n";
    stream << "yllcorner\t" << QVariant(item->minY()).toString() << "\n";

    stream << "cellsize\t" << QVariant(item->resolution()).toString() << "\n";
    stream << "NODATA_value\t" << item->NAAsString() << "\n";

    // write data
    for (int yy = 0 ; yy < ydim ; yy++)
    {
        for (int xx = 0 ; xx < xdim ; xx++)
        {
            size_t sIndex = -1; // Default value before initialization in 'index'
            item->index(xx, yy, sIndex);
            stream << item->valueAtIndexAsString(sIndex);

            if (xx < xdim - 1)
                stream << "\t";
        }

        stream << "\n";
    }

    file.close();

    return true;
}
