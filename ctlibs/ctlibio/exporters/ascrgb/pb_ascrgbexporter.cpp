#include "pb_ascrgbexporter.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QEventLoop>
#include <QApplication>
#include <QProgressDialog>
#include <QFileInfo>

#include "ct_tools/ct_numerictostringconversiont.h"

#include "ct_itemdrawable/abstract/ct_abstractpointattributesscalar.h"
#include "ct_itemdrawable/ct_pointsattributescolor.h"

PB_ASCRGBExporter::PB_ASCRGBExporter(int subMenuLevel) : SuperClass(subMenuLevel),
    mPointsAttributeOfTypeColorOrScalar(nullptr),
    mPointsNormal(nullptr)
{
    addNewExportFormat(FileFormat("asc", tr("Fichier asc")));

    setToolTip(tr("Exporte les points au format ASCII, avec les champs suivants :<br>"
                  "- X  : Coordonnée X<br>"
                  "- Y  : Coordonnée Y<br>"
                  "- Z  : Coordonnée Z<br>"
                  "- R  : Composante rouge<br>"
                  "- V  : Composante verte<br>"
                  "- B  : Composante Bleue<br>"
                  "- NX : Coordonnée X de la normale au point<br>"
                  "- NY : Coordonnée Y de la normale au point<br>"
                  "- NZ : Coordonnée Z de la normale au point<br><br>"));
}

PB_ASCRGBExporter::PB_ASCRGBExporter(const PB_ASCRGBExporter& other) : SuperClass(other),
    mItems(other.mItems)
{
}

QString PB_ASCRGBExporter::displayableName() const
{
    return tr("Points, ASCII(X,Y,Z,R,G,B,NX,NY,NZ)");
}

void PB_ASCRGBExporter::setItemsToExport(const QList<const CT_AbstractItemDrawableWithPointCloud*>& items,
                                         const CT_AbstractPointsAttributes* pointsAttributeOfTypeColorOrScalar,
                                         const CT_PointsAttributesNormal* normalsAttributes)
{
    setMustUseModels(false);
    mItems = items;

    mPointsAttributeOfTypeColorOrScalar = pointsAttributeOfTypeColorOrScalar;
    mPointsNormal = normalsAttributes;
}

void PB_ASCRGBExporter::internalDeclareInputModels(CT_ExporterInModelStructureManager& manager)
{
    manager.addGroupToRootGroup(m_hInGroup);
    manager.addItemToGroup(m_hInGroup, m_hInItem, tr("Item à exporter"));
    manager.addPointAttributeToGroup(m_hInGroup, m_hInColorsAttribute, tr("Couleurs"));
    manager.addPointAttributeToGroup(m_hInGroup, m_hInNormalsAttribute, tr("Normales"));
}

CT_AbstractExporter::ExportReturn PB_ASCRGBExporter::internalExportToFile()
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

    if(mustUseModels())
    {
        mPointsAttributeOfTypeColorOrScalar = nullptr;
        mPointsNormal = nullptr;

        const CT_AbstractPointsAttributes* pA = m_hInColorsAttribute.firstInput(m_handleResultExport);

        if((dynamic_cast<const CT_PointsAttributesColor*>(pA) != nullptr)
                || (dynamic_cast<const CT_AbstractPointAttributesScalar*>(pA)))
        {
            mPointsAttributeOfTypeColorOrScalar = pA;
        }

        mPointsNormal = m_hInNormalsAttribute.firstInput(m_handleResultExport);
    }

    txtStream << "X Y Z";

    if(mPointsAttributeOfTypeColorOrScalar != nullptr)
        txtStream << " R G B";

    if(mPointsNormal != nullptr)
        txtStream << " NX NY NZ";

    txtStream << "\n";

    const CT_PointsAttributesColor* cc = dynamic_cast<const CT_PointsAttributesColor*>(mPointsAttributeOfTypeColorOrScalar);
    const CT_AbstractPointAttributesScalar* scalar = dynamic_cast<const CT_AbstractPointAttributesScalar*>(mPointsAttributeOfTypeColorOrScalar);

    const double min = scalar != nullptr ? scalar->minScalarAsDouble() : 0;
    const double max = scalar != nullptr ? scalar->maxScalarAsDouble() : 0;
    const double range = qMax(max-min, 1.0);

    // function that assign the color values if variable "scalar" != nullptr
    std::function<void (const size_t&, quint16&, quint16&, quint16&)> vScalarNotNull = [&scalar, &min, &range](const size_t& globalIndex, quint16& r, quint16& g, quint16& b)
    {
        r = g = b = quint16(((scalar->scalarAsDoubleAt(globalIndex)-min)*255)/range);
    };

    // function that assign the color values if variable "cc" != nullptr
    std::function<void (const size_t&, quint16&, quint16&, quint16&)> vCCNotNull = [&cc](const size_t& globalIndex, quint16& r, quint16& g, quint16& b)
    {
        const CT_Color &col = cc->constColorAt(globalIndex);
        r = quint16(col.r() / 255.0);
        g = quint16(col.g() / 255.0);
        b = quint16(col.b() / 255.0);
    };

    std::function<void (const size_t&, quint16&, quint16&, quint16&)> finalColorFunc = (cc == nullptr) ? ((scalar == nullptr) ? nullptr : vScalarNotNull) : vCCNotNull;

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

            exportItem(item, txtStream, finalColorFunc, nExported, totalToExport);

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
            exportItem(item, txtStream, finalColorFunc, nExported, totalToExport);
            nExported += 100;
        }
    }

    file.close();

    return NoMoreItemToExport;
}

void PB_ASCRGBExporter::clearIterators()
{
    mIteratorItemBegin = HandleItemType::const_iterator();
    mIteratorItemEnd = mIteratorItemBegin;
}

void PB_ASCRGBExporter::clearAttributesClouds()
{
}

void PB_ASCRGBExporter::exportItem(const CT_AbstractItemDrawableWithPointCloud* item,
                                   QTextStream& txtStream,
                                   std::function<void (const size_t&, quint16&, quint16&, quint16&)> colorFunc,
                                   const int& nExported,
                                   const int& totalToExport)
{
    CT_PointIterator itP(item->pointCloudIndex());

    size_t totalSize = itP.size();
    size_t i = 0;

    quint16 r, g, b;

    while(itP.hasNext())
    {
        const CT_Point &point = itP.next().currentPoint();

        txtStream << CT_NumericToStringConversionT<double>::toString(point(0)) << " ";
        txtStream << CT_NumericToStringConversionT<double>::toString(point(1)) << " ";
        txtStream << CT_NumericToStringConversionT<double>::toString(point(2));

        if(colorFunc != nullptr)
        {
            colorFunc(itP.currentGlobalIndex(), r, g, b);

            txtStream << " " << r << " ";
            txtStream << g << " ";
            txtStream << b;
        }

        if(mPointsNormal != nullptr)
        {
            const CT_Normal &no = mPointsNormal->constNormalAt(itP.currentGlobalIndex());

            txtStream << " " << no[0] << " ";
            txtStream << no[1] << " ";
            txtStream << no[2];
        }

        txtStream << "\n";

        ++i;

        setExportProgress(int((((int(i)*100)/int(totalSize))+nExported)/totalToExport));
    }
}
