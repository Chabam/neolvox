#include "ct_reader_pcd.h"

#include "ct_log/ct_logmanager.h"

#include "ct_point.h"
#include "ct_cloudindex/registered/ct_standardnotmodifiablecloudindexregisteredt.h"
#include "ct_pointcloudindex/ct_pointcloudindexvector.h"

#include <limits>

#include "ct_pcldefines.h"
#include <pcl/io/pcd_io.h>


CT_Reader_PCD::CT_Reader_PCD(int subMenuLevel) : SuperClass(subMenuLevel)
{
    addNewReadableFormat(FileFormat("pcd", tr("Fichiers de points PCD (Librairie PCL)")));

    setToolTip(tr("Charge un fichier de points au format PCD"));
}

CT_Reader_PCD::CT_Reader_PCD(const CT_Reader_PCD& other) : SuperClass(other)
{
}

QString CT_Reader_PCD::displayableName() const
{
    return tr("Points, fichier PCD");
}

bool CT_Reader_PCD::preVerifyFile(const QString& filepath, QFile& fileOpenReadOnly) const
{
    Q_UNUSED(filepath);
    Q_UNUSED(fileOpenReadOnly);
    return true;
}


void CT_Reader_PCD::internalDeclareOutputModels(CT_ReaderOutModelStructureManager& manager)
{
    manager.addItem(m_hOutScene, tr("Scène"));
}

bool CT_Reader_PCD::internalReadFile(CT_StandardItemGroup* group)
{

    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud (new pcl::PointCloud<pcl::PointXYZ>);
    if (pcl::io::loadPCDFile<pcl::PointXYZ> (filepath().toStdString(), *cloud) == -1)
    {
        PS_LOG->addMessage(LogInterface::error, LogInterface::reader, QString(tr("Echec de la lecture du fichier PCD.")).arg(filepath()));
        return false;
    }

    CT_AbstractUndefinedSizePointCloud* pointCloud = PS_REPOSITORY->createNewUndefinedSizePointCloud();
    CT_Point pReaded;

    for (size_t i = 0; i < cloud->points.size (); ++i)
    {
        pReaded(0) = cloud->points[i].x;
        pReaded(1) = cloud->points[i].y;
        pReaded(2) = cloud->points[i].z;

        pointCloud->addPoint(pReaded);
    }

    if (pointCloud->size() > 0)
    {
        CT_NMPCIR pcir = PS_REPOSITORY->registerUndefinedSizePointCloud(pointCloud);
        CT_Scene *scene = new CT_Scene(pcir);
        scene->updateBoundingBox();
        group->addSingularItem(m_hOutScene, scene);

        return true;
    } else
    {
        delete pointCloud;
    }

    return false;
}
