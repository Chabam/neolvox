#include "lvox_grid3dexporter.h"
#include "ct_itemdrawable/abstract/ct_abstractgrid3d.h"

#include <math.h>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>

#include "ct_global/ct_context.h"

LVOX_Grid3DExporter::LVOX_Grid3DExporter() : CT_AbstractExporter()
{
}

LVOX_Grid3DExporter::~LVOX_Grid3DExporter()
{

}

QString LVOX_Grid3DExporter::getExporterCustomName() const
{
    return tr("Grilles 3D, ACSII");
}

CT_StepsMenu::LevelPredefined LVOX_Grid3DExporter::getExporterSubMenuName() const
{
    return CT_StepsMenu::LP_Voxels;
}


void LVOX_Grid3DExporter::init()
{
    addNewExportFormat(FileFormat("GRD3D", tr("Fichiers Grilles 3D (ASCII)")));

    setToolTip(tr("Export des Grilles 3D au format ASCII, inspiré du format ASCII ESRI GRID pour les rasters (1 fichier par grille)"));
}

bool LVOX_Grid3DExporter::setItemDrawableToExport(const QList<CT_AbstractItemDrawable*> &list)
{
    clearErrorMessage();

    QList<CT_AbstractItemDrawable*> myList;
    QListIterator<CT_AbstractItemDrawable*> it(list);
    int nGrids = 0;

    while(it.hasNext())
    {
        CT_AbstractItemDrawable *item = it.next();
        if(dynamic_cast<CT_AbstractGrid3D*>(item) != NULL)
        {
            myList.append(item);
            ++nGrids;
        }
    }

    if(nGrids == 0)
    {
        setErrorMessage(tr("Aucun ItemDrawable du type CT_AbstractGrid3D"));
        return false;
    }

    return CT_AbstractExporter::setItemDrawableToExport(myList);
}

bool LVOX_Grid3DExporter::configureExport()
{
    if(!errorMessage().isEmpty())
    {
        QMessageBox::critical(NULL, tr("Erreur"), errorMessage(), QMessageBox::Ok);
        return false;
    }

    return true;
}

CT_AbstractExporter* LVOX_Grid3DExporter::copy() const
{
    return new LVOX_Grid3DExporter();
}

bool LVOX_Grid3DExporter::protectedExportToFile()
{
    bool ok = true;

    QFileInfo exportPathInfo = QFileInfo(exportFilePath());
    QString path = exportPathInfo.path();
    QString baseName = exportPathInfo.baseName();
    QString suffix = "GRD3D";

    QString indice = "";
    if (itemDrawableToExport().size() > 1) {indice = "_0";}
    int cpt = 0;

    QListIterator<CT_AbstractItemDrawable*> it(itemDrawableToExport());
    while (it.hasNext())
    {
        CT_AbstractGrid3D* item = dynamic_cast<CT_AbstractGrid3D*>(it.next());
        if (item != NULL)
        {
            QString filePath = QString("%1/%2%3.%4").arg(path).arg(baseName).arg(indice).arg(suffix);

            QFile file(filePath);

            if(file.open(QFile::Text | QFile::WriteOnly))
            {
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

                stream << "cellsize\t" << QVariant(item->resolution()).toString() << "\n";
                stream << "NODATA_value\t" << item->NAAsString() << "\n";

                // write data
                for (size_t zz = 0 ; zz < zdim ; zz++)
                {
                    for (size_t yy = 0 ; yy < ydim ; yy++)
                    {
                        for (size_t xx = 0 ; xx < xdim ; xx++)
                        {
                            size_t sIndex;
                            item->index(xx, ydim - yy - 1, zz, sIndex);
                            stream << item->valueAtIndexAsString(sIndex);

                            if (xx < xdim - 1) {stream << "\t";}
                        }
                        stream << "\n";
                    }
                    stream << "\n";
                }

                file.close();
            } else {ok = false;}
            indice = QString("_%1").arg(++cpt);
        }
    }

    return ok;
}
