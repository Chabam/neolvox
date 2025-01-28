/**
 * @author Michael Krebs (AMVALOR)
 * @date 25.01.2017
 * @version 1
 */
#ifndef LVOX3_STEPLOADFILES_H
#define LVOX3_STEPLOADFILES_H

#include "ct_step/abstract/ct_abstractstep.h"

#include "ct_reader/abstract/ct_abstractreader.h"

#include "ct_itemdrawable/ct_scanner.h"
#include "ct_itemdrawable/ct_scene.h"
#include "ct_itemdrawable/ct_shootingpatternd.h"
#include "ct_itemdrawable/tools/scanner/ct_shootingpattern.h"


#include "view/loadfileconfiguration.h"

class CT_Scanner;

class LVOX3_StepLoadFiles : public CT_AbstractStep
{
    Q_OBJECT

public:
    LVOX3_StepLoadFiles();
    ~LVOX3_StepLoadFiles();

    QString description() const;

    QString detailledDescription() const;

    CT_VirtualAbstractStep* createNewInstance() const final;

    void savePostSettings(SettingsWriterInterface& writer) const final;

    bool restorePostSettings(SettingsReaderInterface &reader) final;

    CT_ShootingPattern *makeShootingPattern(const LoadFileConfiguration::Configuration &conf, CT_PCIR pcir);

    QString parametersDescription();

    QString outputDescription() const;


protected:

    void declareInputModels(CT_StepInModelStructureManager& manager) final;

    void declareOutputModels(CT_StepOutModelStructureManager& manager) final;

    void compute() final;

    bool postInputConfigure();

private:
    bool                                            m_useUserScannerConfiguration;
    bool                                            m_filterPointsOrigin;
    bool                                            m_restrictScene;
    int                                             m_restrictRadius;
    CT_AbstractReader*                              m_reader;
    QList<LoadFileConfiguration::Configuration>     m_configuration;   



    CT_HandleOutResultGroup                         m_hOutResult;
    CT_HandleOutStdGroup                            m_hOutRootGroup;
    CT_HandleOutSingularItem<CT_Scanner>            m_outScanner;
    CT_HandleOutSingularItem<CT_ShootingPatternD>   m_outShootingPattern;

    /**
     * @brief Returns a vector for the new coordinates of the point, if it's further than 60m of the scanner coordinates
     */
    bool evaluatePoint(const Eigen::Vector3d scannerCenterCoords, const CT_Point &point, Eigen::Vector3d &newPointCoordinates);
};

#endif // LVOX3_STEPLOADFILES_H
