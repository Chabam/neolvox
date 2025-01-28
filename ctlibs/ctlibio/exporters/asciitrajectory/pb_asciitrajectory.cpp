#include "pb_asciitrajectory.h"

#include <QFileInfo>
#include <QFile>
#include <QTextStream>

#include "ct_tools/ct_numerictostringconversiont.h"
#include "ct_iterator/ct_pointiterator.h"

PB_AsciiTrajectory::PB_AsciiTrajectory(int subMenuLevel) : SuperClass(subMenuLevel)
{
    addNewExportFormat(FileFormat("asc", tr("Fichier asc")));

    setToolTip(tr("Exporte des trajectoires au format ASCII. Un fichier est créé pour chaque trajectoire, avec les champs suivants :<br>"
                  "- Easting : Coordonnée X<br>"
                  "- Northing : Coordonnée Y<br>"
                  "- Height : Coordonnée Z<br>"
                  "- Heading : Heading (degrees)<br>"
                  "- Roll : Roll (degrees)<br>"
                  "- Pitch : Pith (degrees)<br>"
                  ));
}

PB_AsciiTrajectory::PB_AsciiTrajectory(const PB_AsciiTrajectory& other) : SuperClass(other),
    mItems(other.mItems)
{
}

QString PB_AsciiTrajectory::displayableName() const
{
    return tr("Trajectoire ASCII (X,Y,Z, Heading, Roll, Pitch)");
}

void PB_AsciiTrajectory::setItemsToExport(const QList<const CT_ScanPath*>& items)
{
    setMustUseModels(false);
    mItems = items;
}

bool PB_AsciiTrajectory::isExportEachItemInSeparateFileOptionnal() const
{
    return false;
}

void PB_AsciiTrajectory::internalDeclareInputModels(CT_ExporterInModelStructureManager& manager)
{
    manager.addGroupToRootGroup(m_hInGroup);
    manager.addItemToGroup(m_hInGroup, m_traj, tr("Trajectoire"));
}

CT_AbstractExporter::ExportReturn PB_AsciiTrajectory::internalExportToFile()
{
    const QFileInfo exportPathInfo = QFileInfo(filePath());
    const QString path = exportPathInfo.path();
    const QString baseName = exportPathInfo.baseName();
    const QString suffix = "txt";

    const QString currentFilePath = QString("%1/%2.%4").arg(path, baseName, suffix);

    QFile file(currentFilePath);

    if(!file.open(QFile::WriteOnly | QFile::Text))
        return CT_AbstractExporter::ErrorWhenExport;

    QTextStream stream(&file);

    stream << "Time Easting Northing Height Heading Roll Pitch\n";

    if(mustUseModels())
    {
        if(mIteratorItemBegin == mIteratorItemEnd)
        {
            auto iterator = m_traj.iterateInputs(m_handleResultExport);
            mIteratorItemBegin = iterator.begin();
            mIteratorItemEnd = iterator.end();
            _nExported = 0;
        }
        const int totalToExport = maximumItemToExportInFile(int(std::distance(mIteratorItemBegin, mIteratorItemEnd)));

        // write data
        if (mIteratorItemBegin != mIteratorItemEnd)
        {
            const CT_ScanPath* item = *mIteratorItemBegin;

            exportItem(item, stream, _nExported, totalToExport);

            _nExported++;
            ++mIteratorItemBegin;
        }

        file.close();
        return (mIteratorItemBegin == mIteratorItemEnd) ? NoMoreItemToExport : ExportCanContinue;
    }
    else
    {
        const int totalToExport = mItems.size();
        int _nExported = 0;

        for(const CT_ScanPath* item : qAsConst(mItems))
        {
            exportItem(item, stream, _nExported, totalToExport);
            _nExported++;
        }
    }
    file.close();

    return NoMoreItemToExport;
}

void PB_AsciiTrajectory::clearIterators()
{
    mIteratorItemBegin = HandleItemType::const_iterator();
    mIteratorItemEnd = mIteratorItemBegin;
}

void PB_AsciiTrajectory::clearAttributesClouds()
{
}

void PB_AsciiTrajectory::exportItem(const CT_ScanPath *item, QTextStream &stream, const int& nExported, const int& totalToExport)
{

    const QList<CT_ScanPath::PathPoint>& points = item->getPath();

    for (const CT_ScanPath::PathPoint &point : points)
    {
        Eigen::Vector3d position, orientation;
        position = point._position;
        orientation = point._orientation;
        stream << QString::number(point._gpsTime, 'f', 4) << " ";
        stream << QString::number(position(0), 'f', 4) << " ";
        stream << QString::number(position(1), 'f', 4) << " ";
        stream << QString::number(position(2), 'f', 4) << " ";
        stream << QString::number(orientation(0), 'f', 4) << " ";
        stream << QString::number(orientation(1), 'f', 4) << " ";
        stream << QString::number(orientation(2), 'f', 4) << "\n";
    }

    setExportProgress(int(100*float(nExported)/float(totalToExport)));
}
