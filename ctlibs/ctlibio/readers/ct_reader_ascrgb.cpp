#include "ct_reader_ascrgb.h"
#include <QFile>
#include <QIODevice>
#include <QTextStream>

#include "ct_colorcloud/ct_colorcloudstdvector.h"
#include "ct_cloudindex/registered/ct_standardnotmodifiablecloudindexregisteredt.h"
#include "ct_log/ct_logmanager.h"
#include "ct_point.h"

#include <limits>

CT_Reader_ASCRGB::CT_Reader_ASCRGB(int subMenuLevel) : SuperClass(subMenuLevel), CT_ReaderPointsFilteringExtension()
{
    m_filterRadius = 0;
    _zminFilter = -std::numeric_limits<double>::max();
    _zmaxFilter = std::numeric_limits<double>::max();

    addNewReadableFormat(FileFormat("asc", tr("Fichiers de points ASCII (XYZRGB, sans entête, RGB [0;1])")));

    setToolTip(tr("Charge un fichier de points au format ASCII, sans entête, ordonné, avec les champs suivants :<br>"
                  "- X  : Coordonnée X<br>"
                  "- Y  : Coordonnée Y<br>"
                  "- Z  : Coordonnée Z<br>"
                  "- R  : Composante rouge, valeur entre 0 et 1 (optionnel)<br>"
                  "- V  : Composante verte, valeur entre 0 et 1 (optionnel)<br>"
                  "- B  : Composante Bleue, valeur entre 0 et 1 (optionnel)<br>"));
}

CT_Reader_ASCRGB::CT_Reader_ASCRGB(const CT_Reader_ASCRGB& other) : SuperClass(other)
{
    m_filterRadius = other.m_filterRadius;
    _zminFilter = other._zminFilter;
    _zmaxFilter = other._zmaxFilter;
}

QString CT_Reader_ASCRGB::displayableName() const
{
    return tr("Points, fichier ASCII (XYZRGB, sans entête, RGB [0;1])");
}

void CT_Reader_ASCRGB::setRadiusFilter(const double &radius)
{
    m_filterRadius = radius;
}

void CT_Reader_ASCRGB::setRadiusFilter(const double &radius, const double &zmin, const double &zmax)
{
    m_filterRadius = radius;
    _zminFilter = zmin;
    _zmaxFilter = zmax;
}

bool readPoint(QStringList &values, CT_Point &point)
{
    if (values.size() <= 2)
        return false;

    bool okX = false;
    bool okY = false;
    bool okZ = false;

    double x = values.at(0).toDouble(&okX);
    double y = values.at(1).toDouble(&okY);
    double z = values.at(2).toDouble(&okZ);

    if (!okX || !okY || !okZ)
        return false;

    point(0) = x;
    point(1) = y;
    point(2) = z;
    return true;
}

void readColor(QStringList &values, CT_Color &color)
{
    bool okR = false;
    bool okG = false;
    bool okB = false;

    double valueR = 1;
    double valueG = 1;
    double valueB = 1;

    if (values.size() > 3) {valueR = values.at(3).toDouble(&okR);}
    if (values.size() > 4) {valueG = values.at(4).toDouble(&okG);}
    if (values.size() > 5) {valueB = values.at(5).toDouble(&okB);}

    if (!okG) {valueG = valueR;}
    if (!okB) {valueB = valueR;}

    valueR = qBound(0., valueR, 1.);
    valueG = qBound(0., valueG, 1.);
    valueB = qBound(0., valueB, 1.);

    color(0) = uchar(255*valueR);
    color(1) = uchar(255*valueG);
    color(2) = uchar(255*valueB);
    color(3) = 255;
}

/*
 * Check if the point is inside the bounds of a cylinder
 */
bool CT_Reader_ASCRGB::isInsideRadius(const CT_Point &point)
{
    if (m_filterRadius > 0) {
        double distance2D = sqrt(point.x() * point.x() + point.y() * point.y());
        return (distance2D <= m_filterRadius &&
                point.z() >= _zminFilter &&
                point.z() <= _zmaxFilter);
    }
    return true;
}

bool CT_Reader_ASCRGB::preVerifyFile(const QString& filepath, QFile& fileOpenReadOnly) const
{
    QTextStream stream(&fileOpenReadOnly);

    stream.readLine();

    const QString line = stream.readLine();

    if(!line.isNull())
    {
        QStringList values = line.split(" ");
        if (values.size() >= 3)
            return true;
        else
            PS_LOG->addMessage(LogInterface::error, LogInterface::reader, QString(tr("Le fichier %1 n'a pas le bon format (colonnes manquantes).")).arg(filepath));
    } else {
        PS_LOG->addMessage(LogInterface::error, LogInterface::reader, QString(tr("Le fichier %1 est vide.")).arg(filepath));
    }

    return false;
}

void CT_Reader_ASCRGB::internalDeclareOutputModels(CT_ReaderOutModelStructureManager& manager)
{
    manager.addItem(m_hOutScene, tr("Scène"));
    manager.addPointAttribute(m_hOutColors, tr("Attribut de points (couleurs)"));
}

bool CT_Reader_ASCRGB::internalReadFile(CT_StandardItemGroup* group)
{
    if (QFile::exists(filepath()))
    {
        QFile f(filepath());

        if (f.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream stream(&f);

            CT_AbstractUndefinedSizePointCloud* pointCloud = PS_REPOSITORY->createNewUndefinedSizePointCloud();
            auto colorSetter = m_hOutColors.createUndefinedPointCloudSizeAttributesSetter(pointCloud);

            double xmin = std::numeric_limits<double>::max();
            double ymin = std::numeric_limits<double>::max();
            double zmin = std::numeric_limits<double>::max();

            double xmax = -std::numeric_limits<double>::max();
            double ymax = -std::numeric_limits<double>::max();
            double zmax = -std::numeric_limits<double>::max();

            QString line;

            // Getting the file size to set progress
            qint64 fileSize = f.size();
            qint64 currentSizeRead = 0;

            CT_Point point;
            CT_Color color;

            while(!stream.atEnd()
                  && !isStopped())
            {
                line = stream.readLine();
                currentSizeRead += line.size();
                setProgress(int(currentSizeRead*100)/fileSize);
                QStringList values = line.split(" ", QT_SKIP_EMPTY_PARTS);
                bool ok = readPoint(values, point);

                // FIXME: convert radius filter to lambda
                if (!ok || isPointFiltered(point) || !isInsideRadius(point))
                    continue;

                readColor(values, color);

                if (isPointFiltered(point))
                    continue;

                pointCloud->addPoint(point);
                colorSetter.setLastValue(color);

                /* update bounding box */
                if (point.x() < xmin) {xmin = point.x();}
                if (point.x() > xmax) {xmax = point.x();}
                if (point.y() < ymin) {ymin = point.y();}
                if (point.y() > ymax) {ymax = point.y();}
                if (point.z() < zmin) {zmin = point.z();}
                if (point.z() > zmax) {zmax = point.z();}
            }

            f.close();

            if(pointCloud->size() != 0)
            {
                CT_NMPCIR pcir = PS_REPOSITORY->registerUndefinedSizePointCloud(pointCloud);

                CT_Scene *scene = new CT_Scene(pcir);
                scene->setBoundingBox(xmin, ymin, zmin, xmax, ymax, zmax);
                group->addSingularItem(m_hOutScene, scene);
                group->addSingularItem(m_hOutColors, m_hOutColors.createAttributeInstance(pcir));

                m_readScene = scene;

                return true;
            }
            else
            {
                delete pointCloud;
            }

        } else {
            PS_LOG->addMessage(LogInterface::error, LogInterface::reader, QString(tr("Le fichier %1 n'est pas accessible.")).arg(filepath()));
        }
    } else {
        PS_LOG->addMessage(LogInterface::error, LogInterface::reader, QString(tr("Le fichier %1 n'existe pas.")).arg(filepath()));
    }

    return false;
}
