#ifndef CT_GLOBALPOINTCLOUDMANAGER_H
#define CT_GLOBALPOINTCLOUDMANAGER_H

#include "ct_cloud/tools/ct_globalcloudmanagert.h"
#include "ct_pointcloud/tools/ct_standardundefinedsizepointcloud.h"
#include "ct_point.h"

/**
 * Class who manage globals cloud of points. The manager create a CT_PointCloudStdVector that have
 * a collection of CT_Point.
 *
 * It's an utility class who manage a global cloud. It will inform the CT_AbstractCloudIndexRegistrationManagerT and all
 * CT_AbstractCloudSyncToGlobalCloudManagerT when points of the cloud is deleted or added. So we can sync index and
 * other cloud with this global cloud.
 */
class CTLIBCLOUDS_EXPORT CT_GlobalPointCloudManager : public QObject, public CT_GlobalCloudManagerT<CT_PointData, CT_InternalPointCloud>
{
    Q_OBJECT

public:

    CT_GlobalPointCloudManager();
    ~CT_GlobalPointCloudManager();

    /**
     * @brief Crée un nouveau nuage de points en mémoire dont la taille est indéfini.
     *
     *        ATTENTION vous ne pouvez créé qu'un nuage dont la taille est indéfini à la fois. Il faut d'abord l'enregistrer
     *        avant de pouvoir en créer un autre. Une fois enregistré le nuage de points ne peux plus être modifié.
     *
     * @return Retourne nullptr si un nuage de points dont la taille est indéfini a déjà été créé et n'a pas encore été enregistré. Sinon
     *         retourne un objet permettant d'ajouter/supprimer des points au nuage de points global.
     */
    CT_AbstractUndefinedSizePointCloud* createNewUndefinedSizePointCloud();

    /**
     * @brief Enregistre le nuage de points dont la taille n'était pas défini.
     *
     *        ATTENTION celui-ci sera supprimé de la mémoire après avoir appelé cette méthode. Vous ne pourrez plus l'utiliser.
     *
     * @param uspc : le nuage de points dont la taille était indéfini
     * @param optim : le type d'optimisation du nuage d'index
     * @return Retourne un nuage d'index correspondant à l'emplacement en mémoire des points. Ce nuage d'index n'est pas modifiable !
     */
    CT_NMPCIR registerUndefinedSizePointCloud(CT_AbstractUndefinedSizePointCloud *uspc, IndexOptimization optim = MemoryOptimized);

private:
    QPair<CT_StandardUndefinedSizePointCloud*, size_t>          m_uspc;

private slots:

    void undefinedSizePointCloudDestroyed(QObject *o);
};

#endif // CT_GLOBALPOINTCLOUDMANAGER_H
