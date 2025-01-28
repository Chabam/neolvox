#ifndef CDM_STEPLISTINFOMANAGER_H
#define CDM_STEPLISTINFOMANAGER_H

#include "computreeCore_global.h"
#include "cdm_stepmanager.h"
#include "cdm_pluginmanager.h"

#include "ct_model/inModel/ct_inresultmodelgroup.h"
#include "ct_model/outModel/abstract/ct_outabstractresultmodelgroup.h"
#include "ct_model/inModel/tools/ct_instdresultmodelpossibility.h"

#include <QObject>

class COMPUTREECORESHARED_EXPORT CDM_StepListInfoManager : public QObject
{
    Q_OBJECT
public:
    struct StepInfo
    {
        StepInfo(CT_VirtualAbstractStep* step,
                 int num,
                 int indent,
                 QString stepName,
                 QString stepDescription,
                 QString stepDetailledDescription,
                 QString pluginName)
        {
            _step = step;
            _num = num;
            _indent = indent;
            _stepName = stepName;
            _stepDescription = stepDescription;
            _stepDetailledDescription = stepDetailledDescription;
            _pluginName = pluginName;
            _helpFileName = QString("Step_%1").arg(_num, 3, 10, QLatin1Char('0'));
        }

        CT_VirtualAbstractStep *    _step;
        int                         _num;
        int                         _indent;
        QString                     _stepName;
        QString                     _stepDescription;
        QString                     _stepDetailledDescription;
        QString                     _pluginName;
        QString                     _helpFileName;
    };


    CDM_StepListInfoManager(CDM_StepManager *stepManager, CDM_PluginManager *pluginManager);

    const QList<StepInfo> &getStepInfoList() const;

    QList<CT_VirtualAbstractStep *> steps();

    QString getScriptStepList();
    QString getScriptClickableStepList(QString stepDir);
    QString getScriptStepListToolTip(QList<int> strongSteps);

    QString getScriptStepListParameters();
    QString getScriptStepListInputConfig();
    void recursiveCreateInputTree(QString root, const DEF_CT_AbstractGroupModelOut *group, QList<const CT_OutAbstractModel *> &allPoss, QMultiMap<const CT_OutAbstractModel *, int> &selectedPoss);

    QString getPluginAndStepCitations();
    QString getPluginRIS();

    QString getUsedPlugins();
    QString getPluginListToCite();


    static QString getComputreeCoreRis();

    bool hasStepCitation();

    void generateHTMLDocForAllSteps(QString baseDirectory, QString cssRelativeDirectory, QString parentPage);

    QString getStepIndex(int num, QString parentPage);


public slots:
private:
    QList<CDM_StepListInfoManager::StepInfo>    _stepInfoList;
    QMap<QString, CT_AbstractStepPlugin*>       _pluginList;
    QList<QString>                              _corePluginList;

    CDM_StepManager*                            _stepManager;
    CDM_PluginManager*                          _pluginManager;

    QString                                     _computreeCitationRIS;


    void recursiveCountHierachicalRank(int &count, const CT_AbstractModel *model);
    QString getTabsForHierachicalRank(int count);

    QString generateHTMLDoc(CDM_StepListInfoManager::StepInfo stepInfo, QString directory, QString cssRelativeDirectory, QString parentPage);
};

#endif // CDM_STEPLISTINFOMANAGER_H
