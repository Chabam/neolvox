#ifndef CDM_STEPSMENUMANAGER_H
#define CDM_STEPSMENUMANAGER_H

#include "computreeCore_global.h"
#include "ct_step/tools/menu/ct_stepsmenu.h"

class QXmlStreamWriter;
class QXmlStreamReader;
class CT_VirtualAbstractStep;
class CDM_PluginManager;

class COMPUTREECORESHARED_EXPORT CDM_StepsMenuManager
{
public:
    CDM_StepsMenuManager();
    ~CDM_StepsMenuManager();

    /**
     * @brief Set the plugin manager
     */
    void setPluginManager(CDM_PluginManager *pluginManager);

    /**
     * @brief get the menu to fill with steps and level
     */
    CT_StepsMenu* stepsMenu() const;

    /**
     * @brief Clear all the menu
     */
    void clear();

    /**
     * @brief Load the favorites level from a file
     */
    void loadFavoritesFrom(const QString &filepath);

    /**
     * @brief Save the favorites level to file
     */
    void saveFavoritesTo(const QString &filepath);

    /**
     * @brief Returns the default extension of favorites file
     */
    QString favoriteDefaultFileExtension() const;

private:
    CT_StepsMenu        *m_menuOfSteps;
    CDM_PluginManager   *m_pluginManager;

    void writeLevelInformationToFavoritesFileRecursively(CT_MenuLevel *level, QXmlStreamWriter &xmlWriter);
    void writeStepInformationToFavoritesFileRecursively(const CT_VirtualAbstractStep *step, QXmlStreamWriter &xmlWriter);

    void readLevelAndAddItToFavoritesRecursively(CT_MenuLevel *parentLevel, QXmlStreamReader &xmlReader);
    void readStepAndAddItToLevel(CT_MenuLevel *parentLevel, QXmlStreamReader &xmlReader);
};

#endif // CDM_STEPSMENUMANAGER_H
