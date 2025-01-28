#include "cdm_steplistinfomanager.h"
#include "ct_abstractstepplugin.h"
#include "ct_step/abstract/ct_virtualabstractstep.h"

#include "ct_risformat/ct_parseris.h"

CDM_StepListInfoManager::CDM_StepListInfoManager(CDM_StepManager *stepManager, CDM_PluginManager *pluginManager) : QObject(nullptr)
{
    _stepManager = stepManager;
    _pluginManager = pluginManager;

    QList<CT_VirtualAbstractStep *> stepList;
    QHash<const CT_VirtualAbstractStep*, const CT_VirtualAbstractStep*> parents;

    const QList<CT_VirtualAbstractStep*> rootList = stepManager->getStepRootList();

    for(CT_VirtualAbstractStep* currentStep : rootList)
    {
        stepList.append(currentStep);

        currentStep->recursiveVisitChildrens([&stepList, &parents](const CT_VirtualAbstractStep* step, const CT_VirtualAbstractStep* child) -> bool {
            stepList.append(const_cast<CT_VirtualAbstractStep*>(child));
            parents.insert(child, step);
            return true;
        });
    }


    QList<int> stepIndent;
    // Compute step indentation and plugin list
    for (int i = 0 ; i < stepList.size() ; i++)
    {
        const CT_VirtualAbstractStep *step = stepList.at(i);

        // Compute plugin list
        CT_AbstractStepPlugin* plugin = step->pluginStaticCastT<>();
        QString pluginName = _pluginManager->getPluginName(_pluginManager->getPluginIndex(plugin));
        QString pluginOfficialName = plugin->getPluginOfficialName();

        if (pluginName.left(5) == "plug_")
        {
            pluginName.remove(0, 5);
        }
        if (pluginOfficialName != "")
        {
            pluginName = pluginOfficialName;
        }

        if (!_pluginList.contains(pluginName))
        {
            _pluginList.insert(pluginName, plugin);
        }

        // Compute indentation
        int indent = -1;
        while (step != nullptr)
        {
            ++indent;
            step = parents.value(step, nullptr);
        }
        stepIndent.append(indent);
    }


    // Create Core plugin list
    _corePluginList.append("Base");
    _corePluginList.append("Toolkit");
    _corePluginList.append("Generate");


    // Create stepInfos
    int count = 1;
    for (int i = 0 ; i < stepList.size() ; i++)
    {
        CT_VirtualAbstractStep* currentStep = stepList.at(i);

        CT_AbstractStepPlugin* plugin = currentStep->pluginStaticCastT<>();
        QString pluginOfficialName = plugin->getPluginOfficialName();
        QString pluginName = _pluginManager->getPluginName(_pluginManager->getPluginIndex(plugin));

        if (pluginName.left(5) == "plug_")
        {
            pluginName.remove(0, 5);
        }
        if (pluginOfficialName != "")
        {
            pluginName = pluginOfficialName;
        }

        _stepInfoList.append(CDM_StepListInfoManager::StepInfo(currentStep, count++, stepIndent.at(i), currentStep->name(), currentStep->description(), currentStep->detailledDescription(), pluginName));
    }
}


const QList<CDM_StepListInfoManager::StepInfo> &CDM_StepListInfoManager::getStepInfoList() const
{
    return _stepInfoList;
}


QList<CT_VirtualAbstractStep *> CDM_StepListInfoManager::steps()
{
    QList<CT_VirtualAbstractStep *> list;

    for (const CDM_StepListInfoManager::StepInfo& stepInfo : qAsConst(_stepInfoList))
    {
        list.append(const_cast<CT_VirtualAbstractStep*>(stepInfo._step));
    }

    return list;
}


QString CDM_StepListInfoManager::getScriptStepList()
{
    QString result;
    for (const CDM_StepListInfoManager::StepInfo &stepInfo : qAsConst(_stepInfoList))
    { 
        // create indentation
        QString spaces = "&nbsp;&nbsp;&nbsp;&nbsp;";
        for (int j = 0 ; j < stepInfo._indent ; j++) {spaces.append("&nbsp;&nbsp;&nbsp;&nbsp;");}

        result.append(spaces + "<strong>"  + QString::number(stepInfo._num) + ". " + stepInfo._stepDescription + "</strong> <em>(plugin " + stepInfo._pluginName + ")</em><br><br>\n");
    }

    return result;
}

QString CDM_StepListInfoManager::getScriptClickableStepList(QString stepDir)
{
    QString result;
    for (const CDM_StepListInfoManager::StepInfo &stepInfo : qAsConst(_stepInfoList))
    {
        // create indentation
        QString spaces;
        for (int j = 0 ; j < stepInfo._indent ; j++) {spaces.append("&nbsp;&nbsp;&nbsp;&nbsp;");}

        result.append(spaces + QString("<a href=\"%1/%2.html\" class=\"tooltip\" style=\"margin-bottom:5px;\"><strong>").arg(stepDir, stepInfo._helpFileName)  + QString::number(stepInfo._num) + ". " + stepInfo._stepDescription + "</strong></a><br>\n");
    }

    return result;
}

QString CDM_StepListInfoManager::getScriptStepListToolTip(QList<int> strongSteps)
{
    QString result;
    for (const CDM_StepListInfoManager::StepInfo &stepInfo : qAsConst(_stepInfoList))
    {
        // create indentation
        QString spaces = "&nbsp;&nbsp;&nbsp;&nbsp;";
        for (int j = 0 ; j < stepInfo._indent ; j++) {spaces.append("&nbsp;&nbsp;&nbsp;&nbsp;");}

        if (strongSteps.contains(stepInfo._num))
        {
            result.append(spaces + "<strong style=\"color:#8B0000;\">" + QString::number(stepInfo._num) + ". " + stepInfo._stepDescription + "</strong><br>\n");
        } else {
            result.append(spaces + QString::number(stepInfo._num) + ". " + stepInfo._stepDescription + "<br>\n");
        }
    }

    return result;
}


QString CDM_StepListInfoManager::getScriptStepListParameters()
{
    QString result;
    for (const CDM_StepListInfoManager::StepInfo& stepInfo : qAsConst(_stepInfoList))
    {
        const CT_VirtualAbstractStep *step = stepInfo._step;

        result.append("<h2>" + stepInfo._stepDescription + " <em>(plugin " + stepInfo._pluginName + ")</em></h2>\n");
        QString parametersString = const_cast<CT_VirtualAbstractStep*>(step)->parametersDescription(false);

        if (parametersString.isEmpty())
        {
            result.append("<div style='margin-left:50px'>\n<em>" + tr("Pas de paramètres pour cette étape.") + "</em>\n</div>\n");
        } else {
            result.append("<div style='margin-left:50px'>\n" + parametersString + "\n</div>\n");
        }
    }

    return result;
}


// TESTTUTO
QString CDM_StepListInfoManager::getScriptStepListInputConfig()
{
    QString result;
    for (const CDM_StepListInfoManager::StepInfo& stepInfo : qAsConst(_stepInfoList))
    {
        const CT_VirtualAbstractStep *step = stepInfo._step;

        QList<const CT_InAbstractResultModel*> resultInModels;

        step->visitInResultModels([this, step, &resultInModels](const CT_InAbstractResultModel* child) -> bool {
            resultInModels.append(child);
            qDebug();
            qDebug() << step->displayableName();

            child->visitSelectedPossibilities([this](const CT_InAbstractModel* inModel, const CT_InStdModelPossibility* possibility) -> bool {
                Q_UNUSED(inModel);
                const CT_InStdResultModelPossibility* resultPossibility = dynamic_cast<const CT_InStdResultModelPossibility*>(possibility);


                if (resultPossibility != nullptr)
                {
                    CT_OutAbstractResultModelGroup* outResModel = dynamic_cast<CT_OutAbstractResultModelGroup*>(resultPossibility->outModel());
                    CT_InResultModelGroup* inResModel = dynamic_cast<CT_InResultModelGroup*>(resultPossibility->inResultModel());

                    QList<const CT_OutAbstractModel*> allPoss;
                    QMultiMap<const CT_OutAbstractModel*, int> selectedPoss;

                    qDebug() << "------------IN-----------------";
                    int modelCount = 1;
                    inResModel->recursiveVisitInChildrens([this, &allPoss, &selectedPoss, &modelCount] (const CT_InAbstractModel* child)-> bool {

                        if (child->displayableName() != "*")
                        {

                            int count = 0;
                            this->recursiveCountHierachicalRank(count, child);

                            qDebug() << this->getTabsForHierachicalRank(count) << child->displayableName() << " " << modelCount;

                            child->visitPossibilities([&allPoss, &selectedPoss, &modelCount](const CT_InAbstractModel* inModel, const CT_InStdModelPossibility* possibility) -> bool {
                                Q_UNUSED(inModel);
                                allPoss.append(possibility->outModel());
                                if (possibility->isSelected())
                                {
                                    selectedPoss.insert(possibility->outModel(), modelCount);
                                }
                                return true;
                            });
                            modelCount++;
                        }

                        return true;
                    });

                    qDebug() << "-----------------------------";

                    if((outResModel != nullptr)
                        && (inResModel != nullptr)
                        && (inResModel->rootGroup() != nullptr))
                    {
                        QList<QString> retList;

                        // Nom du résultat modèle d'entrée
                        QString rootName = outResModel->step() == nullptr ? tr("No Step") : static_cast<CT_VirtualAbstractStep *>(outResModel->step())->displayableName();
                        retList.append(rootName);
                        qDebug() << rootName;

                        this->recursiveCreateInputTree(rootName, outResModel->rootGroup(), allPoss, selectedPoss);
                    }
                } else {
                    qDebug() << "nullptr";
                }


                return true;
            });

            return true;
        });
    }

    // TESTTUTO
    // Aller voir ici : CTG_InModelPossibilitiesChoice::constructModel
    // CT_ModelSelectionHelper::recursiveSelectAllPossibilitiesByDefault
    // CTG_ModelsLinkConfigurationFlowView

    return result;
}


void CDM_StepListInfoManager::recursiveCreateInputTree(QString root, const DEF_CT_AbstractGroupModelOut* group, QList<const CT_OutAbstractModel*> &allPoss, QMultiMap<const CT_OutAbstractModel*, int> &selectedPoss)
{
    if(group == nullptr)
        return;

    QList<QString> list;

    // le nom du modèle de sortie
    QString rootItem = group->displayableName();
    list.append(rootItem);


    if (allPoss.contains(group))
    {
        QList<int> modelCounts = selectedPoss.values(group);
        std::sort(modelCounts.begin(), modelCounts.end());
        QString str;
        for (int i = 0 ; i < modelCounts.size() ; i++)
        {
            str.append(QString("%1").arg(modelCounts.at(i)));
            if (i < modelCounts.size() - 1)
            {
                str.append(";");
            }
        }

        int count = 0;
        this->recursiveCountHierachicalRank(count, group);

        QString itemGroup = group->displayableName();
        qDebug() << this->getTabsForHierachicalRank(count) << itemGroup << ((modelCounts.size() > 0)?QString(" *** %1").arg(str):"");

        group->visitGroups([this, &rootItem, &allPoss, &selectedPoss](const DEF_CT_AbstractGroupModelOut* g) -> bool {
            this->recursiveCreateInputTree(rootItem, g, allPoss, selectedPoss);
            return true;
        });
    }

    group->visitItems([this, &allPoss, &selectedPoss](const CT_OutAbstractSingularItemModel* item) -> bool {

        QList<QString> list2;

        // le nom du modèle de sortie
        QString itemItem = item->displayableName();
        list2.append(itemItem);

        if (allPoss.contains(item))
        {
            QList<int> modelCounts = selectedPoss.values(item);
            std::sort(modelCounts.begin(), modelCounts.end());
            QString str;
            for (int i = 0 ; i < modelCounts.size() ; i++)
            {
                str.append(QString("%1").arg(modelCounts.at(i)));
                if (i < modelCounts.size() - 1)
                {
                    str.append(";");
                }
            }

            int count = 0;
            this->recursiveCountHierachicalRank(count, item);

            qDebug() << this->getTabsForHierachicalRank(count) << itemItem << ((modelCounts.size() > 0)?QString(" *** %1").arg(str):"");
        }

        item->visitAttributes([this, &allPoss, &selectedPoss](const CT_OutAbstractItemAttributeModel* itemAtt) -> bool {
            QList<QString> list3;

            // le nom du modèle de sortie
            QString itemItemAtt = itemAtt->displayableName();
            list3.append(itemItemAtt);

            if (allPoss.contains(itemAtt))
            {
                QList<int> modelCounts = selectedPoss.values(itemAtt);
                std::sort(modelCounts.begin(), modelCounts.end());
                QString str;
                for (int i = 0 ; i < modelCounts.size() ; i++)
                {
                    str.append(QString("%1").arg(modelCounts.at(i)));
                    if (i < modelCounts.size() - 1)
                    {
                        str.append(";");
                    }
                }

                int count = 0;
                this->recursiveCountHierachicalRank(count, itemAtt);

                qDebug() << this->getTabsForHierachicalRank(count) << itemItemAtt << ((modelCounts.size() > 0)?QString(" *** %1").arg(str):"");
            }

            //itemItem->appendRow(list3);

            return true;
        });

        //rootItem->appendRow(list2);

        return true;
    });

    //root->appendRow(list);
}

void CDM_StepListInfoManager::recursiveCountHierachicalRank(int &count, const CT_AbstractModel* model)
{
    if (model->parentModel() != nullptr)
    {
        count++;
        recursiveCountHierachicalRank(count, model->parentModel());
    }
}

QString CDM_StepListInfoManager::getTabsForHierachicalRank(int count)
{
    QString str;
    for (int i = 0 ; i < count ; i++)
    {
        str.append("  ");
    }
    return str;
}


QString CDM_StepListInfoManager::getPluginAndStepCitations()
{
    QList<CT_AbstractStepPlugin*> pluginDone;

    QString str;
    QString str2;

    str.append("<h3>" + tr("Citation de la plateforme Computree") + "</h3>");
    str.append("<p>" + tr("Pour citer la plateforme Computree :") + "</p>");
    str.append("<p class=\"descBlock\">");
    str.append(CT_ParseRIS::parseRIS(getComputreeCoreRis()));
    str2.append("</p>");
    str.append("<br><br>");
    str.append("<h3>" + tr("Citation des plugins") + "</h3>");
    str.append("<p>" + tr("Pour citer les plugins utilisés :") + "</p>");

    for (const CDM_StepListInfoManager::StepInfo& stepInfo : qAsConst(_stepInfoList))
    {
        const CT_VirtualAbstractStep *currentStep = stepInfo._step;

        CT_AbstractStepPlugin* plugin = currentStep->pluginStaticCastT<>();

        if (!pluginDone.contains(plugin))
        {
            pluginDone.append(plugin);

            if (!_corePluginList.contains(stepInfo._pluginName)) // core plugins don't need to be cite individually
            {
                str.append(tr("Plugin %1 :<br>").arg(stepInfo._pluginName));
                QStringList pluginRIS = plugin->getPluginRISCitationList();
                if (pluginRIS.isEmpty())
                {
                    str.append(tr("<em>No official citation was provided</em><br>"));
                } else {
                    for (int pp = 0 ; pp < pluginRIS.size() ; pp++)
                    {
                        str.append("<p class=\"descBlock\">");
                        str.append(CT_ParseRIS::parseRIS(pluginRIS.at(pp)));
                        str.append("</p>");
                    }
                }
                str.append("<br>");
            }
        }

        QStringList stepsRis = currentStep->getStepRISCitations();
        if (stepsRis.size() > 0)
        {
            str2.append(tr("Etape <b>%1</b> <em>(plugin %2)</em> : <br>").arg(stepInfo._stepDescription, stepInfo._pluginName));
        }

        for (int i = 0 ; i < stepsRis.size() ; i++)
        {
            str2.append("<p class=\"descBlock\">");
            str2.append(CT_ParseRIS::parseRIS(stepsRis.at(i)));
            str2.append("</p>");
            str2.append("<br>");
        }
    }

    if (!str2.isEmpty())
    {
        str.append("<h3>" + tr("Citations spécifiques pour les étapes") + "</h3>");
        str.append("<p>" + tr("Ci-dessous, la liste des étapes fournissant des références bibliographiques spécifiques (une même référence peut être liée à plusieurs étapes) :") + "</p>");
        str.append(str2);
    }


    return str;
}

bool CDM_StepListInfoManager::hasStepCitation()
{
    for (const CDM_StepListInfoManager::StepInfo& stepInfo : qAsConst(_stepInfoList))
    {
        const CT_VirtualAbstractStep *currentStep = stepInfo._step;

        QStringList stepsRis = currentStep->getStepRISCitations();
        if (stepsRis.size() > 0) {return true;}
    }
    return false;
}

QString CDM_StepListInfoManager::getPluginRIS()
{
    QString str;

    str.append(getComputreeCoreRis());

    QMapIterator<QString, CT_AbstractStepPlugin*> it(_pluginList);
    while (it.hasNext())
    {
        it.next();
        if (!_corePluginList.contains(it.key()))
        {
            QStringList citationList = it.value()->getPluginRISCitationList();
            for (int pp = 0 ; pp < citationList.size() ; pp++)
            {
                str.append(citationList.at(pp));
            }
        }
    }

    QList<QString> stepCitations;

    for (const CDM_StepListInfoManager::StepInfo& stepInfo : qAsConst(_stepInfoList))
    {
        const CT_VirtualAbstractStep *currentStep = stepInfo._step;

        QStringList stepsRis = currentStep->getStepRISCitations();
        for (int i = 0 ; i < stepsRis.size() ; i++)
        {
            QString ris = stepsRis.at(i);
            if (!stepCitations.contains(ris))
            {
                stepCitations.append(ris);
            }
        }
    }

    for (int i = 0 ; i < stepCitations.size() ; i++)
    {
        str.append(stepCitations.at(i));
    }

    return str;
}

QString CDM_StepListInfoManager::getUsedPlugins()
{
    QString str;

    QMapIterator<QString, CT_AbstractStepPlugin*> it(_pluginList);
    while (it.hasNext())
    {
        it.next();
        QString pluginName = it.key();
        CT_AbstractStepPlugin* plugin = it.value();

        str.append(tr("<strong>Plugin %1</strong> : <a href=\"%2\" target=\"_blank\" rel=\"noreferrer noopener\">%2</a><br><br>").arg(pluginName, plugin->getPluginURL()));
    }

    return str;
}

QString CDM_StepListInfoManager::getPluginListToCite()
{
    QString str;

    QMapIterator<QString, CT_AbstractStepPlugin*> it(_pluginList);
    while (it.hasNext())
    {
        it.next();
        QString pluginName = it.key();
        if (!_corePluginList.contains(pluginName))
        {
            if (str.isEmpty())
            {
                str.append(pluginName);
            } else {
                str.append(", " + pluginName);
            }
        }
    }

    return str;
}


QString CDM_StepListInfoManager::getComputreeCoreRis()
{
    QString computreeCitationRIS = "TY  - COMP\n"
                            "TI  - Computree platform\n"
                            "AU  - Computree Core Team\n"
                            "PB  - Computree group\n"
                            "PY  - 2022\n"
                            "UR  - https://computree.onf.fr\n"
                            "ER  - \n";

    return computreeCitationRIS;
}


void CDM_StepListInfoManager::generateHTMLDocForAllSteps(QString baseDirectory, QString cssRelativeDirectory, QString parentPage)
{
    for (const CDM_StepListInfoManager::StepInfo& stepInfo : qAsConst(_stepInfoList))
    {
        generateHTMLDoc(stepInfo, baseDirectory, cssRelativeDirectory, parentPage);
    }
}

QString CDM_StepListInfoManager::getStepIndex(int num, QString parentPage)
{
    QString index;

    index.append(QString("<a href=\"%1\" class=\"tooltip\">Sommaire</a>&nbsp;&nbsp;").arg(parentPage));

    for (const CDM_StepListInfoManager::StepInfo& stepInfo : qAsConst(_stepInfoList))
    {
        QString outFilename = QString("../steps/%1.html").arg(stepInfo._helpFileName);

        QString className = "tooltip";
        if (stepInfo._num == num)
        {
            className = "tooltipactive";
        }

        index.append(QString("<a href=\"%1\" class=\"" + className + "\">%2<div class=\"tooltiptext\">%3</div></a>&nbsp;").arg(outFilename).arg(stepInfo._num).arg(getScriptStepListToolTip(QList<int>() << stepInfo._num)));
    }

    return index;
}


QString CDM_StepListInfoManager::generateHTMLDoc(CDM_StepListInfoManager::StepInfo stepInfo, QString baseDirectory, QString cssRelativeDirectory, QString parentPage)
{
    QString outFilename = QString("%1/content/steps/%2.html").arg(baseDirectory, stepInfo._helpFileName);
    QString pluginURL = stepInfo._step->plugin()->pluginToolForStep()->url();

    QFile f(outFilename);
    if (f.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream stream(&f);
        stream << "<!DOCTYPE html>\n";
        stream << "<html>\n";
        stream << "<head>\n";
        stream << "<metacharset=\"utf-8\">\n";
        stream << "<title>Computree Documentation</title>";
        stream << "<link rel=\"stylesheet\" href=\"" << cssRelativeDirectory << "/style.css\" />";
        stream << "</head>\n";
        stream << "<body>";
        stream << "<div class=\"mainBlock\">";

        stream << "<h2>" << tr("Index des étapes") << "</h2>\n";
        stream << getStepIndex(stepInfo._num, parentPage) << "\n";
        stream << "<br>";
        stream << "<br>";
        stream << "<hr />";

        stream << "<h1>" << stepInfo._num << ". " << stepInfo._stepDescription << "</h1>\n";

        stream << "<Strong>" << tr("Plugin : </Strong><a href=\"") << pluginURL << "\" target=\"_blank\" rel=\"noreferrer noopener\">"<< stepInfo._pluginName << "</a>\n";
        stream << "<br>";
        stream << "<Strong>" << tr("Nom de classe : </strong>") << stepInfo._stepName << "\n";
        stream << "<br><br>";

        stream << "<section>\n";
        stream << "<h2>" << tr("Description") << "</h2>\n";
        stream << "<div class=\"descBlock\">";
        stream << stepInfo._stepDetailledDescription << "\n";
        stream << "</div>";
        stream << "</section>\n";

        QString parameters = stepInfo._step->parametersDescription(true);
        if (!parameters.isEmpty())
        {
            stream << "<section>\n";
            stream << "<h2>" << tr("Paramètres") << "</h2>\n";
            stream << "<div class=\"descBlock\">";
            stream << parameters;
            stream << "</div>";
            stream << "</section>\n";
        }

        // Create screen captures for IN models config.
        QStringList exportedFiles;
        stepInfo._step->exportViewForINModelConfig(QString("%1/content/images/%2.png").arg(baseDirectory, stepInfo._helpFileName), exportedFiles);

        if (!exportedFiles.isEmpty())
        {
            stream << "<section>\n";
            stream << "<h2>" << tr("Configuration des résultats d'entrée") << "</h2>\n";
            stream << "<div class=\"descBlock\">";

            for (const QString &file : qAsConst(exportedFiles))
            {
                stream << "<img src=" << QString("../images/%1").arg(file) << " />\n";
                stream << "<br>\n";
            }

            stream << "</div>";
            stream << "</section>\n";
        }

        QString outputResults = stepInfo._step->outputDescription();
        if (!outputResults.isEmpty())
        {
            stream << "<section>\n";
            stream << "<h2>" << tr("Données de sortie") << "</h2>\n";
            stream << "<div class=\"descBlock\">";
            stream << outputResults;
            stream << "</div>";
            stream << "</section>\n";
        }

        QString details = stepInfo._step->detailsDescription();
        if (!details.isEmpty())
        {
            stream << "<section>\n";
            stream << "<h2>" << tr("Détails") << "</h2>\n";
            stream << "<div class=\"descBlock\">";
            stream << details;
            stream << "</div>";
            stream << "</section>\n";
        }

        QStringList references = stepInfo._step->getStepRISCitations();
        if (!references.isEmpty())
        {
            stream << "<section>\n";
            stream << "<h2>" << tr("Références") << "</h2>\n";
            stream << "<div class=\"descBlock\">";
            for (int i = 0 ; i < references.size() ; i++)
            {
                stream << CT_ParseRIS::parseRIS(references.at(i));
                stream << "<br>";
            }
            stream << "</div>";
            stream << "</section>\n";
        }

        stream << "<br>";
        stream << "<br>";
        stream << "<br>";
        stream << "</div>\n";
        stream << "</body>\n";
        stream << "</html>";
        f.close();
    }

    return outFilename;
}

