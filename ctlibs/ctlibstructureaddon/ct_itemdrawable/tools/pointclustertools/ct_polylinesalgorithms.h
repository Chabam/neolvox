#ifndef CT_POLYLINESALGORITHMS_H
#define CT_POLYLINESALGORITHMS_H

#include "ctlibstructure_global.h"
#include "ct_itemdrawable/ct_pointcluster.h"

class CTLIBSTRUCTUREADDON_EXPORT CT_PolylinesAlgorithms
{
public:

    static void createPolyline2D(const CT_PointCluster *baseCluster, CT_PointCluster *polylineCluster);

    static Eigen::Vector2d compute2DArcData(const CT_PointCluster *polyline, double &sagitta, double &chord, double &radius);


    /**
      * \struct CandidatePoint
      * \brief  Elments caractristiques d'un point candidat  l'ajout
      */
    struct CandidatePoint {
        size_t indexInCloud;		/*!< Indice du point dans le nuage de points global.*/
        size_t indexOfMinDistance;	/*!< Indice du point dans le nuage global (  l'interieur du mme groupe) qui minimise la distance   la ligne*/
        double distanceToLine;	/*!< Distance du point a la ligne*/
    };

};

#endif // CT_POLYLINESALGORITHMS_H
