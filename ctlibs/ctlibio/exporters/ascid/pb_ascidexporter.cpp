#include "pb_ascidexporter.h"

#include <QFileInfo>
#include <QFile>
#include <QTextStream>

#include "ct_tools/ct_numerictostringconversiont.h"
#include "ct_iterator/ct_pointiterator.h"

PB_ASCIDExporter::PB_ASCIDExporter(int subMenuLevel) : SuperClass(subMenuLevel)
{
    addNewExportFormat(FileFormat("asc", tr("Fichier asc")));

    setToolTip(tr("Exporte les points au format ASCII. Tous les items contenant des points sont exportés dans le même fichier, avec les champs suivants :<br>"
                  "- ID : ID Computree de l'item contenant le point<br>"
                  "- X  : Coordonnée X<br>"
                  "- Y  : Coordonnée Y<br>"
                  "- Z  : Coordonnée Z<br>"));
}

PB_ASCIDExporter::PB_ASCIDExporter(const PB_ASCIDExporter& other) : SuperClass(other),
    mItems(other.mItems)
{
}

QString PB_ASCIDExporter::displayableName() const
{
    return tr("Points + ID item, ASCII(X,Y,Z)");
}

void PB_ASCIDExporter::setItemsToExport(const QList<const CT_AbstractItemDrawableWithPointCloud*>& items)
{
    setMustUseModels(false);
    mItems = items;
}

void PB_ASCIDExporter::internalDeclareInputModels(CT_ExporterInModelStructureManager& manager)
{
    manager.addGroupToRootGroup(m_hInGroup);
    manager.addItemToGroup(m_hInGroup, m_hInItem, tr("Item à exporter"));
    manager.addItemAttribute(m_hInItem, m_hInItemAttribute, QStringList() << CT_AbstractCategory::DATA_ID << CT_AbstractCategory::DATA_VALUE, tr("ID"));
}

CT_AbstractExporter::ExportReturn PB_ASCIDExporter::internalExportToFile()
{
    const QFileInfo exportPathInfo = QFileInfo(filePath());
    const QString path = exportPathInfo.path();
    const QString baseName = exportPathInfo.baseName();
    const QString suffix = "asc";

    const QString currentFilePath = QString("%1/%2.%4").arg(path).arg(baseName).arg(suffix);

    QFile file(currentFilePath);

    if(!file.open(QFile::WriteOnly | QFile::Text))
        return ErrorWhenExport;

    QTextStream txtStream(&file);

    txtStream << "ID\tX\tY\tZ\n";

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
            const CT_AbstractItemDrawableWithPointCloud* item = *mIteratorItemBegin;

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

        for(const CT_AbstractItemDrawableWithPointCloud* item : mItems)
        {
            exportItem(item, txtStream, nExported, totalToExport);
            nExported += 100;
        }
    }

    file.close();

    return NoMoreItemToExport;
}

void PB_ASCIDExporter::clearIterators()
{
    mIteratorItemBegin = HandleItemType::const_iterator();
    mIteratorItemEnd = mIteratorItemBegin;
}

void PB_ASCIDExporter::clearAttributesClouds()
{
}

void PB_ASCIDExporter::exportItem(const CT_AbstractItemDrawableWithPointCloud* item, QTextStream& stream, const int& nExported, const int& totalToExport)
{
    auto itemAttribute = item->itemAttribute(m_hInItemAttribute);
    const QString id = (itemAttribute != nullptr) ? itemAttribute->toString(item, nullptr) : QString().setNum(item->id());

    CT_PointIterator itP(item->pointCloudIndex());

    const size_t totalSize = itP.size();
    size_t i = 0;

    while(itP.hasNext())
    {
        const CT_Point& point = itP.next().currentPoint();

        stream << id << "\t";
        stream << CT_NumericToStringConversionT<double>::toString(point(0)) << "\t";
        stream << CT_NumericToStringConversionT<double>::toString(point(1)) << "\t";
        stream << CT_NumericToStringConversionT<double>::toString(point(2)) << "\n";

        ++i;

        setExportProgress(int((((i*100)/totalSize)+size_t(nExported))/size_t(totalToExport)));
    }
}
