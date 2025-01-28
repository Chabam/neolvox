#include "pb_gdalexporter.h"

#ifdef USE_GDAL
#include "gdal.h"
#include "ogrsf_frmts.h"
#endif

#include "readers/tools/gdal/ct_gdaltools.h"

#include <QFileInfo>
#include <QDebug>

#define VECTOR_DRIVER_TYPE "Vector"
#define RASTER_DRIVER_TYPE "Raster"

// TODO : choose raster type (what is R, G or B, etc...) and export it in multiple bands
// TODO : Group raster by dimension and resolution

PB_GDALExporter::PB_GDALExporter(int subMenuLevelRaster, int subMenuLevelVector, int subMenuLevelDefault) : SuperClass(subMenuLevelDefault),
    mSubMenuLevelRaster(subMenuLevelRaster),
    mSubMenuLevelVector(subMenuLevelVector)
{
}

PB_GDALExporter::PB_GDALExporter(const PB_GDALExporter& other) : SuperClass(other),
    #ifdef USE_GDAL
    m_driver(other.m_driver),
    m_nameFromDriver(other.m_nameFromDriver),
    #endif
    mSubMenuLevelRaster(other.mSubMenuLevelRaster),
    mSubMenuLevelVector(other.mSubMenuLevelVector),
    mVectors(other.mVectors),
    mRasters(other.mRasters)
{
}

#ifdef USE_GDAL
PB_GDALExporter::PB_GDALExporter(const GDALDriver *driver, int subMenuLevelRaster, int subMenuLevelVector, int subMenuLevelDefault) : SuperClass(subMenuLevelDefault),
    m_driver(const_cast<GDALDriver*>(driver)),
    mSubMenuLevelRaster(subMenuLevelRaster),
    mSubMenuLevelVector(subMenuLevelVector)
{
    updateNameFromDriver();

    QStringList ext = CT_GdalTools::staticGdalDriverExtension(m_driver);

    if(ext.isEmpty())
        ext.append("gdal");

    addNewExportFormat(FileFormat(ext, CT_GdalTools::staticGdalDriverName((m_driver))));

    const QString driverType = typeOfDriver();

    QString toolTip = tr("Exporter GDAL de type : %1").arg(driverType.isEmpty()?tr("Inconnu"):driverType);

    if (driverType == RASTER_DRIVER_TYPE)
    {
        toolTip.append("<br>");
        toolTip.append("<br>");
        toolTip.append(tr("Pour plus de détails voir : http://www.gdal.org/formats_list.html"));
    }
    else if (driverType == VECTOR_DRIVER_TYPE)
    {
        toolTip.append("<br>");
        toolTip.append("<br>");
        toolTip.append(tr("Pour plus de détails voir : http://www.gdal.org/ogr_formats.html"));
    }

    toolTip.append("<br>");
    toolTip.append("<br>");
    toolTip.append(tr("Extension : "));

    for (int i = 0 ; i < ext.size() ; i++)
    {
        toolTip.append("*.");
        toolTip.append(ext.at(i));
        if ((i + 1) < ext.size()) {toolTip.append(" ");}
    }

    setToolTip(toolTip);
}
#endif

QString PB_GDALExporter::displayableName() const
{
#ifdef USE_GDAL
    if(!m_nameFromDriver.isEmpty())
        return m_nameFromDriver;
#endif

    return metaObject()->className();
}

bool PB_GDALExporter::isExportEachItemInSeparateFileOptionnal() const
{
#ifdef USE_GDAL
    const QString driverType = typeOfDriver();

    if (driverType == RASTER_DRIVER_TYPE) // raster can only be exported one per file
        return false;

    // multiple vectors or one vector per file is allowed
    return true;
#else
    return SuperClass::isExportEachItemInSeparateFileOptionnal();
#endif
}

QString PB_GDALExporter::uniqueName() const
{
    return displayableName();
}

int PB_GDALExporter::subMenuLevel() const
{
    const QString driverType = typeOfDriver();

    if (driverType == RASTER_DRIVER_TYPE)
        return mSubMenuLevelRaster;

    if (driverType == VECTOR_DRIVER_TYPE)
        return mSubMenuLevelVector;

    return SuperClass::subMenuLevel();
}

QString PB_GDALExporter::typeOfDriver() const
{
#ifdef USE_GDAL
    if (m_driver->GetMetadataItem(GDAL_DCAP_RASTER) != nullptr && m_driver->GetMetadataItem(GDAL_DCAP_VECTOR) == nullptr)
        return RASTER_DRIVER_TYPE;

    if (m_driver->GetMetadataItem(GDAL_DCAP_VECTOR) != nullptr && m_driver->GetMetadataItem(GDAL_DCAP_RASTER) == nullptr)
        return VECTOR_DRIVER_TYPE;
#endif

    return QString();
}

void PB_GDALExporter::setVectorsToExport(const QList<const CT_AbstractShape2D*>& vectorList)
{
    setMustUseModels(false);
    mRasters.clear();
    mVectors = vectorList;
}

void PB_GDALExporter::setRastersToExport(const QList<const CT_AbstractImage2D*>& rasterList)
{
    setMustUseModels(false);
    mVectors.clear();
    mRasters = rasterList;
}

#ifdef USE_GDAL2

    QList<CT_AbstractItemAttribute *> attributes = shape2d->itemAttributes();
    for (int i = 0 ; i < attributes.size() ; i++)
    {
        CT_AbstractItemAttribute* att = attributes.at(i);

        CT_OutAbstractSingularItemModel  *itemModel = (CT_OutAbstractSingularItemModel*) shape2d->model();
        CT_OutAbstractItemAttributeModel *attrModel = att->model();

        //QString itemDN = itemModel->displayableName();
        QString itemUN = itemModel->uniqueName();

        QString attrDN = attrModel->displayableName();
        QString attrUN = attrModel->uniqueName();

        if (attrModel->isADefaultItemAttributeModel() && attrModel->rootOriginalModel() != nullptr) {attrUN = attrModel->rootOriginalModel()->uniqueName();}

        QString key = QString("ITEM_%1_ATTR_%2").arg(itemUN).arg(attrUN);

        if (!_modelsKeys.contains(key))
        {
            _modelsKeys.append(key);
            _names.insert(key, attrDN);

            CT_AbstractCategory::ValueType type = attrModel->itemAttribute()->type();

            if      (type == CT_AbstractCategory::BOOLEAN) {_ogrTypes.insert(key, OFTInteger);}
            else if (type == CT_AbstractCategory::STRING)  {_ogrTypes.insert(key, OFTString);}
            else if (type == CT_AbstractCategory::STRING)  {_ogrTypes.insert(key, OFTString);}
            else if (type == CT_AbstractCategory::INT8)    {_ogrTypes.insert(key, OFTInteger);}
            else if (type == CT_AbstractCategory::UINT8)   {_ogrTypes.insert(key, OFTInteger);}
            else if (type == CT_AbstractCategory::INT16)   {_ogrTypes.insert(key, OFTInteger);}
            else if (type == CT_AbstractCategory::UINT16)  {_ogrTypes.insert(key, OFTInteger);}
            else if (type == CT_AbstractCategory::INT32)   {_ogrTypes.insert(key, OFTInteger);}
            //                else if (type == CT_AbstractCategory::UINT32)  {_ogrTypes.insert(key, OFTInteger64);}
            //                else if (type == CT_AbstractCategory::INT64)   {_ogrTypes.insert(key, OFTInteger64);}
            //                else if (type == CT_AbstractCategory::INT32)   {_ogrTypes.insert(key, OFTInteger64);}
            else                                           {_ogrTypes.insert(key, OFTReal);}
        }
    }

    if (!_names.isEmpty())
    {
        replaceBadCharacters(_names);
        std::sort(_modelsKeys.begin(), _modelsKeys.end());
        _shortNames = computeShortNames(_names);


        QFileInfo exportPathInfo = QFileInfo(exportFilePath());

        QFile ffields(QString("%1/fields_names.txt").arg(exportPathInfo.absolutePath()));
        QTextStream fstream(&ffields);
        if (ffields.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QMapIterator<QString, QString> itF(_shortNames);
            while (itF.hasNext())
            {
                itF.next();
                QString key = itF.key();
                QString shortName = itF.value();
                QString longName = _names.value(key);
                fstream << shortName << "\t";
                fstream << longName << "\n";
            }
            ffields.close();
        }

    }
#endif


void PB_GDALExporter::internalDeclareInputModels(CT_ExporterInModelStructureManager& manager)
{
    manager.addGroupToRootGroup(m_hInGroup);

    const QString driverType = typeOfDriver();

    if(driverType == RASTER_DRIVER_TYPE)
        manager.addItemToGroup(m_hInGroup, m_hInRaster, tr("Rasters"));
    else if(driverType == VECTOR_DRIVER_TYPE)
        manager.addItemToGroup(m_hInGroup, m_hInVector, tr("Données vectorielles"));
}

CT_AbstractExporter::ExportReturn PB_GDALExporter::internalExportToFile()
{
    const QFileInfo exportPathInfo = QFileInfo(filePath());
    const QString prePath = exportPathInfo.path() + "/" + exportPathInfo.baseName();
    const QString suffix = exportFormats().first().suffixes().first();

    if(mustUseModels())
    {
        if(m_hInVector.isValid())
        {
            if(mIteratorVectorBegin == mIteratorVectorEnd)
            {
                auto iterator = m_hInVector.iterateInputs(m_handleResultExport);
                mIteratorVectorBegin = iterator.begin();
                mIteratorVectorEnd = iterator.end();
            }

            int nExported = 0;
            const int totalToExport = maximumItemToExportInFile(std::numeric_limits<int>::max());
            const QString noIndice;

            QList<const CT_AbstractShape2D*> vectors;

            while((mIteratorVectorBegin != mIteratorVectorEnd)
                  && (nExported < totalToExport))
            {
                if(isStopped())
                    break;

                vectors.append(*mIteratorVectorBegin);

                ++nExported;
                ++mIteratorVectorBegin;
            }

            // write data
            if(!isStopped() && !exportVectors(vectors, prePath, suffix))
                return ErrorWhenExport;

            return (mIteratorVectorBegin == mIteratorVectorEnd) ? NoMoreItemToExport : ExportCanContinue;
        }
        else if(m_hInRaster.isValid())
        {
            if(mIteratorRasterBegin == mIteratorRasterEnd)
            {
                auto iterator = m_hInRaster.iterateInputs(m_handleResultExport);
                mIteratorRasterBegin = iterator.begin();
                mIteratorRasterEnd = iterator.end();
            }

            if(mIteratorRasterBegin != mIteratorRasterEnd)
            {
                if(!exportRaster(*mIteratorRasterBegin, prePath + "." + suffix))
                    return ErrorWhenExport;

                ++mIteratorRasterBegin;
            }

            return (mIteratorRasterBegin == mIteratorRasterEnd) ? NoMoreItemToExport : ExportCanContinue;
        }
    }
    else if(!mVectors.isEmpty())
    {
        if(!exportVectors(mVectors, prePath, suffix))
            return ErrorWhenExport;
    }
    else if(!mRasters.isEmpty())
    {
        if (mRasters.size() > 1)
        {
            int cpt = 0;
            for(auto raster : mRasters)
            {
                if(isStopped())
                    break;

                if(!exportRaster(raster, prePath + QString("_%1").arg(cpt) + "." + suffix))
                    return ErrorWhenExport;

                ++cpt;
            }
        } else {
            if(!exportRaster(mRasters.first(), prePath + "." + suffix))
                return ErrorWhenExport;
        }
    }

    return NoMoreItemToExport;
}

void PB_GDALExporter::clearIterators()
{
    mIteratorVectorBegin = HandleVectorType::const_iterator();
    mIteratorVectorEnd = mIteratorVectorBegin;

    mIteratorRasterBegin = HandleRasterType::const_iterator();
    mIteratorRasterEnd = mIteratorRasterBegin;
}

void PB_GDALExporter::clearAttributesClouds()
{
}

// (multiple vector per file)
bool PB_GDALExporter::exportVectors(const QList<const CT_AbstractShape2D*> vectors, const QString& filePathWithoutSuffix, const QString& suffix)
{
#ifdef USE_GDAL
    const std::string fp = (filePathWithoutSuffix + "." + suffix).toStdString();
    GDALDataset* dataset = m_driver->Create(fp.data(), 0, 0, 0, GDT_Unknown, nullptr);

    if( dataset == nullptr )
    {
        setErrorMessage(tr("Creation of output file failed."));
        return false;
    }

    OGRLayer* layer = dataset->CreateLayer("layer", nullptr, wkbUnknown, nullptr);

    if( layer == nullptr )
    {
        GDALClose(dataset);
        setErrorMessage(tr("Layer creation failed."));
        return false;
    }

    QList<QString> modelKeys;
    QMap<QString, QString>  names;
    QMap<QString, OGRFieldType> ogrTypes;

    for(const CT_AbstractShape2D* vector : vectors)
    {
        const CT_OutAbstractModel* itemModel = vector->model();

        vector->visitItemAttributes([&modelKeys, &names, &ogrTypes, &itemModel](const CT_AbstractItemAttribute* att) -> bool
        {
            const CT_OutAbstractItemAttributeModel* attModel = static_cast<CT_OutAbstractItemAttributeModel*>(att->model());

            const QString attrDN = attModel->displayableName();

            const QString key = QString("ITEM_%1_ATTR_%2").arg(size_t(itemModel)).arg(size_t(attModel));

            if(!modelKeys.contains(key))
            {
                modelKeys.append(key);
                names.insert(key, attrDN);

                const CT_AbstractCategory::ValueType type = CT_AbstractCategory::ValueType(attModel->itemAttribute()->itemAttributeToolForModel()->valueType());

                if      (type == CT_AbstractCategory::BOOLEAN) {ogrTypes.insert(key, OFTInteger);}
                else if (type == CT_AbstractCategory::STRING)  {ogrTypes.insert(key, OFTString);}
                else if (type == CT_AbstractCategory::STRING)  {ogrTypes.insert(key, OFTString);}
                else if (type == CT_AbstractCategory::INT8)    {ogrTypes.insert(key, OFTInteger);}
                else if (type == CT_AbstractCategory::UINT8)   {ogrTypes.insert(key, OFTInteger);}
                else if (type == CT_AbstractCategory::INT16)   {ogrTypes.insert(key, OFTInteger);}
                else if (type == CT_AbstractCategory::UINT16)  {ogrTypes.insert(key, OFTInteger);}
                else if (type == CT_AbstractCategory::INT32)   {ogrTypes.insert(key, OFTInteger);}
                //                else if (type == CT_AbstractCategory::UINT32)  {ogrTypes.insert(key, OFTInteger64);}
                //                else if (type == CT_AbstractCategory::INT64)   {ogrTypes.insert(key, OFTInteger64);}
                //                else if (type == CT_AbstractCategory::INT32)   {ogrTypes.insert(key, OFTInteger64);}
                else                                           {ogrTypes.insert(key, OFTReal);}
            }

            return true;
        });
    }

    replaceBadCharacters(names);
    std::sort(modelKeys.begin(), modelKeys.end());
    const QMap<QString, QString> shortNames = computeShortNames(names);

    for(const QString& key : modelKeys)
    {
        if (ogrTypes.contains(key))
        {
            const OGRFieldType ogrType = ogrTypes.value(key);

            const std::string fieldName = shortNames.value(key).toStdString();
            OGRFieldDefn oField(fieldName.data(), ogrType );

            if(layer->CreateField( &oField ) != OGRERR_NONE)
            {
                setErrorMessage(tr("Creation of a field failed."));
                GDALClose(dataset);
                return false;
            }
        }
    }

    QFile fieldNamesFile(filePathWithoutSuffix + "_fields_names.txt");

    if(!fieldNamesFile.open(QFile::WriteOnly))
    {
        setErrorMessage(tr("Creation of output file (fields names) failed."));
        GDALClose(dataset);
        return false;
    }

    QTextStream fieldsNamesStream(&fieldNamesFile);

    QMapIterator<QString, QString> itF(shortNames);
    while (itF.hasNext())
    {
        itF.next();
        QString shortName = itF.value();
        QString longName = names.value(itF.key());
        fieldsNamesStream << shortName << "\t";
        fieldsNamesStream << longName << "\n";
    }
    fieldNamesFile.close();

    for(const CT_AbstractShape2D* vector : vectors)
        exportVector(vector, dataset, layer, shortNames, ogrTypes);

    GDALClose(dataset);

    return true;
#else
    Q_UNUSED(suffix);
    Q_UNUSED(filePathWithoutSuffix);
    return false;
#endif
}

// RASTER (one raster per file)
bool PB_GDALExporter::exportRaster(const CT_AbstractImage2D* grid, const QString& filePath)
{
#ifdef USE_GDAL
    const int nXSize = grid->xdim();
    const int nYSize = grid->ydim();

    const std::string fp = filePath.toStdString();
    GDALDataset* dataset = m_driver->Create(fp.data(), nXSize, nYSize, 1, GDT_Float32, nullptr);

    if( dataset == nullptr )
    {
        setErrorMessage(tr("Creation of output file failed."));
        return false;
    }

    if( dataset->GetRasterBand(1) == nullptr )
    {
        GDALClose(dataset);
        setErrorMessage(tr("Creation of output file failed."));
        return false;
    }

    GDALRasterBand *poBand = dataset->GetRasterBand(1);

    double padfTransform[6] = { 0, 0, 0, 0, 0, 0 };

    padfTransform[0] = grid->minX();
    padfTransform[1] = grid->resolution();
    padfTransform[5] = -grid->resolution();
    padfTransform[3] = grid->minY() + nYSize*padfTransform[1];

    dataset->SetGeoTransform( padfTransform );

    poBand->SetNoDataValue(grid->NAAsDouble());

    float* pafScanline = static_cast<float*>(CPLMalloc(sizeof(float)*size_t(nXSize)));

    size_t index = 0;

    for(int y = 0 ;  y < nYSize ; ++y)
    {
        for(int x = 0 ; x < nXSize ; ++x)
        {
            grid->index(x, y, index);
            pafScanline[x] = float(grid->valueAtIndexAsDouble(index));
        }
#if defined(_WIN32) && defined(_MSC_VER) // Microsoft Visual Studio Compiler
#elif (defined(__linux__) || defined(_WIN32)) && defined(__GNUC__) // GNU Compiler (gcc,g++) for Linux, Unix, and MinGW (Windows)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
#elif defined(__APPLE__) // Clang Compiler (Apple)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
#endif
        poBand->RasterIO( GF_Write, 0, y, nXSize, 1, pafScanline, nXSize, 1, GDT_Float32, 0, 0);
#if defined(_WIN32) && defined(_MSC_VER)
#elif (defined(__linux__) || defined(_WIN32)) && defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(__APPLE__)
#pragma GCC diagnostic pop
#endif
    }

    CPLFree(pafScanline);

    double min, max, mean, stdDev;
    poBand->FlushCache();
    poBand->ComputeStatistics(0, &min, &max, &mean, &stdDev, nullptr, nullptr);
    poBand->SetStatistics(min, max, mean, stdDev);
    poBand->FlushCache();

    GDALClose(dataset);

    return true;
#else
    Q_UNUSED(filePath);
    Q_UNUSED(grid);
    return false;
#endif
}

#ifdef USE_GDAL

bool PB_GDALExporter::exportVector(const CT_AbstractShape2D* shape2d, GDALDataset *dataset, OGRLayer *layer, const QMap<QString, QString>& shortNames, const QMap<QString, OGRFieldType>& ogrTypes)
{
    OGRFeature *poFeature;
    poFeature = OGRFeature::CreateFeature( layer->GetLayerDefn() );

    shape2d->visitItemAttributes([shape2d, &poFeature, &shortNames, &ogrTypes](const CT_AbstractItemAttribute* att) -> bool
    {
        const QString key = QString("ITEM_%1_ATTR_%2").arg(size_t(shape2d->model())).arg(size_t(att->model()));

        const OGRFieldType vt = ogrTypes.value(key);
        const std::string fieldName = shortNames.value(key).toStdString();

        if(vt == OFTBinary)
        {
            poFeature->SetField(fieldName.data(), att->toInt(shape2d, nullptr));
        }
        else if (vt == OFTString)
        {
            const std::string text = replaceAccentCharacters(att->toString(shape2d, nullptr)).toStdString();
            poFeature->SetField(fieldName.data(), text.data());
        }
        else if(vt == OFTInteger)
        {
            poFeature->SetField(fieldName.data(), att->toInt(shape2d, nullptr));
        }
        else
        {
            poFeature->SetField(fieldName.data(), att->toDouble(shape2d, nullptr));
        }

        return true;
    });

    const CT_Box2D *box = dynamic_cast<const CT_Box2D*>(shape2d);
    if(box != nullptr)
        return exportBox2D(box, dataset, layer, poFeature);

    const CT_Circle2D *circle = dynamic_cast<const CT_Circle2D*>(shape2d);
    if(circle != nullptr)
        return exportCircle2D(circle, dataset, layer, poFeature);

    const CT_Line2D *line = dynamic_cast<const CT_Line2D*>(shape2d);
    if(line != nullptr)
        return exportLine2D(line, dataset, layer, poFeature);

    const CT_Point2D *point = dynamic_cast<const CT_Point2D*>(shape2d);
    if(point != nullptr)
        return exportPoint2D(point, dataset, layer, poFeature);

    const CT_Polygon2D *polyg = dynamic_cast<const CT_Polygon2D*>(shape2d);
    if(polyg != nullptr)
        return exportPolygon2D(polyg, dataset, layer, poFeature);

    const CT_Polyline2D *polyl = dynamic_cast<const CT_Polyline2D*>(shape2d);
    if(polyl != nullptr)
        return exportPolyline2D(polyl, dataset, layer, poFeature);

    OGRFeature::DestroyFeature( poFeature );

    return false;
}

bool PB_GDALExporter::exportBox2D(const CT_Box2D *box, GDALDataset *dataset, OGRLayer *layer, OGRFeature *poFeature)
{
    OGRPolygon po;

    Eigen::Vector3d min, max;
    box->boundingBox(min, max);

    OGRLinearRing extRing;
    extRing.addPoint(min[0], min[1], 0);
    extRing.addPoint(max[0], min[1], 0);
    extRing.addPoint(max[0], max[1], 0);
    extRing.addPoint(min[0], max[1], 0);
    extRing.addPoint(min[0], min[1], 0);

    po.addRing(&extRing);

    return exportOGRGeometry(&po, dataset, layer, poFeature);
}

bool PB_GDALExporter::exportCircle2D(const CT_Circle2D *circle, GDALDataset *dataset, OGRLayer *layer, OGRFeature *poFeature)
{
    Eigen::Vector2d c = circle->getCenter();
    c[0] -= circle->getRadius();

    Eigen::Vector2d c2 = circle->getCenter();
    c2[0] += circle->getRadius();

    OGRLineString *ogrl = OGRGeometryFactory::curveToLineString(c[0], c[1], 0, c2[0], c2[1], 0, c[0], c[1], 0, false, 0, nullptr);

    bool r = exportOGRGeometry(ogrl, dataset, layer, poFeature);

    OGRGeometryFactory::destroyGeometry(ogrl);

    return r;
}

bool PB_GDALExporter::exportLine2D(const CT_Line2D *line, GDALDataset *dataset, OGRLayer *layer, OGRFeature *poFeature)
{
    OGRLineString ogrl;

    ogrl.addPoint(line->x1(), line->y1(), 0);
    ogrl.addPoint(line->x2(), line->y2(), 0);

    return exportOGRGeometry(&ogrl, dataset, layer, poFeature);
}

bool PB_GDALExporter::exportPoint2D(const CT_Point2D *point, GDALDataset *dataset, OGRLayer *layer, OGRFeature *poFeature)
{
    OGRPoint ogrp;

    ogrp.setX(point->x());
    ogrp.setY(point->y());
    ogrp.setZ(0);

    return exportOGRGeometry(&ogrp, dataset, layer, poFeature);
}

bool PB_GDALExporter::exportPolygon2D(const CT_Polygon2D *polygon, GDALDataset *dataset, OGRLayer *layer, OGRFeature *poFeature)
{
    OGRPolygon po;
    OGRLinearRing extRing;

    QVectorIterator<Eigen::Vector2d> it(polygon->getVertices());

    while(it.hasNext())
    {
        const Eigen::Vector2d& v = it.next();
        extRing.addPoint(v[0], v[1], 0);
    }

    it.toFront();
    if (it.hasNext())
    {
        const Eigen::Vector2d& v = it.next();
        extRing.addPoint(v[0], v[1], 0);
    }

    po.addRing(&extRing);

    return exportOGRGeometry(&po, dataset, layer, poFeature);
}

bool PB_GDALExporter::exportPolyline2D(const CT_Polyline2D *polyline, GDALDataset *dataset, OGRLayer *layer, OGRFeature *poFeature)
{
    OGRLineString ogrl;

    QVectorIterator<Eigen::Vector2d> it(polyline->getVertices());

    while(it.hasNext())
    {
        const Eigen::Vector2d& v = it.next();
        ogrl.addPoint(v[0], v[1], 0);
    }

    return exportOGRGeometry(&ogrl, dataset, layer, poFeature);
}

bool PB_GDALExporter::exportOGRGeometry(OGRGeometry *geo, GDALDataset *dataset, OGRLayer *layer, OGRFeature *poFeature)
{
    Q_UNUSED(dataset)

    poFeature->SetGeometry( geo );

    if( layer->CreateFeature( poFeature ) != OGRERR_NONE )
    {
        OGRFeature::DestroyFeature( poFeature );
        setErrorMessage(tr("Failed to create feature in shapefile."));
        return false;
    }

    OGRFeature::DestroyFeature( poFeature );

    return true;
}

void PB_GDALExporter::updateNameFromDriver()
{
    if(m_driver != nullptr)
        m_nameFromDriver = QString("%1 %2").arg(typeOfDriver()).arg(CT_GdalTools::staticGdalDriverName(m_driver));
    else
        m_nameFromDriver = QString();
}

#endif

void PB_GDALExporter::replaceBadCharacters(QMap<QString, QString> &names)
{
    QMutableMapIterator<QString, QString> it(names);
    while (it.hasNext())
    {
        it.next();
        it.setValue(replaceBadCharacters(it.value()));
    }
}

QString PB_GDALExporter::replaceBadCharacters(const QString &name)
{
    QString value = name;
    value.replace(QRegExp("[àáâãäå]"), "a");
    value.replace(QRegExp("[ÀÁÂÃÄÅ]"), "A");
    value.replace(QRegExp("[éèëê]"), "e");
    value.replace(QRegExp("[ÈÉÊË]"), "E");
    value.replace(QRegExp("[ìíîï]"), "i");
    value.replace(QRegExp("[ÌÍÎÏ]"), "I");
    value.replace(QRegExp("[òóôõöø]"), "o");
    value.replace(QRegExp("[ÒÓÔÕÖØ]"), "O");
    value.replace(QRegExp("[ùúûü]"), "u");
    value.replace(QRegExp("[ÙÚÛÜ]"), "U");
    value.replace(QRegExp("[ñ]"), "n");
    value.replace(QRegExp("[Ñ]"), "N");
    value.replace(QRegExp("[ç]"), "c");
    value.replace(QRegExp("[Ç]"), "C");
    value.replace(QRegExp("[\\W]"), "_");
    return value;
}

QString PB_GDALExporter::replaceAccentCharacters(const QString &name)
{
        QString value = name;
        value.replace(QRegExp("[àáâãäå]"), "a");
        value.replace(QRegExp("[ÀÁÂÃÄÅ]"), "A");
        value.replace(QRegExp("[éèëê]"), "e");
        value.replace(QRegExp("[ÈÉÊË]"), "E");
        value.replace(QRegExp("[ìíîï]"), "i");
        value.replace(QRegExp("[ÌÍÎÏ]"), "I");
        value.replace(QRegExp("[òóôõöø]"), "o");
        value.replace(QRegExp("[ÒÓÔÕÖØ]"), "O");
        value.replace(QRegExp("[ùúûü]"), "u");
        value.replace(QRegExp("[ÙÚÛÜ]"), "U");
        value.replace(QRegExp("[ñ]"), "n");
        value.replace(QRegExp("[Ñ]"), "N");
        value.replace(QRegExp("[ç]"), "c");
        value.replace(QRegExp("[Ç]"), "C");
        return value;
}


QMap<QString, QString> PB_GDALExporter::computeShortNames(const QMap<QString, QString> &names)
{
    QMap<QString, QString> shortNames;
    QList<QString> existing;

    QMapIterator<QString, QString> it(names);
    while (it.hasNext())
    {
        it.next();
        QString key = it.key();
        QString value = it.value();

        if (value.size() <= 10)
        {
            shortNames.insert(key, value);
            existing.append(value.toLower());
        } else {
            QString newValue = value.left(10);
            int cpt = 2;
            while (existing.contains(newValue.toLower()))
            {
                QString number = QVariant(cpt++).toString();
                newValue = QString("%1%2").arg(value.left(10 - number.length())).arg(number);
            }
            shortNames.insert(key, newValue);
            existing.append(newValue.toLower());
        }
    }

    return shortNames;
}
