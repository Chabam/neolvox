#include "ct_reader_larchitect_grid.h"
#include <QFile>
#include <QIODevice>
#include <QTextStream>

#include "ct_colorcloud/ct_colorcloudstdvector.h"
#include "ct_log/ct_logmanager.h"

#include <limits>

#define CLEAR_CLOSE_FALSE(argFunc, argErrMsg) if(argFunc) { delete wood_surface; \
                                                            delete leaf_surface; \
                                                            delete wood_volume; \
                                                            delete leaf_volume; \
                                                            delete all_surface; \
                                                            delete all_volume;  \
                                                            f.close(); \
                                                            PS_LOG->addErrorMessage(LogInterface::reader, argErrMsg); \
                                                            return false; }

#define wood_surface_Exist (wood_surface != nullptr)
#define wood_volume_Exist (wood_volume != nullptr)
#define leaf_surface_Exist (leaf_surface != nullptr)
#define leaf_volume_Exist (leaf_volume != nullptr)
#define all_surface_Exist (all_surface != nullptr)
#define all_volume_Exist (all_volume != nullptr)

CT_Reader_LArchitect_Grid::CT_Reader_LArchitect_Grid(int subMenuLevel) : SuperClass(subMenuLevel)
{
    addNewReadableFormat(FileFormat("grid", tr("Fichiers de grilles 3D LArchitect")));
    setToolTip(tr("Charge des grilles 3D depuis un fichier au format LArchitect"));
}

CT_Reader_LArchitect_Grid::CT_Reader_LArchitect_Grid(const CT_Reader_LArchitect_Grid& other) : SuperClass(other)
{
}



QString CT_Reader_LArchitect_Grid::displayableName() const
{
    return tr("Grilles 3D, format LArchitect");
}

bool CT_Reader_LArchitect_Grid::setFilePath(const QString &filepath)
{
    // Test File validity
    if(QFile::exists(filepath))
    {
        QFile f(filepath);

        if (f.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream stream(&f);

            Eigen::Vector3d min;
            Eigen::Vector3d max;
            Eigen::Vector3d res;
            Eigen::Vector3i dim;
            int nMat;
            QMap<QString, int>  matNames;

            if(readHeader(stream, min, max, res, dim, nMat, matNames)) {
                f.close();
                return CT_AbstractReader::setFilePath(filepath);
            } else {
                PS_LOG->addErrorMessage(LogInterface::reader, tr("Error when read header of the file %1").arg(filepath));
            }

            f.close();
        }
    }

    return false;
}


void CT_Reader_LArchitect_Grid::internalDeclareOutputModels(CT_ReaderOutModelStructureManager& manager)
{
    manager.addItem(m_wood_surface, tr("Wood surface"));
    manager.addItem(m_leaf_surface, tr("Leaf surface"));
    manager.addItem(m_wood_volume, tr("Wood volume"));
    manager.addItem(m_leaf_volume, tr("Leaf volume"));
    manager.addItem(m_all_surface, tr("Total surface"));
    manager.addItem(m_all_volume, tr("Total volume"));
}

bool CT_Reader_LArchitect_Grid::internalReadFile(CT_StandardItemGroup* group)
{
    // Test File validity
    if(QFile::exists(filepath()))
    {
        QFile f(filepath());

        if (f.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream stream(&f);

            Eigen::Vector3d min;
            Eigen::Vector3d max;
            Eigen::Vector3d res;
            Eigen::Vector3i dim;
            int nMat;
            QMap<QString, int>  matNames;

            if(readHeader(stream, min, max, res, dim, nMat, matNames)) {

                CT_Grid3D<float> *wood_surface = nullptr;
                CT_Grid3D<float> *leaf_surface = nullptr;
                CT_Grid3D<float> *wood_volume = nullptr;
                CT_Grid3D<float> *leaf_volume = nullptr;
                CT_Grid3D<float> *all_surface = nullptr;
                CT_Grid3D<float> *all_volume = nullptr;
                CT_Grid3D<float> *refGrid = nullptr;

                if (1)
                {
                    wood_surface = createGrid(min, res, dim);
                    refGrid = wood_surface;
                }

                if (1)
                {
                    leaf_surface = createGrid(min, res, dim);
                    refGrid = leaf_surface;
                }

                if (1)
                {
                    wood_volume = createGrid(min, res, dim);
                    refGrid = wood_volume;
                }

                if (1)
                {
                    leaf_volume = createGrid(min, res, dim);
                    refGrid = leaf_volume;
                }

                if (1)
                {
                    all_surface = createGrid(min, res, dim);
                    refGrid = all_surface;
                }

                if (1)
                {
                    all_volume = createGrid(min, res, dim);
                    refGrid = all_volume;
                }

                int wood_index = matNames.value("WOOD", -1);
                int leaf_index = matNames.value("FOLIAGE", -1);

                CLEAR_CLOSE_FALSE ((wood_surface_Exist || wood_volume_Exist) && wood_index==-1, tr("Pas d'index bois dans le fichier"))
                CLEAR_CLOSE_FALSE((leaf_surface_Exist|| leaf_volume_Exist) && leaf_index==-1, tr("Pas d'index feuille dans le fichier"))

                qint64 fileSize = f.size();
                qint64 readedSize = 0;

                bool ok;
                bool okSurf, okVol;
                float surface, volume;
                int pos, code;
                size_t index;
                QString line;
                QStringList list;
                int lSize;

                for (int xx = 0 ; (xx < dim[0]) && !isStopped(); ++xx)
                {
                    for (int yy = 0 ; (yy < dim[1]) && !isStopped(); ++yy)
                    {
                        for (int zz = 0 ; (zz < dim[2]) && !stream.atEnd() && !isStopped(); ++zz)
                        {
                            line = stream.readLine();
                            readedSize += line.size();

                            list = line.split(" ");
                            lSize = list.size();

                            if (refGrid->index(xx, yy, zz, index))
                            {
                                for (int i = 0 ; i < nMat ; ++i)
                                {
                                    pos = 3*i;
                                    if (lSize >= (pos + 1))
                                    {
                                        code = list.at(pos).toInt(&ok);
                                        if (ok)
                                        {
                                            if (code == wood_index && list.size() >= pos + 3)
                                            {
                                                surface = list.at(pos+1).toFloat(&okSurf);
                                                volume = list.at(pos+2).toFloat(&okVol);

                                                if (wood_surface_Exist && okSurf) {wood_surface->setValueAtIndex(index, surface);}
                                                if (wood_volume_Exist && okVol)  {wood_volume->setValueAtIndex(index, volume);}
                                                if (all_surface_Exist && okSurf)  {all_surface->addValueAtIndex(index, surface);}
                                                if (all_volume_Exist && okVol)   {all_volume->addValueAtIndex(index, volume);}


                                            } else if (code == leaf_index && list.size() >= pos + 3)
                                            {
                                                surface = list.at(pos+1).toFloat(&okSurf);
                                                volume = list.at(pos+2).toFloat(&okVol);

                                                if (leaf_surface_Exist && okSurf) {leaf_surface->setValueAtIndex(index, surface);}
                                                if (leaf_volume_Exist && okVol)  {leaf_volume->setValueAtIndex(index, volume);}
                                                if (all_surface_Exist && okSurf)  {all_surface->addValueAtIndex(index, surface);}
                                                if (all_volume_Exist && okVol)   {all_volume->addValueAtIndex(index, volume);}

                                            }
                                        }
                                    }
                                }
                            }
                            else
                            {
                                PS_LOG->addWarningMessage(LogInterface::reader, tr("Indice hors grille"));
                            }

                            list.clear();
                        }
                    }

                    setProgress(int((readedSize*100.0)/fileSize));
                }

                f.close();

                if (wood_surface_Exist) {wood_surface->computeMinMax();}
                if (wood_volume_Exist) {wood_volume->computeMinMax();}
                if (leaf_surface_Exist) {leaf_surface->computeMinMax();}
                if (leaf_volume_Exist) {leaf_volume->computeMinMax();}
                if (all_surface_Exist) {all_surface->computeMinMax();}
                if (all_volume_Exist) {all_volume->computeMinMax();}

                group->addSingularItem(m_all_surface, all_surface);
                group->addSingularItem(m_all_volume, all_volume);
                group->addSingularItem(m_leaf_surface, leaf_surface);
                group->addSingularItem(m_leaf_volume, leaf_volume);
                group->addSingularItem(m_wood_surface, wood_surface);
                group->addSingularItem(m_wood_volume, wood_volume);

                return true;
            }

            f.close();
        }
    }

    return false;
}

bool CT_Reader_LArchitect_Grid::readHeader(QTextStream &stream, Eigen::Vector3d &min, Eigen::Vector3d &max, Eigen::Vector3d &res, Eigen::Vector3i &dim, int &nMat, QMap<QString, int> &matNames) const
{
    if(stream.atEnd())
        return false;

    // Read (x,y,z) min and max limits of the grid
    QString line = stream.readLine();

    QStringList values = line.split(" ");

    if(values.size() < 6)
        return false;

    bool ok[6];
    min[0] = values.at(0).toDouble(&ok[0]);
    min[1] = values.at(1).toDouble(&ok[1]);
    min[2] = values.at(2).toDouble(&ok[2]);
    max[0] = values.at(3).toDouble(&ok[3]);
    max[1] = values.at(4).toDouble(&ok[4]);
    max[2] = values.at(5).toDouble(&ok[5]);

    if (!ok[0] || !ok[1] || !ok[2] || !ok[3] || !ok[4] || !ok[5])
        return false;

    // read resolution of the grid
    line = stream.readLine();

    values = line.split(" ");

    if (values.size() < 3)
        return false;

    res[0] = values.at(0).toDouble(&ok[0]);
    res[1] = values.at(1).toDouble(&ok[1]);
    res[2] = values.at(2).toDouble(&ok[2]);

    if (!ok[0] || !ok[1] || !ok[2])
        return false;

    double epsilon = 0.000000000001;
    if (fabs(res[0] - res[1]) > epsilon || fabs(res[0] - res[2]) > epsilon)
        return false;

    // read dimensions of the grid
    line = stream.readLine();

    values = line.split(" ");

    if (values.size() < 3)
        return false;

    dim[0] = values.at(0).toInt(&ok[0]);
    dim[1] = values.at(1).toInt(&ok[1]);
    dim[2] = values.at(2).toInt(&ok[2]);

    if (!ok[0] || !ok[1] || !ok[2])
        return false;

    // read number of materials
    line = stream.readLine();

    values = line.split(" ");

    int size = values.size();

    if(size > 0)
    {
        nMat = values.at(0).toInt(&ok[0]);

        if (!ok[0] || (nMat <= 0) || (size < (2*nMat+1)))
            return false;

        int matid;
        QString matName;

        // read materials ids and names
        for (int i = 0 ; i < nMat; ++i)
        {
            matid = values.at(2*i+1).toInt(&ok[0]);
            matName = values.at(2*i+2);

            if (!ok[0] || matName.size() <= 0)
                return false;

            matNames.insert(matName.toUpper(), matid);
        }
    }
    return true;
}

CT_Grid3D<float>* CT_Reader_LArchitect_Grid::createGrid(Eigen::Vector3d &min, Eigen::Vector3d &res, Eigen::Vector3i &dim) const
{
    return new CT_Grid3D<float>(min[0], min[1], min[2], dim[0], dim[1], dim[2], res[0], -1, 0);
}
