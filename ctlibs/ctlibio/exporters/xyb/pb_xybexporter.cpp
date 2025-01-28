#include "pb_xybexporter.h"

#include <limits>

#include "private/pb_xybpiecebypieceprivateexporter.h"

#include "ct_itemdrawable/abstract/ct_abstractpointattributesscalar.h"
#include "ct_itemdrawable/ct_pointsattributescolor.h"

#include "ct_colorcloud/abstract/ct_abstractcolorcloud.h"

PB_XYBExporter::PB_XYBExporter(int subMenuLevel) : SuperClass(subMenuLevel),
    mScanner(nullptr),
    mPointsAttributeOfTypeColorOrScalar(nullptr)
{
    setCanExportPoints(true);
    addNewExportFormat(FileFormat("xyb", tr("Fichiers binaire de points .xyb")));
    setToolTip(tr("Export des points dans un fichier format binaire XYB (FARO)"));
}

PB_XYBExporter::PB_XYBExporter(const PB_XYBExporter& other) : SuperClass(other)
{
    mScanner = nullptr;
}

QString PB_XYBExporter::displayableName() const
{
    return tr("Export de points, XYB (X,Y,Z,I)");
}

bool PB_XYBExporter::setPointsToExport(const QList<CT_AbstractCloudIndex*> &list)
{
    QList<CT_AbstractCloudIndex*> myList;
    QListIterator<CT_AbstractCloudIndex*> it(list);

    while(it.hasNext())
    {
        CT_AbstractCloudIndex *item = it.next();

        if(dynamic_cast<CT_AbstractPointCloudIndex*>(item) != nullptr)
            myList.append(item);
    }

    if(myList.isEmpty())
    {
        setErrorMessage(errorMessage() + "\r\n" + tr("- Aucun points à exporter"));
        return false;
    }

    return SuperClass::setPointsToExport(myList);
}

void PB_XYBExporter::setItemsToExport(const QList<const CT_AbstractItemDrawableWithPointCloud*>& items,
                                      const CT_Scanner* scanner,
                                      const CT_AbstractPointsAttributes* pointsAttributeOfTypeColorOrScalar)
{
    setMustUseModels(false);

    m_items = items;
    mScanner = scanner;
    mPointsAttributeOfTypeColorOrScalar = pointsAttributeOfTypeColorOrScalar;
}

bool PB_XYBExporter::canExportPieceByPiece() const
{
    return true;
}

CT_AbstractPieceByPieceExporter* PB_XYBExporter::createPieceByPieceExporter(const QString& outputFilepath)
{
    return new PB_XYBPieceByPiecePrivateExporter(*this, outputFilepath);
}

void PB_XYBExporter::internalDeclareInputModels(CT_ExporterInModelStructureManager& manager)
{
    manager.addGroupToRootGroup(m_hInGroup);
    manager.addItemToGroup(m_hInGroup, m_hInItem, tr("Item à exporter"));
    manager.addItemToGroup(m_hInGroup, m_hInScanner, tr("Scanner"));
    manager.addPointAttributeToGroup(m_hInGroup, m_hInPointsAttribute, tr("Couleurs"));
}

CT_AbstractExporter::ExportReturn PB_XYBExporter::internalExportToFile()
{
    QList<CT_AbstractPieceByPieceExporter*> exporters;
    exporters << createPieceByPieceExporter(filePath());

    CT_AbstractExporter::ExportReturn ret = ErrorWhenExport;
    if((ret = exportOnePieceOfDataToFiles(exporters)) != ErrorWhenExport)
    {
        if(!finalizePieceByPieceExport(exporters))
            return ErrorWhenExport;
    }

    return ret;
}

CT_AbstractExporter::ExportReturn PB_XYBExporter::internalExportOnePiece(const QList<CT_AbstractPieceByPieceExporter*>& pieceByPieceExporters)
{
    if(mustUseModels())
    {
        mPointsAttributeOfTypeColorOrScalar = nullptr;

        const CT_AbstractPointsAttributes* pA = m_hInPointsAttribute.firstInput(m_handleResultExport);

        if((dynamic_cast<const CT_PointsAttributesColor*>(pA) != nullptr)
                || (dynamic_cast<const CT_AbstractPointAttributesScalar*>(pA)))
        {
            mPointsAttributeOfTypeColorOrScalar = pA;
        }

        if(mIteratorItemBegin == mIteratorItemEnd)
        {
            auto iterator = m_hInItem.iterateInputs(m_handleResultExport);
            mIteratorItemBegin = iterator.begin();
            mIteratorItemEnd = iterator.end();
        }

        int nExported = 0;
        const int totalToExport = maximumItemToExportInFile(std::numeric_limits<int>::max());

        // write data
        while((mIteratorItemBegin != mIteratorItemEnd)
              && (nExported < totalToExport))
        {
            const CT_AbstractItemDrawableWithPointCloud *item = *mIteratorItemBegin;

            exportPoints(pieceByPieceExporters,
                         item->pointCloudIndex());

            ++nExported;
            ++mIteratorItemBegin;
        }

        return (mIteratorItemBegin == mIteratorItemEnd) ? NoMoreItemToExport : ExportCanContinue;
    }
    else
    {
        for(const CT_AbstractItemDrawableWithPointCloud* item : m_items)
        {
            exportPoints(pieceByPieceExporters,
                         item->pointCloudIndex());
        }

        const QList<CT_AbstractCloudIndex*> &pointsSelected = pointsToExport();
        QListIterator<CT_AbstractCloudIndex*> itCI(pointsSelected);

        while(itCI.hasNext())
        {
            exportPoints(pieceByPieceExporters,
                         dynamic_cast<CT_AbstractPointCloudIndex*>(itCI.next()));
        }
    }

    return NoMoreItemToExport;
}

void PB_XYBExporter::exportPoints(const QList<CT_AbstractPieceByPieceExporter*>& pieceByPieceExporters,
                                  const CT_AbstractPointCloudIndex* constPCIndex)
{
    CT_PointIterator it(constPCIndex);
    size_t totalSize = it.size();

    // function that export the point
    auto funcExportPoint = [&pieceByPieceExporters](const size_t& globalIndex, const CT_Point& point, quint16 intensity)
    {
        for(CT_AbstractPieceByPieceExporter* ex : pieceByPieceExporters)
        {
            static_cast<PB_XYBPieceByPiecePrivateExporter*>(ex)->exportPoint(globalIndex, point, intensity);
        }
    };

    const CT_PointsAttributesColor* cc = dynamic_cast<const CT_PointsAttributesColor*>(mPointsAttributeOfTypeColorOrScalar);
    const CT_AbstractPointAttributesScalar* scalar = dynamic_cast<const CT_AbstractPointAttributesScalar*>(mPointsAttributeOfTypeColorOrScalar);

    const double min = scalar != nullptr ? scalar->minScalarAsDouble() : 0;
    const double max = scalar != nullptr ? scalar->maxScalarAsDouble() : 0;
    const double range = qMax(max-min, 1.0);


    // function that return the intensity if variable "cc" and "scalar" == nullptr
    std::function<quint16 ()> vCCAndScalarNull = []()
    {
        return static_cast<quint16>(0);
    };

    // function that return the intensity if variable "scalar" != nullptr
    std::function<quint16 ()> vScalarNotNull = [&scalar, &min, &range, &it]()
    {
        return ((scalar->scalarAsDoubleAt(it.currentGlobalIndex())-min)*255)/range;
    };

    // function that return the intensity if variable "cc" != nullptr
    std::function<quint16 ()> vCCNotNull = [&cc, &it]()
    {
        const CT_Color &col = cc->constColorAt(it.currentGlobalIndex());
        return static_cast<quint16>((col.r() + col.g() + col.b()) / 3.0);
    };

    // final function to use to return the intensity
    std::function<quint16 ()> funcGetIntensity = (cc == nullptr) ? ((scalar == nullptr) ? vCCAndScalarNull : vScalarNotNull) : vCCNotNull;

    // final function that export the point and his intensity
    std::function<void (const size_t&, const CT_Point&)> funcExportAll = [&funcExportPoint, &funcGetIntensity](const size_t& globalIndex, const CT_Point& point)
    {
        funcExportPoint(globalIndex, point, funcGetIntensity());
    };

    // now we don't have any if statement in the loop !
    size_t i = 0;

    while(it.hasNext() && !isStopped())
    {
        const CT_Point &point = it.next().currentPoint();

        funcExportAll(it.currentGlobalIndex(), point);

        i += 100;

        setExportProgress(static_cast<int>(i/totalSize));
    }
}

void PB_XYBExporter::clearIterators()
{
    mIteratorItemBegin = HandleItemType::const_iterator();
    mIteratorItemEnd = mIteratorItemBegin;
}

void PB_XYBExporter::clearAttributesClouds()
{
}

