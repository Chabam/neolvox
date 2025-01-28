#include "lvox3_grid3dexporter.h"

#include <math.h>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>

#include "ct_global/ct_context.h"

LVOX3_Grid3DExporter::LVOX3_Grid3DExporter(int subMenuLevel) : CT_AbstractExporter(subMenuLevel)
{
    addNewExportFormat(FileFormat("GRD3DLVOX", tr("LVOX 3D grid file format (ASCII)")));
    setToolTip(tr("Export of 3D grids in an ASCII format, reimplemented for LVOX3 with x,y,z resolution"));
}

LVOX3_Grid3DExporter::LVOX3_Grid3DExporter(const LVOX3_Grid3DExporter &other) : CT_AbstractExporter(other),
    mItems(other.mItems)
{
}

QString LVOX3_Grid3DExporter::displayableName() const
{
    return tr("3D grids, ACSII LVOX");
}

bool LVOX3_Grid3DExporter::isExportEachItemInSeparateFileOptionnal() const
{
    // we must export each item in a file (no choice for user)
    return false;
}


void LVOX3_Grid3DExporter::setItemsToExport(const QList<const LVOX3_AbstractGrid3D*>& items)
{
    setMustUseModels(false);
    mItems = items;
}

void LVOX3_Grid3DExporter::internalDeclareInputModels(CT_ExporterInModelStructureManager& manager)
{
    manager.addGroupToRootGroup(m_hInGroup);
    manager.addItemToGroup(m_hInGroup, m_hInItem, tr("Items to export"));
}


CT_AbstractExporter::ExportReturn LVOX3_Grid3DExporter::internalExportToFile()
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
        for(const LVOX3_AbstractGrid3D* item : mItems)
        {
            if(!exportItem(item, prePath, QString("_%1").arg(cpt)))
                return ErrorWhenExport;

            ++cpt;
        }
    }

    return NoMoreItemToExport;
}

void LVOX3_Grid3DExporter::clearIterators()
{
    mIteratorItemBegin = HandleItemType::const_iterator();
    mIteratorItemEnd = mIteratorItemBegin;
}

void LVOX3_Grid3DExporter::clearAttributesClouds()
{
}


bool LVOX3_Grid3DExporter::exportItem(const LVOX3_AbstractGrid3D* item, const QString& prePath, const QString& indice)
{
    const QString filePath = QString("%1%2.GRD3DLVOX").arg(prePath).arg(indice);

    QFile file(filePath);

    if(!file.open(QFile::Text | QFile::WriteOnly))
        return false;
    QTextStream stream(&file);

    // write header
    size_t xdim = item->xdim();
    size_t ydim = item->ydim();
    size_t zdim = item->zdim();

    stream << "ncols\t" << QVariant::fromValue(xdim).toString() << "\n";
    stream << "nrows\t" << QVariant::fromValue(ydim).toString() << "\n";
    stream << "nzlev\t" << QVariant::fromValue(zdim).toString() << "\n";

    stream << "xllcorner\t" << QVariant(item->minX()).toString() << "\n";
    stream << "yllcorner\t" << QVariant(item->minY()).toString() << "\n";
    stream << "zllcorner\t" << QVariant(item->minZ()).toString() << "\n";

    stream << "xcellsize\t" << QVariant(item->xresolution()).toString() << "\n";
    stream << "ycellsize\t" << QVariant(item->yresolution()).toString() << "\n";
    stream << "zcellsize\t" << QVariant(item->zresolution()).toString() << "\n";
    stream << "NODATA_value\t" << item->NAAsString() << "\n";

    stream << "datatype\t" << "float" << "\n";

    // write data
    for (size_t zz = 0 ; zz < zdim ; zz++)
    {
        for (size_t yy = 0 ; yy < ydim ; yy++)
        {
            for (size_t xx = 0 ; xx < xdim ; xx++)
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
