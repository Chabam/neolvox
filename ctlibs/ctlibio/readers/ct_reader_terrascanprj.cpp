#include "ct_reader_terrascanprj.h"
#include <QFile>
#include <QIODevice>
#include <QTextStream>

#include <limits>

CT_Reader_TerraScanPrj::CT_Reader_TerraScanPrj(int subMenuLevel) : SuperClass(subMenuLevel)
{
    addNewReadableFormat(FileFormat("prj", tr("Fichier TerraScan PRJ")));
    setToolTip(tr("Charge un fichier d'emprises de dalles (LIDAR), au format TerraScan PRJ"));
}

CT_Reader_TerraScanPrj::CT_Reader_TerraScanPrj(const CT_Reader_TerraScanPrj &other) : SuperClass(other)
{
}

QString CT_Reader_TerraScanPrj::displayableName() const
{
    return tr("Fichier TerraScan - PRJ");
}

bool CT_Reader_TerraScanPrj::setFilePath(const QString &filepath)
{
    // Test File validity
    if(QFile::exists(filepath))
    {
        QFile f(filepath);

        if (f.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream stream(&f);

            QString line = stream.readLine();
            if (line == "[TerraScan project]")
            {
                    f.close();
                    return CT_AbstractReader::setFilePath(filepath);
            }
            f.close();
        }
    }

    return false;
}


void CT_Reader_TerraScanPrj::internalDeclareOutputModels(CT_ReaderOutModelStructureManager& manager)
{
    manager.addItem(m_box, tr("Emprise de la dalle"));
    manager.addItem(m_fileName, tr("Fichier de la dalle"));
}

bool CT_Reader_TerraScanPrj::internalReadFile(CT_StandardItemGroup* group)
{
    // Test File validity
    if(QFile::exists(filepath()))
    {
        QFile f(filepath());

        if (f.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream stream(&f);
            QString line;

            int file = -1;
            QString fileName = "";
            Eigen::Vector2d min, max;

            while(!stream.atEnd() && !isStopped())
            {
                line = stream.readLine();
                QStringList values = line.split(" ", QT_SKIP_EMPTY_PARTS);

                if (values.size() > 1 && values.at(0) == "Block" && !values.at(1).isEmpty())
                {
                    file = 0;
                    min(0) = std::numeric_limits<double>::max();
                    min(1) = std::numeric_limits<double>::max();
                    max(0) = -std::numeric_limits<double>::max();
                    max(1) = -std::numeric_limits<double>::max();
                    fileName = values.at(1);

                } else if (file >= 0)
                {
                    if (values.size() == 2)
                    {
                        bool okX, okY;
                        double x = values.at(0).toDouble(&okX);
                        double y = values.at(1).toDouble(&okY);

                        if (okX && okY)
                        {
                            file++;
                            if (x < min(0)) {min(0) = x;}
                            if (y < min(1)) {min(1) = y;}
                            if (x > max(0)) {max(0) = x;}
                            if (y > max(1)) {max(1) = y;}
                        }
                    }  else {
                        file = -1;
                    }

                    if (file == 5)
                    {
                        file = -1;

                        CT_Box2DData *data = new CT_Box2DData(min, max);
                        CT_Box2D *box = new CT_Box2D(data);

                        group->addSingularItem(m_box, box);

                        CT_FileHeader *header = new CT_FileHeader();
                        header->setFilePath(fileName);

                        group->addSingularItem(m_fileName, header);
                    }
                }
            }

            f.close();

            return true;
        }
    }

    return false;
}
