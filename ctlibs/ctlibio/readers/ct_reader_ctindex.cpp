#include "ct_reader_ctindex.h"

//#include "ctlibplugin/ct_step/tools/menu/ct_stepsmenu.h"
#include "ctliblas/readers/ct_reader_lasv2.h"
#include "ctliblaz/readers/ct_reader_laz.h"

#include "headers/ct_ctiheader.h"

#include "ct_log/ct_logmanager.h"
#include <QFile>
#include <QDataStream>
#include <QTextStream>

#include <limits>

CT_Reader_CTIndex::CT_Reader_CTIndex(int subMenuLevel) : SuperClass(subMenuLevel)
{
    addNewReadableFormat(FileFormat("cti", tr("Fichiers d'indexation de points CTI")));
    setToolTip(tr("Le format CTI indexe des listes de points depuis une liste de fichiers de référence"));
    m_reader = nullptr;
}

CT_Reader_CTIndex::CT_Reader_CTIndex(const CT_Reader_CTIndex& other) : SuperClass(other)
{
    m_fileType = other.m_fileType;
    m_areaType = other.m_areaType;
    m_areaSmallType = other.m_areaSmallType;

    if (other.m_reader == nullptr)
    {
        m_reader = nullptr;
    } else {
        m_reader = other.m_reader->copyFull();
    }    
}

CT_Reader_CTIndex::~CT_Reader_CTIndex()
{
    if (m_reader != nullptr) {delete m_reader;}
}

QString CT_Reader_CTIndex::displayableName() const
{
    return tr("Index de points, Fichier CTI");
}

bool CT_Reader_CTIndex::setFilePath(const QString& filepath)
{
    QString error;
    CT_CTIHeader* header = static_cast<CT_CTIHeader*>(internalReadHeader(filepath, error));

    if (!error.isEmpty()) {
        delete header;
        return false;
    }

    if (CT_AbstractReader::setFilePath(filepath)) {

        if (m_reader == nullptr)
        {
            if (header->fileFormat() == "LAS")
            {
                m_reader = new CT_Reader_LASV2();
            } else if (header->fileFormat() == "LAZ")
            {
                m_reader = new CT_Reader_LAZ();
            }
        }

        m_areaType = header->areaShapeType();
        m_areaSmallType = header->areaSmallShapeType();

        delete header;
        return true;
    }

    delete header;
    return false;
}

CT_FileHeader *CT_Reader_CTIndex::createHeaderPrototype() const
{
    return new CT_CTIHeader();
}


CT_FileHeader* CT_Reader_CTIndex::internalReadHeader(const QString& filepath, QString& error) const
{
    Q_UNUSED(error);

    CT_CTIHeader* header = static_cast<CT_CTIHeader*>(createHeaderPrototype());
    header->setFilePath(filepath);

    if(QFile::exists(filepath))
    {
        QFile f(filepath);

        if(f.open(QFile::ReadOnly))
        {
            QDataStream stream(&f);

            QString fileFormat;
            stream >> fileFormat;
            header->setFileFormat(fileFormat);

            bool bit64;
            stream >> bit64;
            header->setBit64(bit64);

            double xmin, ymin, xmax, ymax;
            stream >> xmin >> ymin >> xmax >> ymax;
            header->setExtent(xmin, ymin, xmax, ymax);

            QString areaShapeType;
            stream >> areaShapeType;

            if (areaShapeType == "CIRCLE") {
                double x, y, radius;
                stream >> x >> y >> radius;

                header->setCircle2DArea(x, y, radius);

            } else if (areaShapeType == "POLYGON") {
                int nVert;
                stream >> nVert;

                QVector<Eigen::Vector2d> verts(nVert);
                for (int i = 0 ; i < nVert ; i++)
                {
                    double x, y;
                    stream >> x >> y;
                    verts[i](0) = x;
                    verts[i](1) = y;
                }
                header->setPolygon2DArea(verts);

            } else {
                stream >> xmin >> ymin >> xmax >> ymax;
                header->setBox2DArea(xmin, ymin, xmax, ymax);
            }


            QString areaSmallShapeType;
            stream >> areaSmallShapeType;

            if (areaSmallShapeType == "CIRCLE") {
                double x, y, radius;
                stream >> x >> y >> radius;

                header->setCircle2DAreaSmall(x, y, radius);

            } else if (areaSmallShapeType == "POLYGON") {
                int nVert;
                stream >> nVert;

                QVector<Eigen::Vector2d> verts(nVert);
                for (int i = 0 ; i < nVert ; i++)
                {
                    double x, y;
                    stream >> x >> y;
                    verts[i](0) = x;
                    verts[i](1) = y;
                }
                header->setPolygon2DAreaSmall(verts);

            } else {
                stream >> xmin >> ymin >> xmax >> ymax;
                header->setBox2DAreaSmall(xmin, ymin, xmax, ymax);
            }


            header->setFirstFileIndexPos(f.pos());

            f.close();
        }

        // retreive BasePath in side file
        QFileInfo info(filepath);
        QString path = info.path() + "/" + "sourcePath.txt";

        QFile fp(path);

        if(fp.open(QFile::ReadOnly | QFile::Text))
        {
            QTextStream stream(&fp);

            if (!stream.atEnd())
            {
                QString line = stream.readLine();
                header->setBasePath(line);
            }
            fp.close();
        }
    }

    return header;
}

bool CT_Reader_CTIndex::restoreSettings(SettingsReaderInterface &reader)
{
    return SuperClass::restoreSettings(reader);
}


void CT_Reader_CTIndex::internalDeclareOutputModels(CT_ReaderOutModelStructureManager& manager)
{
    // Return the unique new point cloud (from the specific reader we have loaded)
    if (m_reader != nullptr)
    {
        m_reader->declareOutputModels(manager);

        if (m_areaType == "BOX")
        {
            manager.addItem(m_outBox2D, tr("Emprise Complète"));
        } else if (m_areaType == "CIRCLE")
        {
            manager.addItem(m_outCircle2D, tr("Emprise Complète"));
        } else if (m_areaType == "POLYGON")
        {
            manager.addItem(m_outPolygon2D, tr("Emprise Complète"));
        }

        if (m_areaSmallType == "BOX")
        {
            manager.addItem(m_outBox2DSmall, tr("Emprise Réduite"));
        } else if (m_areaSmallType == "CIRCLE")
        {
            manager.addItem(m_outCircle2DSmall, tr("Emprise Réduite"));
        } else if (m_areaSmallType == "POLYGON")
        {
            manager.addItem(m_outPolygon2DSmall, tr("Emprise Réduite"));
        }
    }
}

bool CT_Reader_CTIndex::internalReadFile(CT_StandardItemGroup* group)
{    
    m_fileBufferList.clear();
    QString error;
    CT_CTIHeader *header = static_cast<CT_CTIHeader*>(internalReadHeader(filepath(), error));

    if(header == nullptr) {
        PS_LOG->addErrorMessage(LogInterface::reader, tr("Impossible de lire l'en-tête du fichier %1").arg(filepath()));
        return false;
    }

    // Read existing buffer file using local variables
    if(QFile::exists(filepath()))
    {
        QFile f(filepath());

        if(f.open(QFile::ReadOnly))
        {
            f.seek(header->firstFileIndexPos());
            QDataStream stream(&f);

            QString filename;
            bool all;
            qint64 tmp;
            qint64 index;
            qint32 index32;

            while(!stream.atEnd())
            {
                FileBuffer fileBuffer;

                stream >> filename;
                stream >> all;
                stream >> tmp;

                fileBuffer.nPoints = tmp;

                // Reference files include all points, and thus create sequential list of indices.
                if (all)
                {
                    fileBuffer.indexList.resize(fileBuffer.nPoints);
                    for(qint64 i = 0 ; i < fileBuffer.nPoints; i++)
                    {
                        fileBuffer.indexList[i] = i;
                    }
                } else
                {
                    fileBuffer.indexList.resize(fileBuffer.nPoints);
                    for(qint64 i = 0 ; i < fileBuffer.nPoints; i++)
                    {
                        if (header->bit64())
                        {
                            stream >> index;
                            fileBuffer.indexList[i] = index;
                        } else {
                            stream >> index32;
                            fileBuffer.indexList[i] = qint64(index32);
                        }

                    }
                }

                // Use data path to get absolute file path
                fileBuffer.filename = header->basePath() + "/" + filename;

                m_fileBufferList.append(fileBuffer);
            }
            f.close();
        }
    }

    // Finally read the data files using the selected reader
    if(m_reader != nullptr)
    {
       if(m_reader->setMultipleFilePath(m_fileBufferList))
        {
            m_reader->readFile(group);

            if (m_areaType == "BOX")
            {
                group->addSingularItem(m_outBox2D, new CT_Box2D(new CT_Box2DData(header->box2d())));
            } else if (m_areaType == "CIRCLE")
            {
                group->addSingularItem(m_outCircle2D, new CT_Circle2D(new CT_Circle2DData(header->circle2d())));
            } else if (m_areaType == "POLYGON")
            {
                group->addSingularItem(m_outPolygon2D, new CT_Polygon2D(new CT_Polygon2DData(header->polygon2d())));
            }

            if (m_areaSmallType == "BOX")
            {
                group->addSingularItem(m_outBox2DSmall, new CT_Box2D(new CT_Box2DData(header->box2dSmall())));
            } else if (m_areaSmallType == "CIRCLE")
            {
                group->addSingularItem(m_outCircle2DSmall, new CT_Circle2D(new CT_Circle2DData(header->circle2dSmall())));
            } else if (m_areaSmallType == "POLYGON")
            {
                group->addSingularItem(m_outPolygon2DSmall, new CT_Polygon2D(new CT_Polygon2DData(header->polygon2dSmall())));
            }

            delete header;
            return true;
        }
    }
    else
    {
        QString warn = tr("Reader for %1 wasn't created properly.").arg(m_fileType);
        PS_LOG->addWarningMessage(LogInterface::reader, warn);
    }


    // If anything fails before
    delete header;
    return false;
}
