#include "ct_reader_xyb.h"

#include "ct_itemdrawable/ct_pointsattributesscalartemplated.h"
#include "ct_coordinates/tools/ct_coordinatesystemmanager.h"
#include "ct_iterator/ct_mutablepointiterator.h"
#include "ct_cloudindex/registered/ct_standardnotmodifiablecloudindexregisteredt.h"

#include <QFile>
#include <QTextStream>

#include <limits>

CT_Reader_XYB::CT_Reader_XYB(int subMenuLevel) : SuperClass(subMenuLevel), CT_ReaderPointsFilteringExtension()
{
    m_filterRadius = 0;
    m_zMinFilter = -std::numeric_limits<double>::max();
    m_zMaxFilter = std::numeric_limits<double>::max();

    addNewReadableFormat(FileFormat("xyb", tr("Fichiers binaire de points .xyb")));
    setToolTip(tr("Chargement de points depuis un fichier format binaire XYB (FARO)"));
}

CT_Reader_XYB::CT_Reader_XYB(const CT_Reader_XYB& other) : SuperClass(other), CT_ReaderPointsFilteringExtension()
{
    m_current = other.m_current;
    m_filterRadius = other.m_filterRadius;
    m_zMinFilter = other.m_zMinFilter;
    m_zMaxFilter = other.m_zMaxFilter;
}

QString CT_Reader_XYB::displayableName() const
{
    return tr("Points, Fichier XYB");
}

bool CT_Reader_XYB::setFilePath(const QString& filepath)
{
    QString error;
    CT_XYBHeader* header = static_cast<CT_XYBHeader*>(internalReadHeader(filepath, error));

    if(!error.isEmpty()) {
        delete header;
        return false;
    }

    if(CT_AbstractReader::setFilePath(filepath)) {
        m_current.m_center[0] = header->scanCenterX();
        m_current.m_center[1] = header->scanCenterY();
        m_current.m_center[2] = header->scanCenterZ();
        m_current._rows = header->nRows();
        m_current._cols = header->nCols();
        m_current._offset = header->offset();

        delete header;
        return true;
    }

    delete header;
    return false;
}

CT_FileHeader* CT_Reader_XYB::createHeaderPrototype() const
{
    return new CT_XYBHeader();
}

void CT_Reader_XYB::setFilterRadius(const double &radius)
{
    m_filterRadius = radius;
}

void CT_Reader_XYB::setFilterRadius(const double &radius, const double &zmin, const double &zmax)
{
    m_filterRadius = radius;
    m_zMinFilter = zmin;
    m_zMaxFilter = zmax;
}

double CT_Reader_XYB::filterRadius() const
{
    return m_filterRadius;
}

double CT_Reader_XYB::filterZMin() const
{
    return m_zMinFilter;
}

double CT_Reader_XYB::filterZMax() const
{
    return m_zMaxFilter;
}

void CT_Reader_XYB::saveSettings(SettingsWriterInterface &writer) const
{
    SuperClass::saveSettings(writer);

    writer.addParameter(this, "FilterRadius", m_filterRadius);
    writer.addParameter(this, "FilterZMin", m_zMinFilter);
    writer.addParameter(this, "FilterZMax", m_zMaxFilter);
}

bool CT_Reader_XYB::restoreSettings(SettingsReaderInterface &reader)
{
    if(!SuperClass::restoreSettings(reader))
        return false;

    QVariant value;

    if(reader.parameter(this, "FilterRadius", value))
        m_filterRadius = value.toDouble();

    if(reader.parameter(this, "FilterZMin", value))
        m_zMinFilter = value.toDouble();

    if(reader.parameter(this, "FilterZMax", value))
        m_zMaxFilter = value.toDouble();

    return true;
}

bool CT_Reader_XYB::isPointFiltered(const CT_Point& point) const
{
    if((m_filterRadius <= 0) || (m_zMinFilter > m_zMaxFilter))
        return CT_ReaderPointsFilteringExtension::isPointFiltered(point);

    const double dx = point.x() - m_current.m_center[0];
    const double dy = point.y() - m_current.m_center[1];
    const double distance2D = sqrt(dx*dx + dy*dy);

    return (distance2D > m_filterRadius ||
            point.z() < m_zMinFilter ||
            point.z() > m_zMaxFilter ||
            CT_ReaderPointsFilteringExtension::isPointFiltered(point));
}

CT_FileHeader* CT_Reader_XYB::internalReadHeader(const QString& filepath, QString& error) const
{
    error = tr("En-tête non valide");

    CT_XYBHeader* header = static_cast<CT_XYBHeader*>(createHeaderPrototype());
    header->setFilePath(filepath);

    if(QFile::exists(filepath))
    {
        QFile f(filepath);

        if(f.open(QFile::ReadOnly))
        {
            int      offset = 0;
            double   xc = 0;
            double   yc = 0;
            double   zc = 0;
            int      rows = 0;
            int      cols = 0;

            char d_data[8];
            int nb_0 = 0;

            while((offset<150) && (nb_0 != 4))
            {
                if (!f.atEnd())
                {
                    f.read(d_data, 1);

                    if(d_data[0] == 0)
                        ++nb_0;
                    else
                        nb_0 = 0;

                    ++offset;
                }
            }

            if(nb_0 != 4)
                offset = -1;

            f.close();

            // Read scan position
            bool okx = true;
            bool oky = true;
            bool okz = true;
            bool okr = true;
            bool okc = true;

            if (f.open(QFile::ReadOnly | QFile::Text))
            {
                QTextStream stream(&f);

                stream.readLine();

                QString line = stream.readLine();

                if (!line.isNull())
                {
                    const QStringList values = line.split(" ");

                    if ((values.size() >= 4) && (values.at(0) == "ScanPosition"))
                    {
                        xc = values.at(1).toDouble(&okx);
                        yc = values.at(2).toDouble(&oky);
                        zc = values.at(3).toDouble(&okz);
                    }
                }

                line = stream.readLine();

                if (!line.isNull())
                {
                    const QStringList values = line.split(" ");

                    if ((values.size() >= 2) && (values.at(0) == "Rows"))
                        rows = values.at(1).toInt(&okr);
                }

                line = stream.readLine();

                if (!line.isNull())
                {
                    const QStringList values = line.split(" ");

                    if ((values.size() >= 2) && (values.at(0) == "Cols"))
                        cols = values.at(1).toInt(&okc);
                }

                f.close();
            }

            if (okx && oky && okz && okr && okc && offset > 0) {
                header->setScanCenter(xc, yc, zc);
                header->setNRows(rows);
                header->setNCols(cols);
                header->setOffset(offset);
                error.clear();
            }
        }
    }

    return header;
}

void CT_Reader_XYB::internalDeclareOutputModels(CT_ReaderOutModelStructureManager& manager)
{
    manager.addItem(m_hOutScene, tr("Scène"));
    manager.addPointAttribute(m_hOutIntensity, tr("Intensité"));
    manager.addItem(m_hOutScanner, tr("Scanner"));
}

bool CT_Reader_XYB::internalReadFile(CT_StandardItemGroup* group)
{
    if(QFile::exists(filepath()))
    {
        QFile f(filepath());

        if(f.open(QFile::ReadOnly))
        {
            QDataStream stream(&f);
            stream.setByteOrder(QDataStream::LittleEndian);
            stream.skipRawData(m_current._offset);

            const qint64 filesize = f.size();

            // un tableau de n_points
            const qint64 nPoints = (filesize - m_current._offset) / 26;

            CT_NMPCIR pcir;

            double xmin = std::numeric_limits<double>::max();
            double ymin = std::numeric_limits<double>::max();
            double zmin = std::numeric_limits<double>::max();
            quint16 imin = std::numeric_limits<quint16>::max();

            double xmax = -std::numeric_limits<double>::max();
            double ymax = -std::numeric_limits<double>::max();
            double zmax = -std::numeric_limits<double>::max();
            quint16 imax = 0;

            const bool filter = (m_filterRadius > 0) || (pointsFilter() != nullptr);

            if(filter) {
                pcir = internalReadFileWithFilter(stream,
                                                  xmin,
                                                  ymin,
                                                  zmin,
                                                  imin,
                                                  xmax,
                                                  ymax,
                                                  zmax,
                                                  imax,
                                                  nPoints);
            } else {
                pcir = internalReadFileWithoutFilter(stream,
                                                     xmin,
                                                     ymin,
                                                     zmin,
                                                     imin,
                                                     xmax,
                                                     ymax,
                                                     zmax,
                                                     imax,
                                                     nPoints);
            }

            // create and add the scene
            CT_Scene* scene = new CT_Scene(pcir);
            scene->setBoundingBox(xmin, ymin, zmin, xmax, ymax, zmax);
            group->addSingularItem(m_hOutScene, scene);

            m_readScene = scene;

            // create and add intensity
            CT_PointsAttributesScalarTemplated<quint16>* pas = m_hOutIntensity.createAttributeInstance(pcir,
                                                                                                       imin,
                                                                                                       imax);

            group->addSingularItem(m_hOutIntensity, pas);

            if ((m_current._rows <= 0) || (m_current._cols <= 0))
                return true;

            // create and add the scanner
            const double hres = 150.0 / (double(m_current._rows));
            const double vres = 360.0 / (double(m_current._cols));
            group->addSingularItem(m_hOutScanner, new CT_Scanner(0,
                                                                 m_current.m_center,
                                                                 Eigen::Vector3d(0,1,0),
                                                                 360,
                                                                 150,
                                                                 hres,
                                                                 vres,
                                                                 0,
                                                                 0,
                                                                 true,
                                                                 false));

            return true;
        }
    }

    return false;
}

CT_NMPCIR CT_Reader_XYB::internalReadFileWithFilter(QDataStream& stream,
                                                    double& xmin,
                                                    double& ymin,
                                                    double& zmin,
                                                    quint16& imin,
                                                    double& xmax,
                                                    double& ymax,
                                                    double& zmax,
                                                    quint16& imax,
                                                    const qint64& nPointToRead) {

    CT_AbstractUndefinedSizePointCloud* mpcir = PS_REPOSITORY->createNewUndefinedSizePointCloud();
    CT_UndefinedPointCloudSizeDenseAttributeSetter<quint16> setter = m_hOutIntensity.createUndefinedPointCloudSizeAttributesSetter(mpcir);

    CT_Point pReaded;
    double x, y, z;
    quint16 reflectance;
    qint64 nPointReaded = 0;

    while(!stream.atEnd()
          && !isStopped())
    {
        stream >> x;
        stream >> y;
        stream >> z;
        stream >> reflectance;

        pReaded(CT_Point::X) = x;
        pReaded(CT_Point::Y) = y;
        pReaded(CT_Point::Z) = z;

        if (!isPointFiltered(pReaded))
        {
            if (x<xmin) {xmin = x;}
            if (x>xmax) {xmax = x;}
            if (y<ymin) {ymin = y;}
            if (y>ymax) {ymax = y;}
            if (z<zmin) {zmin = z;}
            if (z>zmax) {zmax = z;}
            if (reflectance<imin) {imin = reflectance;}
            if (reflectance>imax) {imax = reflectance;}

            mpcir->addPoint(pReaded);
            setter.setLastValue(reflectance);
        }

        ++nPointReaded;
        setProgress(int(nPointReaded*100/nPointToRead));
    }

    return PS_REPOSITORY->registerUndefinedSizePointCloud(mpcir);
}

CT_NMPCIR CT_Reader_XYB::internalReadFileWithoutFilter(QDataStream& stream,
                                                       double& xmin,
                                                       double& ymin,
                                                       double& zmin,
                                                       quint16& imin,
                                                       double& xmax,
                                                       double& ymax,
                                                       double& zmax,
                                                       quint16& imax,
                                                       const qint64& nPointToRead) {

    CT_NMPCIR pcir = PS_REPOSITORY->createNewPointCloud(size_t(nPointToRead));
    CT_DenseAttributeSetter<quint16> setter = m_hOutIntensity.createAttributesSetter(pcir);

    CT_MutablePointIterator it(pcir);

    CT_Point pReaded;
    double x, y, z;
    quint16 reflectance;
    quint64 nPointReaded = 0;

    while(!stream.atEnd()
          && !isStopped())
    {
        stream >> x;
        stream >> y;
        stream >> z;
        stream >> reflectance;

        pReaded(CT_Point::X) = x;
        pReaded(CT_Point::Y) = y;
        pReaded(CT_Point::Z) = z;

        if (x<xmin) {xmin = x;}
        if (x>xmax) {xmax = x;}
        if (y<ymin) {ymin = y;}
        if (y>ymax) {ymax = y;}
        if (z<zmin) {zmin = z;}
        if (z>zmax) {zmax = z;}
        if (reflectance<imin) {imin = reflectance;}
        if (reflectance>imax) {imax = reflectance;}

        it.next();
        it.replaceCurrentPoint(pReaded);
        setter.setValueWithLocalIndex(nPointReaded, reflectance);

        ++nPointReaded;
        setProgress(int(nPointReaded*100.0/nPointToRead));
    }

    return pcir;
}

