#ifndef CT_MENULEVEL_H
#define CT_MENULEVEL_H

#include "ct_abstractstepplugin.h"
#include "ct_stepsmenu.h"

#include <QList>

class CT_VirtualAbstractStep;

/**
 * @brief A level is identified by a displayable name and contains a collection of step
 *        and child level
 */
class CTLIBPLUGIN_EXPORT CT_MenuLevel : public QObject
{
    Q_OBJECT

public:
    ~CT_MenuLevel();

    /**
     * @brief This is a special structure that you can use when a step
     *        was not found when plugins has been loaded
     */
    struct CT_NotFoundedStep {

        /**
         * @brief The key of the step in the plugin
         */
        QString stepKey;

        /**
         * @brief The plugin name
         */
        QString pluginName;

        bool operator ==(const CT_NotFoundedStep &other) {
            return (other.stepKey == stepKey) && (other.pluginName == pluginName);
        }

        bool operator !=(const CT_NotFoundedStep &other) {
            return (other.stepKey != stepKey) || (other.pluginName != pluginName);
        }
    };

    /**
     * @brief Create a level with a custom displayable name and add it to
     *        the specified parent level if it does not already exist, otherwise return
     *        it.
     */
    static CT_MenuLevel* staticCreateOrGetLevelInParentLevel(const QString& customDisplayableName, CT_MenuLevel* parentLevel, bool isCustom = true);

    /**
     * @brief Create a level with a predefined displayable name and add it to
     *        the specified parent level if it does not already exist, otherwise return
     *        it.
     */
    static CT_MenuLevel* staticCreateOrGetLevelInParentLevel(const CT_StepsMenu::LevelPredefined& predefinedLevel, CT_MenuLevel* parentLevel);

    /**
     * @brief Returns the displayable name of this level
     */
    QString displayableName() const;

    /**
     * @brief Add the type of step to this level. Use this method in your plugin
     * @example level->addStep<PB_CustomStep>(this);
     */
    template<class Step>
    bool addStep(const CT_AbstractStepPlugin* stepPlugin) {
        Step* s = new Step();
        s->setPlugin(stepPlugin);

        return addStepToCollectionOrDeleteIt(s);
    }

    /**
     * @brief Add a step to this menu. If she already exist (method existStepInCollection(...)) in this level she
     *        will be deleted from memory and the method will return false, otherwise it will return true.
     */
    bool addStep(CT_VirtualAbstractStep* step);

    /**
     * @brief Move the specified step from his position to position-1 (up in the menu)
     */
    void raiseStepInMenu(CT_VirtualAbstractStep* step);

    /**
     * @brief Move the specified step from his position to position+1 (down in the menu)
     */
    void lowerStepInMenu(CT_VirtualAbstractStep* step);

    /**
     * @brief Add a step that was not been found (used for favorites levels)
     */
    bool addNotFoundedStep(const CT_NotFoundedStep& nfs);

    /**
     * @brief Returns the list of steps contained in this level
     */
    QList<CT_VirtualAbstractStep*> steps();

    /**
     * @brief Returns the list of steps not founded contained in this level
     */
    QList<CT_NotFoundedStep> stepsNotFounded() const;

    /**
     * @brief Returns the list of levels contained in this level.
     */
    QList<CT_MenuLevel*> levels() const;

    /**
     * @brief Returns only child levels that contain step or than sub-level contain step or etc...
     *        If it was not at least one step in a level recursively, the child level will not be added to the
     *        returned list.
     */
    QList<CT_MenuLevel*> levelsWithSteps() const;

    /**
     * @brief Returns the parent level
     */
    CT_MenuLevel* parentLevel() const;


    /**
     * @brief Define step orders (static), for given step list
     */
    static void defineHighPriorityStepsOrder();

private:
    friend class CT_StepsMenu;
    friend class CT_StepSeparator;
    friend class CT_StepCanBeAddedFirstSeparator;
    friend class CT_StepLoadFileSeparator;
    friend class CT_AbstractStepPlugin;

    CT_MenuLevel();
    CT_MenuLevel(const QString& displayableName);

    /**
     * @brief Check if this level has at least one step and if not test sub-level
     */
    bool isStepsNotEmptyRecursively() const;

    /**
     * @brief Add the step to the collection if it does not already exist otherwiase delete it
     */
    bool addStepToCollectionOrDeleteIt(CT_VirtualAbstractStep* step);

    /**
     * @brief Return true if the step already exist in the collection. It will compare the className
     *        returned by the meta object of the step and compare the result of the method "CT_VirtualAbstractStep::name()" too.
     */
    bool existStepInCollection(const CT_VirtualAbstractStep* step);

    /**
     * @brief Add the specified level to this level
     */
    void addLevel(CT_MenuLevel* level);

    /**
     * @brief Set the parent of this level
     */
    void setParentLevel(CT_MenuLevel* parent);

    /**
     * @brief Returns the level that has the specified displayable name and that was in this level
     */
    CT_MenuLevel* levelFromDisplayableName(const QString& name);

    /**
     * @brief Returns true if it was a sublevel of the favorite level
     */
    bool isAFavoriteSubLevel() const;


private:
    typedef QList<CT_VirtualAbstractStep*>               StepCollection;
    typedef QList<CT_NotFoundedStep>                     StepNotFoundedCollection;
    typedef QList<CT_MenuLevel*>                         LevelCollection;

    StepCollection              m_steps;
    StepNotFoundedCollection    m_stepsNotFounded;
    LevelCollection             m_levelsPredefined;
    LevelCollection             m_levelsCustom;
    QString                     m_displayableName;
    CT_MenuLevel*               m_parent;
    bool                        m_isCustomLevel;


    static QList<QString>   STEPORDER;

    /**
     * @brief Step order comparator
     */
    static bool sortByStepOrder(const CT_VirtualAbstractStep *s1, const CT_VirtualAbstractStep *s2);


private slots:
    /**
     * @brief Called when a sub level that was in this level is deleted
     */
    void subLevelDeleted(QObject* level);

    /**
     * @brief Called when a step that was in this level is deleted
     */
    void stepDeleted(QObject* step);
};

#endif // CT_MENULEVEL_H
