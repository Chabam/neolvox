#ifndef CT_STEPSMENU_H
#define CT_STEPSMENU_H

#include "ctlibplugin_global.h"

#include <QMap>
#include <QObject>

class CT_MenuLevel;

/**
 * @brief Represent a structure where steps are sorted into levels
 */
class CTLIBPLUGIN_EXPORT CT_StepsMenu : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief The root level must be one of this
     */
    enum LevelOperationType {
        LO_Favorites,           // Favorites Steps. Must not be used by plugins ! Only by gui or core
        LO_Load,                // Steps that load/import files
        LO_Export,              // Steps that exports files
        LO_WorkFlow,            // Steps that can control the worklow of all other steps
        LO_Points,              // Steps that produce points clouds / clusters
        LO_GeometricalShapes,   // Steps that produce 3D geometrical shapes
        LO_Shapes2D,            // Steps that produce 2D geometrical shapes
        LO_Voxels,              // Steps that produce voxels
        LO_Rasters,             // Steps that produce rasters
        LO_Meshes,              // Steps that produce meshes
        LO_Metrics,             // Steps that produce metrics
        LO_Other,               // Steps that can not be ordered in one of other operation
        LO_Beta,                // Steps that are in Beta(work in progress)
        LO_InternalUse          // Not intended to be used by a developer
    };
    Q_ENUMS(LevelOperationType)

    /**
     * @brief Sub levels can use one of this predefined value
     */
    enum LevelPredefined {
        LP_Others = 0,
        LP_Filter,
        LP_Create,
        LP_Extract,
        LP_Detect,
        LP_Transform,
        LP_Classify,
        LP_Analyze,
        LP_Fit,
        LP_Colorize,
        LP_Clusters,
        LP_Loops,
        LP_Plots,
        LP_Footprints,
        LP_Crowns,
        LP_Stems,
        LP_Points,
        LP_Items,
        LP_Raster,
        LP_Vector,
        LP_GDALRaster,
        LP_GDALVector,
        LP_GDALOthers,
        LP_Voxels,
        LP_Meshes,
        LP_ItemAttributes,
        LP_Topography,
        LP_Radiometry
    };
    Q_ENUMS(LevelPredefined)

    CT_StepsMenu();
    ~CT_StepsMenu();

    /**
     * @brief Create the root level that correspond to the specified operation
     *        if it does not already exist and return it or just return it.
     */
    CT_MenuLevel* createOrGetRootLevel(LevelOperationType operation);

    /**
     * @brief Returns the existing level that correspond to the specified operation. nullptr
     *        if it does not exist.
     */
    CT_MenuLevel* levelFromOperation(LevelOperationType operation) const;

    /**
     * @brief Returns all levels of this menu
     */
    QList<CT_MenuLevel*> levels() const;

    /**
     * @brief Remove a level that was in favorites menu (the level and it's sub level will be deleted from memory)
     */
    bool removeLevelFromFavorites(CT_MenuLevel* level);

    /**
     * @brief Returns a displayable name for the specified operation
     */
    static QString staticOperationToDisplayableName(LevelOperationType operation);

    /**
     * @brief Convert a predefined enum to a displayable name
     */
    static QString staticPredefinedToString(LevelPredefined predefined);

    /**
     * @brief Returns true if the string is the displayable name of a LevelPredefined
     */
    static bool staticIsPredefinedLevelString(const QString& name);

private:
    QMap<LevelOperationType, CT_MenuLevel*>     m_rootLevels;
};

#endif // CT_STEPSMENU_H
