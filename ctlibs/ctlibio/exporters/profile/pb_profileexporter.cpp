#include "pb_profileexporter.h"

#include <QFileInfo>

PB_ProfileExporter::PB_ProfileExporter(int subMenuLevel) : SuperClass(subMenuLevel)
{
    addNewExportFormat(FileFormat("hist", tr("Fichiers Profile (ASCII)")));
    setToolTip(tr("Export de profils au format ASCII (1 fichier par profil).<br>"
                  "Le fichier comporte d'abord une entête inspirée du format ASCII ESRI GRID pour les rasters, décrivant l'origine, la direction et la résolution du profil.<br>"
                  "Ensuite, une table fournit les champs suivants :<br>"
                  "- Level : Numéro de la cellule dans le profil<br>"
                  "- Length : Longueur correspondant au centre de la cellule le long du profil<br>"
                  "- Xc : Coordonnée X du centre de la cellule<br>"
                  "- Yc : Coordonnée Y du centre de la cellule<br>"
                  "- Zc : Coordonnée Z du centre de la cellule<br>"
                  "- Value : Valeur de la cellule"));
}

PB_ProfileExporter::PB_ProfileExporter(const PB_ProfileExporter& other) : SuperClass(other),
    mItems(other.mItems)
{
}


QString PB_ProfileExporter::displayableName() const
{
    return tr("Profils, ACSII");
}

bool PB_ProfileExporter::isExportEachItemInSeparateFileOptionnal() const
{
    // we must export each item in a file (no choice for user)
    return false;
}

void PB_ProfileExporter::setItemsToExport(const QList<const CT_AbstractProfile*>& items)
{
    setMustUseModels(false);
    mItems = items;
}

void PB_ProfileExporter::internalDeclareInputModels(CT_ExporterInModelStructureManager& manager)
{
    manager.addGroupToRootGroup(m_hInGroup);
    manager.addItemToGroup(m_hInGroup, m_hInItem, tr("Item à exporter"));
}

CT_AbstractExporter::ExportReturn PB_ProfileExporter::internalExportToFile()
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
        for(const CT_AbstractProfile* item : mItems)
        {
            if(!exportItem(item, prePath, QString("_%1").arg(cpt)))
                return ErrorWhenExport;

            ++cpt;
        }
    }

    return NoMoreItemToExport;
}

void PB_ProfileExporter::clearIterators()
{
    mIteratorItemBegin = HandleItemType::const_iterator();
    mIteratorItemEnd = mIteratorItemBegin;
}

void PB_ProfileExporter::clearAttributesClouds()
{
}

bool PB_ProfileExporter::exportItem(const CT_AbstractProfile* item, const QString& prePath, const QString& indice)
{
    const QString filePath = QString("%1%2.asc").arg(prePath).arg(indice);

    QFile file(filePath);

    if(!file.open(QFile::Text | QFile::WriteOnly))
        return false;

    QTextStream stream(&file);

    // write header
    size_t dim = item->nCells();

    stream << "ItemName\t" << item->displayableName() << "\n";
    stream << "Nlevels\t" << dim << "\n";

    stream << "Xorigin\t" << item->minX() << "\n";
    stream << "Yorigin\t" << item->minY() << "\n";
    stream << "Zorigin\t" << item->minZ() << "\n";

    const Eigen::Vector3d &direction = item->getDirection();

    stream << "Xdirection\t" << direction(0) << "\n";
    stream << "Ydirection\t" << direction(1) << "\n";
    stream << "Zdirection\t" << direction(2) << "\n";

    stream << "Resolution\t" << item->resolution() << "\n";
    stream << "NODATA_value\t" << item->NAAsString() << "\n";

    stream << "Level\tLength\tXc\tYc\tZc\tValue\n";

    // write data
    for (size_t index = 0 ; index < dim ; index++)
    {
        Eigen::Vector3d cellCenter;
        item->getCellCenterXYZ(index, cellCenter);

        stream << index << "\t";
        stream << item->lengthForIndex(index) << "\t";
        stream << cellCenter(0) << "\t";
        stream << cellCenter(1) << "\t";
        stream << cellCenter(2) << "\t";
        stream << item->valueAtIndexAsString(index) << "\n";
    }

    file.close();


    return true;
}
