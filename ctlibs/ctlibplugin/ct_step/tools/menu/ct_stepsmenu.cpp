#include "ct_stepsmenu.h"

#include "ct_menulevel.h"

#include <QMetaEnum>

#include <QDebug>

CT_StepsMenu::CT_StepsMenu()
{
}

CT_StepsMenu::~CT_StepsMenu()
{    
    qDeleteAll(m_rootLevels.begin(), m_rootLevels.end());
}

CT_MenuLevel* CT_StepsMenu::createOrGetRootLevel(CT_StepsMenu::LevelOperationType operation)
{
    if(operation == LO_InternalUse)
        return nullptr;

    CT_MenuLevel* level = levelFromOperation(operation);

    if(level == nullptr) {
        level = new CT_MenuLevel(staticOperationToDisplayableName(operation));
        m_rootLevels.insert(operation, level);
    }

    return level;
}

CT_MenuLevel* CT_StepsMenu::levelFromOperation(CT_StepsMenu::LevelOperationType operation) const
{
    if(operation == LO_InternalUse)
        return nullptr;

    return m_rootLevels.value(operation, nullptr);
}

QList<CT_MenuLevel*> CT_StepsMenu::levels() const
{
    return m_rootLevels.values();
}

bool CT_StepsMenu::removeLevelFromFavorites(CT_MenuLevel* level)
{
    Q_ASSERT(level != nullptr);
    if (level == nullptr) {qDebug() << "CT_StepsMenu::removeLevelFromFavorites" << ", " <<  "level == nullptr"; return false;}

    CT_MenuLevel* favorites = levelFromOperation(LO_Favorites);

    if(favorites != nullptr) {
        CT_MenuLevel* pLevel = level;

        while((pLevel = pLevel->parentLevel()) != nullptr) {
            if(pLevel == favorites) {
                delete level;
                return true;
            }
        }
    }

    return false;
}

QString CT_StepsMenu::staticOperationToDisplayableName(CT_StepsMenu::LevelOperationType operation)
{
    switch(operation) {
    case LO_Load : return QObject::tr("Charger");
    case LO_Export : return QObject::tr("Exporter");
    case LO_WorkFlow : return QObject::tr("Boucles");
    case LO_Points : return QObject::tr("Points");
    case LO_GeometricalShapes : return QObject::tr("Géométrie 3D");
    case LO_Shapes2D : return QObject::tr("Géométrie 2D");
    case LO_Voxels : return QObject::tr("Voxels");
    case LO_Rasters : return QObject::tr("Rasters / Images");
    case LO_Meshes : return QObject::tr("Maillages");
    case LO_Metrics : return QObject::tr("Métriques");
    case LO_Other : return QObject::tr("Autres");
    case LO_Beta : return QObject::tr("Travaux en cours (Beta)");
    case LO_Favorites : return QObject::tr("Favoris");
    case LO_InternalUse : return "Internal must not be use by programmers";
    }

    return QObject::tr("Inconnu TODO !");;
}

QString CT_StepsMenu::staticPredefinedToString(CT_StepsMenu::LevelPredefined predefined)
{           
    switch(predefined) {
    case LP_Filter: return QObject::tr("Filtrer");
    case LP_Create: return QObject::tr("Créer / Fusionner");
    case LP_Extract: return QObject::tr("Extraire");
    case LP_Detect: return QObject::tr("Détecter");
    case LP_Transform: return QObject::tr("Transformer");
    case LP_Classify: return QObject::tr("Classifier");
    case LP_Analyze: return QObject::tr("Analyser");
    case LP_Fit: return QObject::tr("Ajuster");
    case LP_Colorize: return QObject::tr("Coloriser");
    case LP_Clusters: return QObject::tr("Clusteriser");
    case LP_Loops: return QObject::tr("Boucles");
    case LP_Plots: return QObject::tr("Placettes");
    case LP_Footprints: return QObject::tr("Emprises");
    case LP_Crowns: return QObject::tr("Détécter (houppiers)");
    case LP_Stems: return QObject::tr("Détécter (tiges)");
    case LP_Points: return QObject::tr("Points");
    case LP_Items: return QObject::tr("Géométrie 3D");
    case LP_Raster: return QObject::tr("Rasters / Images");
    case LP_Vector: return QObject::tr("Géométrie 2D");
    case LP_GDALRaster: return QObject::tr("GDAL Raster");
    case LP_GDALVector: return QObject::tr("GDAL Vecteur");
    case LP_GDALOthers: return QObject::tr("GDAL Divers");
    case LP_Voxels: return QObject::tr("Voxels");
    case LP_Meshes: return QObject::tr("Maillages");
    case LP_ItemAttributes: return QObject::tr("Attributs");
    case LP_Topography: return QObject::tr("Topographie ");
    case LP_Radiometry: return QObject::tr("Radiométrie ");
    case LP_Others: return QObject::tr("Autres");
    }

    return QObject::tr("Inconnu (TODO)");
}

bool CT_StepsMenu::staticIsPredefinedLevelString(const QString &name)
{
    const QMetaObject& mo = CT_StepsMenu::staticMetaObject;

    int index = mo.indexOfEnumerator("LevelPredefined");

    if(index >= 0) {
        QMetaEnum metaEnum = mo.enumerator(index);

        int s = metaEnum.keyCount();

        for(int i=0; i<s; ++i) {
            if(name == staticPredefinedToString(static_cast<CT_StepsMenu::LevelPredefined>(metaEnum.value(i))))
                return true;
        }
    }

    return false;
}
