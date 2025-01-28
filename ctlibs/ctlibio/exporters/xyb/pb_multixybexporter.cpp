#include "pb_multixybexporter.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QEventLoop>
#include <QApplication>
#include <QProgressDialog>
#include <QFileInfo>

#include "ct_global/ct_context.h"
#include "ct_itemdrawable/abstract/ct_abstractpointsattributes.h"
#include "ct_itemdrawable/ct_scene.h"
#include "ct_colorcloud/abstract/ct_abstractcolorcloud.h"
#include "ct_coordinates/tools/ct_coordinatesystemmanager.h"
#include "ct_iterator/ct_pointiterator.h"
#include "ct_point.h"

PB_MultiXYBExporter::PB_MultiXYBExporter() : CT_AbstractExporterPointAttributesSelection()
{
    setCanExportItems(true);
    setCanExportPoints(false); // TODO : verify what this is set to false ??!! or what it inherit from CT_AbstractExporterPointAttributesSelection ??!!
    setCanExportColors(true);
    setCanExportNormals(false);
}

QString PB_MultiXYBExporter::getExporterCustomName() const
{
    return tr("Points (1 file/scene), XYB (X,Y,Z,I)");
}

CT_StepsMenu::LevelPredefined PB_MultiXYBExporter::getExporterSubMenuName() const
{
    return CT_StepsMenu::LP_Points;
}

void PB_MultiXYBExporter::init()
{
    addNewExportFormat(FileFormat("xyb", tr("Fichiers .xyb (1 fichier/scène)")));

    setToolTip(tr("Export des points contenus dans plusieurs items au format binaire XYB (FARO) : 1 fichier par item"));
}

bool PB_MultiXYBExporter::setItemDrawableToExport(const QList<CT_AbstractItemDrawable *> &list)
{
    QList<CT_AbstractItemDrawable*> myList;
    QListIterator<CT_AbstractItemDrawable*> it(list);

    while(it.hasNext())
    {
        CT_AbstractItemDrawable *item = it.next();

        if(dynamic_cast<CT_IAccessPointCloud*>(item) != NULL)
            myList.append(item);
    }

    if(myList.isEmpty())
    {
        setErrorMessage(errorMessage() + "\r\n" + tr("- Aucun ItemDrawable du type CT_IAccessPointCloud"));
        return false;
    }

    return CT_AbstractExporter::setItemDrawableToExport(myList);
}

bool PB_MultiXYBExporter::protectedExportToFile()
{
    QFileInfo exportPathInfo = QFileInfo(exportFilePath());
    QString path = exportPathInfo.path();
    QString baseName = exportPathInfo.baseName();
    QString suffix = "xyb";

    // Loop on the items
    int cpt = 0;
    int size = itemDrawableToExport().size();
    QListIterator<CT_AbstractItemDrawable*> it(itemDrawableToExport());
    while(it.hasNext())
    {
        CT_AbstractItemDrawable *item = it.next();

        QString filePath = QString("%1/%2_%4.%3").arg(path).arg(baseName).arg(suffix).arg(item->displayableName());
        QFile file(filePath);

        if(file.open(QFile::WriteOnly | QFile::Text))
        {
            QTextStream txtStream(&file);
            txtStream << "# SCENE XYZ binary format v1.0\n";
            txtStream << "ScanPosition 0.00000000 0.00000000 0.00000000 \n";
            txtStream << "Rows 0\n";
            txtStream << "Cols 0\n";
            file.close();
        }

        if(file.open(QFile::Append))
        {
            CT_AbstractColorCloud *cc = createColorCloudBeforeExportToFile();

            QDataStream stream(&file);
            stream.setByteOrder(QDataStream::LittleEndian);

            char d_data[8];

            // write header
            d_data[0] = 0;
            d_data[1] = 0;
            d_data[2] = 0;
            d_data[3] = 0;

            stream.writeRawData(d_data, 4);

            // write data
            exportPoints(stream, dynamic_cast<CT_IAccessPointCloud*>(item)->getPointCloudIndex(), cc);

            file.close();

            PS_LOG->addMessage(LogInterface::info, LogInterface::exporter, tr("Fichier %1 créé").arg(filePath));

        } else {
            PS_LOG->addMessage(LogInterface::error, LogInterface::exporter, tr("Errreur : impossible de créer le fichier %1").arg(filePath));
        }

        setExportProgress(100.0*(float)cpt/(float)size);
    }

    return true;
}

void PB_MultiXYBExporter::exportPoints(QDataStream &stream,
                                  const CT_AbstractPointCloudIndex *constPCIndex,
                                  const CT_AbstractColorCloud *cc)
{
    CT_PointIterator it(constPCIndex);

    while(it.hasNext())
    {
        const CT_Point &point = it.next().currentPoint();

        stream << point(CT_Point::X);
        stream << point(CT_Point::Y);
        stream << point(CT_Point::Z);

        if(cc == NULL)
        {
            stream << (quint16)0;
        }
        else
        {
            const CT_Color &col = cc->constColorAt(it.cIndex());
            quint16 tmp = (quint16)col.r();
            stream << tmp;
        }
    }
}

