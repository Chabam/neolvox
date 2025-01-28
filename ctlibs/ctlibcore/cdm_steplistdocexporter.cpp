#include "cdm_steplistdocexporter.h"

#include "ct_step/tools/menu/ct_menulevel.h"

#include <QDir>

CDM_StepListDocExporter::CDM_StepListDocExporter(CDM_StepManager *stepManager, CDM_PluginManager *pluginManager, QString svnRevision) : QObject(nullptr)
{
    _svnRevision = svnRevision;
    _stepManager = stepManager;
    _pluginManager = pluginManager;

    _stepListInfo = new CDM_StepListInfoManager(_stepManager, _pluginManager);
}

CDM_StepListDocExporter::~CDM_StepListDocExporter()
{
    delete _stepListInfo;
}


void CDM_StepListDocExporter::exportDocumentedScript(QDir& mainDir, QString outDirPath, DocumentatedScriptInfo docInfo)
{
    // export RIS information in subir
    QString RISPath = QString("%1/%2.ris").arg(outDirPath, docInfo._scriptFileName);
    QFile exportRISFile(RISPath);
    if (exportRISFile.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream stream(&exportRISFile);
        stream << _stepListInfo->getPluginRIS();
        exportRISFile.close();
    }


    // Create sub-folders
    QDir outDir(outDirPath);

    outDir.mkdir("content");
    outDir.mkdir("content/steps");
    outDir.mkdir("content/css");
    outDir.mkdir("content/js");
    outDir.mkdir("content/text");
    outDir.mkdir("content/images");
    outDir.mkdir("content/pages");


    QDateTime date = QDateTime::currentDateTime();
    QString formattedTime = date.toString("dd/MM/yyyy hh:mm");

    // export css file
    createCSS(outDirPath + "/content/css/style.css");
    createJSForIframe(outDirPath + "/content/js");


    // create index html file
    QFile findex(outDirPath + "/" + docInfo._scriptFileName + ".html");
    if (findex.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream stream(&findex);

        stream << "<!DOCTYPE html>\n";
        stream << "<html>\n";
        stream << "<head>\n";
        stream << "<metacharset=\"utf-8\">\n";
        stream << "<title>Computree Script Documentation</title><link rel=\"stylesheet\" href=\"content/css/style.css\" /></head>\n";
        stream << "<body>\n";
        stream << "<div class=\"mainBlock\" style = \"display:flex; flex-direction:row; overflow:hidden; min-height:100vh;\">\n";
        stream << "<iframe id=\"frameSummary\" name=\"frameSummary\" src=\"content/pages/summary.html\"  style=\"width:15%; border:none; flex-grow:1;\"></iframe>\n";
        stream << "<iframe id=\"frameContent\" name=\"frameContent\" src=\"content/pages/mainContent.html\" style=\"width:85%; border:none; flex-grow:1;\"></iframe>\n";
        stream << "</div>\n";
        stream << "</body>\n";
        stream << "</html>\n";

        findex.close();
    }

    QString textContent;
    textContent.append("<h1>" + docInfo._scriptName + "</h1>\n");
    createTextContent(outDirPath, "01_title", textContent);

    textContent.clear();
    textContent.append("<p>" + docInfo._description + "</p>\n");
    createTextContent(outDirPath, "02_description", textContent);

    textContent.clear();
    textContent.append("<p><em>" + tr("Ce script a été créé le ") + "</em>" + formattedTime + ", " + tr("avec la version 6.0.") + _svnRevision + tr(" de Computree") + "</p>\n");
    textContent.append("<p><em>" + tr("Auteur : ") + "</em><strong>" + docInfo._author + "</strong></p>\n");
    textContent.append("<p><em>" + tr("Fichier script Computree : ") + "</em><strong>" + docInfo._scriptFileName + ".cts</strong></p>\n");
    createTextContent(outDirPath, "03_version", textContent);

    // Create mainContent.html file
    QFile fMainContent(outDirPath + "/content/pages/mainContent.html");
    if (fMainContent.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream stream(&fMainContent);

        stream << "<!DOCTYPE html>\n";
        stream << "<html>\n";
        stream << "<head>\n";
        stream << "<metacharset=\"utf-8\">\n";
        stream << "<title>Computree Script Documentation</title><link rel=\"stylesheet\" href=\"../css/style.css\" /></head>\n";
        stream << "<script src=\"../js/iframeResizer_parent.js\"></script>\n";
        stream << "<body>\n";
        stream << "<div class=\"mainBlock\">\n";

        stream << "<iframe id=\"title01\" class=\"insertedhtml\" src=\"../text/01_title.html\"></iframe>\n";
        stream << "<script>iFrameResize({ log: true }, '#title01')</script>\n";

        stream << "<h2>" << tr("Description") << "</h2>\n";
        if (!docInfo._description.isEmpty())
        {
            stream << "<iframe id=\"description02\" class=\"insertedhtml\" src=\"../text/02_description.html\"></iframe>\n";
            stream << "<script>iFrameResize({ log: true }, '#description02')</script>\n";
        }

        stream << "<br>\n";
        stream << "<br>\n";

        stream << "<h2>" << tr("Version") << "</h2>\n";
        if (!docInfo._author.isEmpty())
        {
            stream << "<iframe id=\"version03\" class=\"insertedhtml\" src=\"../text/03_version.html\"></iframe>\n";
            stream << "<script>iFrameResize({ log: true }, '#version03')</script>\n";
        }

        stream << "<br>\n";
        stream << "<br>\n";

        stream << "<h2>" << tr("Plugins utilisés") << "</h2>\n";
        stream << "<p>" << tr("Ce script utilise les plugins Computree suivants :") << "</p>\n";
        stream << "<div class=\"linksummary01\">\n";
        stream << _stepListInfo->getUsedPlugins();
        stream << "</div>\n";

        stream << "<br>\n";

        stream << "</body>\n";
        stream << "</html>\n";

        fMainContent.close();
    }


    // Create structure.html file
    QFile fStructure(outDirPath + "/content/pages/use.html");
    if (fStructure.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream stream(&fStructure);

        stream << "<!DOCTYPE html>\n";
        stream << "<html>\n";
        stream << "<head>\n";
        stream << "<metacharset=\"utf-8\">\n";
        stream << "<title>Computree Script Documentation</title><link rel=\"stylesheet\" href=\"../css/style.css\" /></head>\n";
        stream << "<body>\n";
        stream << "<div class=\"mainBlock\">\n";
        stream << "<h1>" << tr("Structure du script") << "</h1>\n";
        stream << "<br>\n";
        stream << "<p>" << tr("Le script est composé de l'enchaînement d'étapes suivant :") << "</p>\n";
        stream << "<br>";
        stream << _stepListInfo->getScriptStepList();
        stream << "<br>";
        stream << "</div>\n";
        stream << "</body>\n";
        stream << "</html>\n";

        fStructure.close();
    }

    // create step help pages
    _stepListInfo->generateHTMLDocForAllSteps(outDirPath, "../css", "../pages/configuration.html");


    // Create parameters.html file
    QFile fParametersContent(outDirPath + "/content/pages/configuration.html");
    if (fParametersContent.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream stream(&fParametersContent);

        stream << "<!DOCTYPE html>\n";
        stream << "<html>\n";
        stream << "<head>\n";
        stream << "<metacharset=\"utf-8\">\n";
        stream << "<title>Computree Script Documentation</title><link rel=\"stylesheet\" href=\"../css/style.css\" /></head>\n";
        stream << "<body>\n";
        stream << "<div class=\"mainBlock\">\n";

        stream << "<h1>" << tr("Index des étapes") << "</h1>\n";
        stream << "<br>\n";
        stream << _stepListInfo->getScriptClickableStepList("../steps") << "\n";
        stream << "<br>\n";

        stream << "</div>\n";
        stream << "</body>\n";
        stream << "</html>\n";

        fParametersContent.close();
    }


    // Create citation.html file
    QFile fcitations(outDirPath + "/content/pages/citations.html");
    if (fcitations.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream stream(&fcitations);

        stream << "<!DOCTYPE html>\n";
        stream << "<html>\n";
        stream << "<head>\n";
        stream << "<metacharset=\"utf-8\">\n";
        stream << "<title>Computree Script Citation</title><link rel=\"stylesheet\" href=\"../css/style.css\" /></head>\n";
        stream << "<body>\n";
        stream << "<div class=\"mainBlock\">\n";
        stream << "<h1>" << tr("Comment citer ce script") << "</h1>\n";
        stream << "<br>\n";
        stream << "<p>" << tr("Pour référencer les travaux utilisés dans ce script, il faut citer :<br>") << "</p>\n";
        stream << "<ul>\n";
        stream << "<li>" + tr("La plateforme Computree") + "</li>\n";
        stream << "<li>" + tr("Les plugins utilisés : <strong>%1</strong>").arg(_stepListInfo->getPluginListToCite()) + "</li>\n";
        if (_stepListInfo->hasStepCitation())
        {
            stream << "<li>" + tr("Les citations spécifiques liées à certaines étapes utilisées") + "</li>\n";
        }
        stream << "</ul>\n";
        stream << "<p>" << tr("L'ensemble des références bibliographiques fournies dans cette page, sont disponibles dans le fichier <strong>%1.ris</strong> (au format bibliographique standard RIS).").arg(docInfo._scriptFileName) << "</p>\n";
        stream << "<br>\n";
        stream << _stepListInfo->getPluginAndStepCitations();
        stream << "</div>\n";
        stream << "</body>\n";
        stream << "</html>\n";

        fcitations.close();
    }

    // Create menu step list
    QMap<QString, CT_VirtualAbstractStep*> stepList;

    CT_StepsMenu *menu = _pluginManager->stepsMenu();
    QList<CT_MenuLevel*> levels = menu->levels();
    QListIterator<CT_MenuLevel*> it(levels);

    while(it.hasNext())
    {
        CT_MenuLevel* level = it.next();
        const QString favoritesName = CT_StepsMenu::staticOperationToDisplayableName(CT_StepsMenu::LO_Favorites);

        if(level->displayableName() != favoritesName)
        {
            const QList<CT_VirtualAbstractStep*> steps = level->steps();

            for(CT_VirtualAbstractStep* step : steps)
            {
                stepList.insert(step->name(), step);
            }

            QList<CT_MenuLevel*> sublevels = level->levels();
            QListIterator<CT_MenuLevel*> its(sublevels);
            while(its.hasNext())
            {
                CT_MenuLevel* subLevel = its.next();
                const QList<CT_VirtualAbstractStep*> steps = subLevel->steps();

                for(CT_VirtualAbstractStep* step : steps)
                {
                    stepList.insert(step->name(), step);
                }
            }
        }
    }


    // Create Summmary and steps documentation pages
    QFile f(outDirPath + "/content/pages/summary.html");

    if (f.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream stream(&f);
        stream << "<!DOCTYPE html>\n";
        stream << "<html>\n";
        stream << "<head>\n";
        stream << "<metacharset=\"utf-8\">\n";
        stream << "<title>Documentation Summary</title>\n";
        stream << "<link rel=\"stylesheet\" href=\"../css/style.css\"/>\n";
        stream << "</head>\n";
        stream << "<body>\n";
        stream << "<div class=\"mainBlock\">\n";

        stream << "<h1>" << tr("Index") << "</h1>\n";

        stream << "<div class=\"linksummary01\" >\n";
        stream << "<a target=\"frameContent\" href=\"mainContent.html\">" + tr("Informations générales") + "<br><br></a>\n";
        stream << "<a target=\"frameContent\" href=\"use.html\">" + tr("Utilisation") + "<br><br></a>\n";
        stream << "<a target=\"frameContent\" href=\"configuration.html\">" + tr("Configuration détaillée") + "<br><br></a>\n";
        stream << "<a target=\"frameContent\" href=\"citations.html\">" + tr("Références") + "<br><br></a>\n";
        stream << "</div>\n";

        stream << "</body>\n";
        stream << "</html>\n";
        f.close();
    }
}





void CDM_StepListDocExporter::createStepHelp(QString exportDir, QProgressDialog &progressDialog)
{
    // Create documentation createStepHelpessary and clean doc_xx/steps content
    QDir dir(exportDir);
    if (!dir.exists()) {dir.mkpath(".");}

    QDir dir2(exportDir + "/steps");
    if (!dir2.exists()) {
        dir2.mkpath(".");
    } else {
        dir2.setNameFilters(QStringList() << "*.*");
        dir2.setFilter(QDir::Files);
        foreach(QString dirFile, dir2.entryList())
        {
            dir2.remove(dirFile);
        }
    }

    QDir dir3(exportDir + "/css");
    if (!dir3.exists()) {dir3.mkpath(".");}


    // Create index.html
    QFile findex(exportDir + "/index.html");
    if (findex.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream stream(&findex);

        stream << "<!DOCTYPE html>\n";
        stream << "<html>\n";
        stream << "<head>\n";
        stream << "<metacharset=\"utf-8\">\n";
        stream << "<title>Computree Documentation</title><link rel=\"stylesheet\" href=\"css/style.css\" /></head>\n";
        stream << "<body>\n";
        stream << "<div class=\"mainBlock\" style = \"display:flex; flex-direction:row; overflow:hidden; min-height:100vh;\">\n";
        stream << "<iframe id=\"frameSummary\" name=\"frameSummary\" src=\"summary.html\"  style=\"width:33%; border:none; flex-grow:1;\"></iframe>\n";
        stream << "<iframe id=\"frameContent\" name=\"frameContent\" src=\"steps/current.html\" style=\"width:66%; border:none; flex-grow:1;\"></iframe>\n";
        stream << "</div>\n";
        stream << "</body>\n";
        stream << "</html>\n";

        findex.close();
    }

    // Create index_default.html
    QFile findexdefault(exportDir + "/index_default.html");
    if (findexdefault.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream stream(&findexdefault);

        stream << "<!DOCTYPE html>\n";
        stream << "<html>\n";
        stream << "<head>\n";
        stream << "<metacharset=\"utf-8\">\n";
        stream << "<title>Computree Documentation</title><link rel=\"stylesheet\" href=\"css/style.css\" /></head>\n";
        stream << "<body>\n";
        stream << "<div class=\"mainBlock\">\n";
        stream << "<h1>" << tr("Documentation des étapes Computree") << "</h1>\n";
        stream << "<p>" << tr("Utiliser l'index sur la gauche pour séléctionner l'étape pour laquelle afficher la documentation. ") << "</p>\n";
        stream << "<br>";
        stream << "<p>" << "<a href=\"http://computree.onf.fr\"target=\"_blank\" rel=\"noopener noreferrer\"><strong>" << tr("Site internet officiel de Computree") << "</strong></a></p>\n";
        stream << "</div>\n";
        stream << "</body>\n";
        stream << "</html>\n";

        findexdefault.close();
    }

    QFile::copy(exportDir + "/index_default.html", exportDir + "/steps/current.html");


    // Create style.css
    createCSS(exportDir + "/css/style.css");

    // Create Summmary and steps documentation pages
    QFile f(exportDir + "/summary.html");
    QFile fst(exportDir + "/status.txt");

    if (f.open(QFile::WriteOnly | QFile::Text) && fst.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream streamFst(&fst);
        streamFst << "Step\tDescription\tDetailledDescription\tParameters\tInput\tOutput\tDetails\tReferences\n";

        QTextStream stream(&f);
        stream << "<!DOCTYPE html>\n";
        stream << "<html>\n";
        stream << "<head>\n";
        stream << "<metacharset=\"utf-8\">\n";
        stream << "<title>Documentation Summary</title>";
        stream << "<link rel=\"stylesheet\" href=\"css/style.css\" />";
        stream << "</head>\n";
        stream << "<body>";
        stream << "<div class=\"mainBlock\">";
        stream << "<h1>" << tr("Index des étapes") << "</h1>\n";

        CT_StepsMenu *menu = _pluginManager->stepsMenu();
        QList<CT_MenuLevel*> levels = menu->levels();
        QListIterator<CT_MenuLevel*> it(levels);

        progressDialog.setMaximum(levels.size());

        int cpt = 0;
        while(it.hasNext())
        {
            CT_MenuLevel* level = it.next();
            const QString favoritesName = CT_StepsMenu::staticOperationToDisplayableName(CT_StepsMenu::LO_Favorites);

            if(level->displayableName() != favoritesName)
            {
                stream << "<details>";
                stream << "<summary><span class=\"h1summary\">" << level->displayableName() << "</span></summary>";

                const QList<CT_VirtualAbstractStep*> steps = level->steps();

                if (steps.size() > 0) {stream << "<div class=\"linksummary01\" >";}
                for(CT_VirtualAbstractStep* step : steps)
                {
                    stream << "<a target=\"frameContent\" href=\"steps/" << step->name() << ".html\">" << step->description() << "<br></a>\n";

                    // Create documentation page for this step
                    step->generateHTMLDocumentation(exportDir + "/steps", "../css");

                    streamFst << step->name() << "\t";
                    streamFst << step->description() << "\t";
                    if (step->detailledDescription() == "No detailled description for this step") {streamFst << "0\t";}
                    else {streamFst << (step->detailledDescription().size()) << "\t";}
                    streamFst << (step->parametersDescription().isEmpty()?"0":"1") << "\t";
                    streamFst << (step->inputDescription().isEmpty()?"0":"1") << "\t";
                    streamFst << (step->outputDescription().isEmpty()?"0":"1") << "\t";
                    streamFst << (step->detailsDescription().isEmpty()?"0":"1") << "\t";
                    streamFst << (step->getStepRISCitations().isEmpty()?"0":"1") << "\n";
                }
                if (steps.size() > 0) {stream << "</div>";}

                QList<CT_MenuLevel*> sublevels = level->levels();
                QListIterator<CT_MenuLevel*> its(sublevels);
                while(its.hasNext())
                {
                    CT_MenuLevel* subLevel = its.next();
                    stream << "<details>";
                    stream << "<summary><span class=\"h2summary\">" << subLevel->displayableName() << "</span></summary>";

                    const QList<CT_VirtualAbstractStep*> steps = subLevel->steps();

                    if (steps.size() > 0) {stream << "<div class=\"linksummary02\" >";}
                    for(CT_VirtualAbstractStep* step : steps)
                    {
                        stream << "<a target=\"frameContent\" href=\"steps/" << step->name() << ".html\">" << step->description() << "<br></a>\n";

                        // Create documentation page for this step
                        step->generateHTMLDocumentation(exportDir + "/steps", "../css");

                        streamFst << step->name() << "\t";
                        streamFst << step->description() << "\t";
                        if (step->detailledDescription() == "No detailled description for this step") {streamFst << "0\t";}
                        else {streamFst << (step->detailledDescription().size()) << "\t";}
                        streamFst << (step->parametersDescription().isEmpty()?"0":"1") << "\t";
                        streamFst << (step->inputDescription().isEmpty()?"0":"1") << "\t";
                        streamFst << (step->outputDescription().isEmpty()?"0":"1") << "\t";
                        streamFst << (step->detailsDescription().isEmpty()?"0":"1") << "\t";
                        streamFst << (step->getStepRISCitations().isEmpty()?"0":"1") << "\n";
                    }
                    if (steps.size() > 0) {stream << "</div>";}
                    stream << "</details>\n";
                }
                stream << "</details>\n";
            }
            progressDialog.setValue(++cpt);
        }
        stream << "</div>\n";
        stream << "</body>\n";
        stream << "</html>";
        f.close();
        fst.close();
    }
}





void CDM_StepListDocExporter::createCSS(QString filename)
{
    QFile fcss(filename);
    if (fcss.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream stream(&fcss);

        stream << ".mainBlock\n";
        stream << "{\n";
        stream << "    display: block;\n";
        stream << "    margin-left: auto;\n";
        stream << "    margin-right: auto;\n";
        stream << "    padding-left: 1.25rem;\n";
        stream << "    padding-right: 1.25rem;\n";
        stream << "}\n";
        stream << "\n";
        stream << "h1\n";
        stream << "{\n";
        stream << "    color:darkred;\n";
        stream << "}\n";
        stream << "h2\n";
        stream << "{\n";
        stream << "    color:black;\n";
        stream << "}\n";
        stream << "h3\n";
        stream << "{\n";
        stream << "    color:darkblue;\n";
        stream << "}\n";
        stream << ".parameterDescr\n";
        stream << "{\n";
        stream << "    color:darkblue;\n";
        stream << "}\n";
        stream << ".resultInDescr\n";
        stream << "{\n";
        stream << "    color:darkblue;\n";
        stream << "}\n";
        stream << ".resultOutDescr\n";
        stream << "{\n";
        stream << "    color:darkblue;\n";
        stream << "}\n";
        stream << ".descBlock\n";
        stream << "{\n";
        stream << "    margin-left:10px;\n";
        stream << "}\n";
        stream << ".descBlocklvl2\n";
        stream << "{\n";
        stream << "    margin-left:30px;\n";
        stream << "}\n";
        stream << "summary {\n";
        stream << "  display: flex;\n";
        stream << "  cursor: pointer;\n";
        stream << "}\n";
        stream << "summary::marker\n";
        stream << "{\n";
        stream << "  display: none;\n";
        stream << "}\n";
        stream << ".h1summary\n";
        stream << "{\n";
        stream << "    color:darkred;\n";
        stream << "}\n";
        stream << ".h2summary\n";
        stream << "{\n";
        stream << "    font-weight: 700;\n";
        stream << "    margin-left: 20px;\n";
        stream << "}\n";
        stream << ".linksummary01\n";
        stream << "{\n";
        stream << "    margin-left: 20px;\n";
        stream << "}\n";
        stream << ".linksummary02\n";
        stream << "{\n";
        stream << "    margin-left: 40px;\n";
        stream << "}\n";
        stream << ".insertedhtml\n";
        stream << "{\n";
        stream << "    width: 1px;\n";
        stream << "    min-width: 100%;\n";
        stream << "    margin: 0;\n";
        stream << "    padding: 0;\n";
        stream << "    border: 0 none;\n";
        stream << "}\n";
        stream << "body\n";
        stream << "{\n";
        stream << "    margin: 0;\n";
        stream << "    padding: 0;\n";
        stream << "}\n";
        stream << "\n";

        stream << "/* Style pour la balise <a> */\n";
        stream << ".tooltip\n";
        stream << "{\n";
        stream << "    position: relative;\n";
        stream << "    display: inline-block;\n";
        stream << "    cursor: pointer;\n";
        stream << "    color: #333; /* Couleur du texte */\n";
        stream << "    text-decoration: none; /* Supprimer le soulignement du lien */\n";
        stream << "    background-color: #f0f0f0; /* Fond gris clair */\n";
        stream << "    border: 1px solid #4d4d4d; /* Bordure gris très foncé */\n";
        stream << "    padding: 6px 12px; /* Espacement autour du texte */\n";
        stream << "    border-radius: 4px; /* Coins légèrement arrondis */\n";
        stream << "    font-size: 14px; /* Taille de police standard */\n";
        stream << "    transition: background-color 0.3s ease; /* Transition douce pour l'arrière-plan */\n";
        stream << "}\n";
        stream << ".tooltip:hover\n";
        stream << "{\n";
        stream << "    background-color: #e0e0e0; /* Légèrement plus foncé au survol */\n";
        stream << "}\n";
        stream << "\n";

        stream << "/* Style pour la balise active <a> */\n";
        stream << ".tooltipactive\n";
        stream << "{\n";
        stream << "    position: relative;\n";
        stream << "    display: inline-block;\n";
        stream << "    cursor: pointer;\n";
        stream << "    color: #333; /* Couleur du texte */\n";
        stream << "    text-decoration: none; /* Supprimer le soulignement du lien */\n";
        stream << "    background-color: #b4e5a2; /* Fond vert pas trop clair */\n";
        stream << "    border: 1px solid #4d4d4d; /* Bordure gris très foncé */\n";
        stream << "    padding: 6px 12px; /* Espacement autour du texte */\n";
        stream << "    border-radius: 4px; /* Coins légèrement arrondis */\n";
        stream << "    font-size: 14px; /* Taille de police standard */\n";
        stream << "    font-weight: bold; /* Texte en gras */\n";
        stream << "    transition: background-color 0.3s ease; /* Transition douce pour l'arrière-plan */\n";
        stream << "}\n";
        stream << ".tooltipactive:hover\n";
        stream << "{\n";
        stream << "    background-color: #adde9b; /* Légèrement plus foncé au survol */\n";
        stream << "}\n";
        stream << "\n";

        stream << "/* Style pour le tooltip */\n";
        stream << ".tooltip .tooltiptext, .tooltipactive .tooltiptext\n";
        stream << "{\n";
        stream << "    visibility: hidden;\n";
        stream << "    width: 500px;\n";
        stream << "    background-color: #f0f0f0;\n";
        stream << "    color: #000000;\n";
        stream << "    text-align: left;\n";
        stream << "    border-radius: 4px;\n";
        stream << "    padding: 8px;\n";
        stream << "    position: absolute;\n";
        stream << "    z-index: 1;\n";
        stream << "    top: 100%; /* Positionne en dessous du lien */\n";
        stream << "    left: 50%;\n";
        stream << "    margin-top: 8px;\n";
        stream << "    opacity: 0;\n";
        stream << "    transition: opacity 0.3s;\n";
        stream << "    /* Permet d'afficher du contenu HTML dans le tooltip */\n";
        stream << "    white-space: normal;\n";
        stream << "    overflow-wrap: break-word; /* Gérer les très longues lignes */\n";
        stream << "    box-sizing: border-box;\n";
        stream << "    border: 1px solid #ccc;\n";
        stream << "    box-shadow: 0px 4px 8px rgba(0, 0, 0, 0.1); /* Ombre légère */\n";
        stream << "}\n";
        stream << "\n";

        stream << "/* Flèche du tooltip */\n";
        stream << ".tooltip .tooltiptext::after, .tooltipactive .tooltiptext::after\n";
        stream << "{\n";
        stream << "    content: '';\n";
        stream << "    position: absolute;\n";
        stream << "    bottom: 100%;\n";
        stream << "    left: 50%;\n";
        stream << "    margin-left: -5px;\n";
        stream << "    border-width: 5px;\n";
        stream << "    border-style: solid;\n";
        stream << "    border-color: transparent transparent #333 transparent;\n";
        stream << "}\n";
        stream << "\n";

        stream << "/* Affiche le tooltip au survol */\n";
        stream << ".tooltip:hover .tooltiptext, .tooltipactive:hover .tooltiptext\n";
        stream << "{\n";
        stream << "    visibility: visible;\n";
        stream << "    opacity: 1;\n";
        stream << "}\n";
        stream << "\n";

        fcss.close();
    }
}

void CDM_StepListDocExporter::createJSForIframe(QString dir)
{
    QFile fjs1(QString("%1/%2").arg(dir).arg("iframeResizer_parent.js"));
    if (fjs1.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream stream(&fjs1);

        stream << "/*! iFrame Resizer (iframeSizer.min.js ) - v4.3.9 - 2023-11-10\n";
        stream << " *  Desc: Force cross domain iframes to size to content.\n";
        stream << " *  Requires: iframeResizer.contentWindow.min.js to be loaded into the target frame.\n";
        stream << " *  Copyright: (c) 2023 David J. Bradshaw - dave@bradshaw.net\n";
        stream << " *  License: MIT\n";
        stream << " */\n";
        stream << "!function(d){var c,u,a,v,x,I,M,r,f,k,i,l,z;function m(){return window.MutationObserver||window.WebKitMutationObserver||window.MozMutationObserver}function F(e,n,i){e.addEventListener(n,i,!1)}function B(e,n,i){e.removeEventListener(n,i,!1)}function p(e){return x+\"[\"+(n=\"Host page: \"+(e=e),n=window.top!==window.self?window.parentIFrame&&window.parentIFrame.getId?window.parentIFrame.getId()+\": \"+e:\"Nested host page: \"+e:n)+\"]\";var n}function t(e){return k[e]?k[e].log:u}function O(e,n){o(\"log\",e,n,t(e))}function E(e,n){o(\"info\",e,n,t(e))}function R(e,n){o(\"warn\",e,n,!0)}function o(e,n,i,t){!0===t&&\"object\"==typeof window.console&&console[e](p(n),i)}function w(e){function i(){t(\"Height\"),t(\"Width\"),P(function(){H(w),C(b),l(\"onResized\",w)},w,\"init\")}function n(){var e=p.slice(I).split(\":\"),n=e[1]?parseInt(e[1],10):0,i=k[e[0]]&&k[e[0]].iframe,t=getComputedStyle(i);return{iframe:i,id:e[0],height:n+function(e){if(\"border-box\"!==e.boxSizing)return 0;var n=e.paddingTop?parseInt(e.paddingTop,10):0,e=e.paddingBottom?parseInt(e.paddingBottom,10):0;return n+e}(t)+function(e){if(\"border-box\"!==e.boxSizing)return 0;var n=e.borderTopWidth?parseInt(e.borderTopWidth,10):0,e=e.borderBottomWidth?parseInt(e.borderBottomWidth,10):0;return n+e}(t),width:e[2],type:e[3]}}function t(e){var n=Number(k[b][\"max\"+e]),i=Number(k[b][\"min\"+e]),e=e.toLowerCase(),t=Number(w[e]);O(b,\"Checking \"+e+\" is in range \"+i+\"-\"+n),t<i&&(t=i,O(b,\"Set \"+e+\" to min value\")),n<t&&(t=n,O(b,\"Set \"+e+\" to max value\")),w[e]=\"\"+t}function o(){var t=e.origin,o=k[b]&&k[b].checkOrigin;if(o&&\"\"+t!=\"null\"&&!function(){if(o.constructor!==Array)return e=k[b]&&k[b].remoteHost,O(b,\"Checking connection is from: \"+e),t===e;var e,n=0,i=!1;for(O(b,\"Checking connection is from allowed list of origins: \"+o);n<o.length;n++)if(o[n]===t){i=!0;break}return i}())throw new Error(\"Unexpected message received from: \"+t+\" for \"+w.iframe.id+\". Message was: \"+e.data+\". This error can be disabled by setting the checkOrigin: false option or by providing of array of trusted domains.\");return 1}function a(e){return p.slice(p.indexOf(\":\")+v+e)}function s(i,t){var e,n,o;e=function(){var e,n;A(\"Send Page Info\",\"pageInfo:\"+(e=document.body.getBoundingClientRect(),n=w.iframe.getBoundingClientRect(),JSON.stringify({iframeHeight:n.height,iframeWidth:n.width,clientHeight:Math.max(document.documentElement.clientHeight,window.innerHeight||0),clientWidth:Math.max(document.documentElement.clientWidth,window.innerWidth||0),offsetTop:parseInt(n.top-e.top,10),offsetLeft:parseInt(n.left-e.left,10),scrollTop:window.pageYOffset,scrollLeft:window.pageXOffset,documentHeight:document.documentElement.clientHeight,documentWidth:document.documentElement.clientWidth,windowHeight:window.innerHeight,windowWidth:window.innerWidth})),i,t)},n=32,z[o=t]||(z[o]=setTimeout(function(){z[o]=null,e()},n))}function r(e){e=e.getBoundingClientRect();return W(b),{x:Math.floor(Number(e.left)+Number(M.x)),y:Math.floor(Number(e.top)+Number(M.y))}}function d(e){var n=e?r(w.iframe):{x:0,y:0},i={x:Number(w.width)+n.x,y:Number(w.height)+n.y};O(b,\"Reposition requested from iFrame (offset x:\"+n.x+\" y:\"+n.y+\")\"),window.top===window.self?(M=i,c(),O(b,\"--\")):window.parentIFrame?window.parentIFrame[\"scrollTo\"+(e?\"Offset\":\"\")](i.x,i.y):R(b,\"Unable to scroll to requested position, window.parentIFrame not found\")}function c(){!1===l(\"onScroll\",M)?S():C(b)}function u(e){var e=e.split(\"#\")[1]||\"\",n=decodeURIComponent(e),n=document.getElementById(n)||document.getElementsByName(n)[0];n?(n=r(n),O(b,\"Moving to in page link (#\"+e+\") at x: \"+n.x+\" y: \"+n.y),M={x:n.x,y:n.y},c(),O(b,\"--\")):window.top===window.self?O(b,\"In page link #\"+e+\" not found\"):window.parentIFrame?window.parentIFrame.moveToAnchor(e):O(b,\"In page link #\"+e+\" not found and window.parentIFrame not found\")}function f(e){var n,i={};i=0===Number(w.width)&&0===Number(w.height)?{x:(n=a(9).split(\":\"))[1],y:n[0]}:{x:w.width,y:w.height},l(e,{iframe:w.iframe,screenX:Number(i.x),screenY:Number(i.y),type:w.type})}function l(e,n){return T(b,e,n)}function m(){switch(k[b]&&k[b].firstRun&&k[b]&&(k[b].firstRun=!1),w.type){case\"close\":N(w.iframe);break;case\"message\":n=a(6),O(b,\"onMessage passed: {iframe: \"+w.iframe.id+\", message: \"+n+\"}\"),l(\"onMessage\",{iframe:w.iframe,message:JSON.parse(n)}),O(b,\"--\");break;case\"mouseenter\":f(\"onMouseEnter\");break;case\"mouseleave\":f(\"onMouseLeave\");break;case\"autoResize\":k[b].autoResize=JSON.parse(a(9));break;case\"scrollTo\":d(!1);break;case\"scrollToOffset\":d(!0);break;case\"pageInfo\":s(k[b]&&k[b].iframe,b),r=b,e(\"Add \",F),k[r]&&(k[r].stopPageInfo=o);break;case\"pageInfoStop\":k[b]&&k[b].stopPageInfo&&(k[b].stopPageInfo(),delete k[b].stopPageInfo);break;case\"inPageLink\":u(a(9));break;case\"reset\":j(w);break;case\"init\":i(),l(\"onInit\",w.iframe);break;default:0===Number(w.width)&&0===Number(w.height)?R(\"Unsupported message received (\"+w.type+\"), this is likely due to the iframe containing a later version of iframe-resizer than the parent page\"):i()}function e(n,i){function t(){k[r]?s(k[r].iframe,r):o()}[\"scroll\",\"resize\"].forEach(function(e){O(r,n+e+\" listener for sendPageInfo\"),i(window,e,t)})}function o(){e(\"Remove \",B)}var r,n}var g,h,p=e.data,w={},b=null;if(\"[iFrameResizerChild]Ready\"===p)for(var y in k)A(\"iFrame requested init\",L(y),k[y].iframe,y);else x===(\"\"+p).slice(0,I)&&p.slice(I).split(\":\")[0]in k?(w=n(),b=w.id,k[b]&&(k[b].loaded=!0),(h=w.type in{true:1,false:1,undefined:1})&&O(b,\"Ignoring init message from meta parent page\"),!h&&(h=!0,k[g=b]||(h=!1,R(w.type+\" No settings for \"+g+\". Message was: \"+p)),h)&&(O(b,\"Received: \"+p),g=!0,null===w.iframe&&(R(b,\"IFrame (\"+w.id+\") not found\"),g=!1),g&&o()&&m())):E(b,\"Ignored: \"+p)}function T(e,n,i){var t=null,o=null;if(k[e]){if(\"function\"!=typeof(t=k[e][n]))throw new TypeError(n+\" on iFrame[\"+e+\"] is not a function\");o=t(i)}return o}function g(e){e=e.id;delete k[e]}function N(e){var n=e.id;if(!1===T(n,\"onClose\",n))O(n,\"Close iframe cancelled by onClose event\");else{O(n,\"Removing iFrame: \"+n);try{e.parentNode&&e.parentNode.removeChild(e)}catch(e){R(e)}T(n,\"onClosed\",n),O(n,\"--\"),g(e)}}function W(e){null===M&&O(e,\"Get page position: \"+(M={x:window.pageXOffset===d?document.documentElement.scrollLeft:window.pageXOffset,y:window.pageYOffset===d?document.documentElement.scrollTop:window.pageYOffset}).x+\",\"+M.y)}function C(e){null!==M&&(window.scrollTo(M.x,M.y),O(e,\"Set page position: \"+M.x+\",\"+M.y),S())}function S(){M=null}function j(e){O(e.id,\"Size reset requested by \"+(\"init\"===e.type?\"host page\":\"iFrame\")),W(e.id),P(function(){H(e),A(\"reset\",\"reset\",e.iframe,e.id)},e,\"reset\")}function H(o){function i(e){var n;function i(){Object.keys(k).forEach(function(e){function n(e){return\"0px\"===(k[i]&&k[i].iframe.style[e])}var i;k[i=e]&&null!==k[i].iframe.offsetParent&&(n(\"height\")||n(\"width\"))&&A(\"Visibility change\",\"resize\",k[i].iframe,i)})}function t(e){O(\"window\",\"Mutation observed: \"+e[0].target+\" \"+e[0].type),h(i,16)}!a&&\"0\"===o[e]&&(a=!0,O(r,\"Hidden iFrame detected, creating visibility listener\"),e=m())&&(n=document.querySelector(\"body\"),new e(t).observe(n,{attributes:!0,attributeOldValue:!1,characterData:!0,characterDataOldValue:!1,childList:!0,subtree:!0}))}function e(e){var n;n=e,o.id?(o.iframe.style[n]=o[n]+\"px\",O(o.id,\"IFrame (\"+r+\") \"+n+\" set to \"+o[n]+\"px\")):O(\"undefined\",\"messageData id not set\"),i(e)}var r=o.iframe.id;k[r]&&(k[r].sizeHeight&&e(\"height\"),k[r].sizeWidth)&&e(\"width\")}function P(e,n,i){i!==n.type&&r&&!window.jasmine?(O(n.id,\"Requesting animation frame\"),r(e)):e()}function A(n,i,t,o,e){function r(){var e;t&&\"contentWindow\"in t&&null!==t.contentWindow?(e=k[o]&&k[o].targetOrigin,O(o,\"[\"+n+\"] Sending msg to iframe[\"+o+\"] (\"+i+\") targetOrigin: \"+e),t.contentWindow.postMessage(x+i,e)):R(o,\"[\"+n+\"] IFrame(\"+o+\") not found\")}function a(){e&&k[o]&&k[o].warningTimeout&&(k[o].msgTimeout=setTimeout(function(){!k[o]||k[o].loaded||s||(s=!0,R(o,\"IFrame has not responded within \"+k[o].warningTimeout/1e3+\" seconds. Check iFrameResizer.contentWindow.js has been loaded in iFrame. This message can be ignored if everything is working, or you can set the warningTimeout option to a higher value or zero to suppress this warning.\"))},k[o].warningTimeout))}var s=!1;o=o||t.id,k[o]&&(r(),a())}function L(e){return e+\":\"+k[e].bodyMarginV1+\":\"+k[e].sizeWidth+\":\"+k[e].log+\":\"+k[e].interval+\":\"+k[e].enablePublicMethods+\":\"+k[e].autoResize+\":\"+k[e].bodyMargin+\":\"+k[e].heightCalculationMethod+\":\"+k[e].bodyBackground+\":\"+k[e].bodyPadding+\":\"+k[e].tolerance+\":\"+k[e].inPageLinks+\":\"+k[e].resizeFrom+\":\"+k[e].widthCalculationMethod+\":\"+k[e].mouseEvents}function s(t,i){function e(i){var e=m();e&&(e=e,t.parentNode)&&new e(function(e){e.forEach(function(e){Array.prototype.slice.call(e.removedNodes).forEach(function(e){e===t&&N(t)})})}).observe(t.parentNode,{childList:!0}),F(t,\"load\",function(){var e,n;A(\"iFrame.onload\",i,t,d,!0),e=k[r]&&k[r].firstRun,n=k[r]&&k[r].heightCalculationMethod in f,!e&&n&&j({iframe:t,height:0,width:0,type:\"init\"})}),A(\"init\",i,t,d,!0)}function o(e){var n=e.split(\"Callback\");2===n.length&&(this[n=\"on\"+n[0].charAt(0).toUpperCase()+n[0].slice(1)]=this[e],delete this[e],R(r,\"Deprecated: '\"+e+\"' has been renamed '\"+n+\"'. The old method will be removed in the next major version.\"))}function n(e){if(e=e||{},k[r]=Object.create(null),k[r].iframe=t,k[r].firstRun=!0,k[r].remoteHost=t.src&&t.src.split(\"/\").slice(0,3).join(\"/\"),\"object\"!=typeof e)throw new TypeError(\"Options is not an object\");Object.keys(e).forEach(o,e);var n,i=e;for(n in l)Object.prototype.hasOwnProperty.call(l,n)&&(k[r][n]=(Object.prototype.hasOwnProperty.call(i,n)?i:l)[n]);k[r]&&(k[r].targetOrigin=!0!==k[r].checkOrigin||\"\"===(e=k[r].remoteHost)||null!==e.match(/^(about:blank|javascript:|file:\\/\\/)/)?\"*\":e)}var r=function(e){if(\"string\"!=typeof e)throw new TypeError(\"Invaild id for iFrame. Expected String\");var n;return\"\"===e&&(t.id=(n=i&&i.id||l.id+c++,null!==document.getElementById(n)&&(n+=c++),e=n),u=(i||{}).log,O(e,\"Added missing iframe ID: \"+e+\" (\"+t.src+\")\")),e}(t.id);if(r in k&&\"iFrameResizer\"in t)R(r,\"Ignored iFrame, already setup.\");else{switch(n(i),O(r,\"IFrame scrolling \"+(k[r]&&k[r].scrolling?\"enabled\":\"disabled\")+\" for \"+r),t.style.overflow=!1===(k[r]&&k[r].scrolling)?\"hidden\":\"auto\",k[r]&&k[r].scrolling){case\"omit\":break;case!0:t.scrolling=\"yes\";break;case!1:t.scrolling=\"no\";break;default:t.scrolling=k[r]?k[r].scrolling:\"no\"}s(\"Height\"),s(\"Width\"),a(\"maxHeight\"),a(\"minHeight\"),a(\"maxWidth\"),a(\"minWidth\"),\"number\"!=typeof(k[r]&&k[r].bodyMargin)&&\"0\"!==(k[r]&&k[r].bodyMargin)||(k[r].bodyMarginV1=k[r].bodyMargin,k[r].bodyMargin=k[r].bodyMargin+\"px\"),e(L(r)),k[r]&&(k[r].iframe.iFrameResizer={close:N.bind(null,k[r].iframe),removeListeners:g.bind(null,k[r].iframe),resize:A.bind(null,\"Window resize\",\"resize\",k[r].iframe),moveToAnchor:function(e){A(\"Move to anchor\",\"moveToAnchor:\"+e,k[r].iframe,r)},sendMessage:function(e){A(\"Send Message\",\"message:\"+(e=JSON.stringify(e)),k[r].iframe,r)}})}function a(e){var n=k[r][e];1/0!==n&&0!==n&&(t.style[e]=\"number\"==typeof n?n+\"px\":n,O(r,\"Set \"+e+\" = \"+t.style[e]))}function s(e){if(k[r][\"min\"+e]>k[r][\"max\"+e])throw new Error(\"Value for min\"+e+\" can not be greater than max\"+e)}}function h(e,n){null===i&&(i=setTimeout(function(){i=null,e()},n))}function e(){\"hidden\"!==document.visibilityState&&(O(\"document\",\"Trigger event: Visibility change\"),h(function(){b(\"Tab Visible\",\"resize\")},16))}function b(i,t){Object.keys(k).forEach(function(e){var n;k[n=e]&&\"parent\"===k[n].resizeFrom&&k[n].autoResize&&!k[n].firstRun&&A(i,t,k[e].iframe,e)})}function y(){F(window,\"message\",w),F(window,\"resize\",function(){var e;O(\"window\",\"Trigger event: \"+(e=\"resize\")),h(function(){b(\"Window \"+e,\"resize\")},16)}),F(document,\"visibilitychange\",e),F(document,\"-webkit-visibilitychange\",e)}function n(){function t(e,n){if(n){if(!n.tagName)throw new TypeError(\"Object is not a valid DOM element\");if(\"IFRAME\"!==n.tagName.toUpperCase())throw new TypeError(\"Expected <IFRAME> tag, found <\"+n.tagName+\">\");s(n,e),o.push(n)}}for(var o,e=[\"moz\",\"webkit\",\"o\",\"ms\"],n=0;n<e.length&&!r;n+=1)r=window[e[n]+\"RequestAnimationFrame\"];return r?r=r.bind(window):O(\"setup\",\"RequestAnimationFrame not supported\"),y(),function(e,n){var i;switch(o=[],(i=e)&&i.enablePublicMethods&&R(\"enablePublicMethods option has been removed, public methods are now always available in the iFrame\"),typeof n){case\"undefined\":case\"string\":Array.prototype.forEach.call(document.querySelectorAll(n||\"iframe\"),t.bind(d,e));break;case\"object\":t(e,n);break;default:throw new TypeError(\"Unexpected data type (\"+typeof n+\")\")}return o}}function q(e){e.fn?e.fn.iFrameResize||(e.fn.iFrameResize=function(i){return this.filter(\"iframe\").each(function(e,n){s(n,i)}).end()}):E(\"\",\"Unable to bind to jQuery, it is not fully loaded.\")}\"undefined\"!=typeof window&&(c=0,a=u=!1,v=\"message\".length,I=(x=\"[iFrameSizer]\").length,M=null,r=window.requestAnimationFrame,f=Object.freeze({max:1,scroll:1,bodyScroll:1,documentElementScroll:1}),k={},i=null,l=Object.freeze({autoResize:!0,bodyBackground:null,bodyMargin:null,bodyMarginV1:8,bodyPadding:null,checkOrigin:!0,inPageLinks:!1,enablePublicMethods:!0,heightCalculationMethod:\"bodyOffset\",id:\"iFrameResizer\",interval:32,log:!1,maxHeight:1/0,maxWidth:1/0,minHeight:0,minWidth:0,mouseEvents:!0,resizeFrom:\"parent\",scrolling:!1,sizeHeight:!0,sizeWidth:!1,warningTimeout:5e3,tolerance:0,widthCalculationMethod:\"scroll\",onClose:function(){return!0},onClosed:function(){},onInit:function(){},onMessage:function(){R(\"onMessage function not defined\")},onMouseEnter:function(){},onMouseLeave:function(){},onResized:function(){},onScroll:function(){return!0}}),z={},window.jQuery!==d&&q(window.jQuery),\"function\"==typeof define&&define.amd?define([],n):\"object\"==typeof module&&\"object\"==typeof module.exports&&(module.exports=n()),window.iFrameResize=window.iFrameResize||n())}();\n";
        stream << "//# sourceMappingURL=iframeResizer.map\n";

        fjs1.close();
    }

    QFile fjs2(QString("%1/%2").arg(dir).arg("iframeResizer_child.js"));
    if (fjs2.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream stream(&fjs2);

        stream << "/*! iFrame Resizer (iframeSizer.contentWindow.min.js) - v4.3.9 - 2023-11-10\n";
        stream << " *  Desc: Include this file in any page being loaded into an iframe\n";
        stream << " *        to force the iframe to resize to the content size.\n";
        stream << " *  Requires: iframeResizer.min.js on host page.\n";
        stream << " *  Copyright: (c) 2023 David J. Bradshaw - dave@bradshaw.net\n";
        stream << " *  License: MIT\n";
        stream << " */\n";
        stream << "!function(a){if(\"undefined\"!=typeof window){var r=!0,P=\"\",u=0,c=\"\",s=null,D=\"\",d=!1,j={resize:1,click:1},l=128,q=!0,f=1,n=\"bodyOffset\",m=n,H=!0,W=\"\",h={},g=32,B=null,p=!1,v=!1,y=\"[iFrameSizer]\",J=y.length,w=\"\",U={max:1,min:1,bodyScroll:1,documentElementScroll:1},b=\"child\",V=!0,X=window.parent,T=\"*\",E=0,i=!1,Y=null,O=16,S=1,K=\"scroll\",M=K,Q=window,G=function(){x(\"onMessage function not defined\")},Z=function(){},$=function(){},_={height:function(){return x(\"Custom height calculation function not defined\"),document.documentElement.offsetHeight},width:function(){return x(\"Custom width calculation function not defined\"),document.body.scrollWidth}},ee={},te=!1;try{var ne=Object.create({},{passive:{get:function(){te=!0}}});window.addEventListener(\"test\",ae,ne),window.removeEventListener(\"test\",ae,ne)}catch(e){}var oe,o,I,ie,N,A,C={bodyOffset:function(){return document.body.offsetHeight+ye(\"marginTop\")+ye(\"marginBottom\")},offset:function(){return C.bodyOffset()},bodyScroll:function(){return document.body.scrollHeight},custom:function(){return _.height()},documentElementOffset:function(){return document.documentElement.offsetHeight},documentElementScroll:function(){return document.documentElement.scrollHeight},max:function(){return Math.max.apply(null,e(C))},min:function(){return Math.min.apply(null,e(C))},grow:function(){return C.max()},lowestElement:function(){return Math.max(C.bodyOffset()||C.documentElementOffset(),we(\"bottom\",Te()))},taggedElement:function(){return be(\"bottom\",\"data-iframe-height\")}},z={bodyScroll:function(){return document.body.scrollWidth},bodyOffset:function(){return document.body.offsetWidth},custom:function(){return _.width()},documentElementScroll:function(){return document.documentElement.scrollWidth},documentElementOffset:function(){return document.documentElement.offsetWidth},scroll:function(){return Math.max(z.bodyScroll(),z.documentElementScroll())},max:function(){return Math.max.apply(null,e(z))},min:function(){return Math.min.apply(null,e(z))},rightMostElement:function(){return we(\"right\",Te())},taggedElement:function(){return be(\"right\",\"data-iframe-width\")}},re=(oe=Ee,N=null,A=0,function(){var e=Date.now(),t=O-(e-(A=A||e));return o=this,I=arguments,t<=0||O<t?(N&&(clearTimeout(N),N=null),A=e,ie=oe.apply(o,I),N||(o=I=null)):N=N||setTimeout(Oe,t),ie});k(window,\"message\",function(t){var n={init:function(){W=t.data,X=t.source,se(),q=!1,setTimeout(function(){H=!1},l)},reset:function(){H?R(\"Page reset ignored by init\"):(R(\"Page size reset by host page\"),Me(\"resetPage\"))},resize:function(){L(\"resizeParent\",\"Parent window requested size check\")},moveToAnchor:function(){h.findTarget(i())},inPageLink:function(){this.moveToAnchor()},pageInfo:function(){var e=i();R(\"PageInfoFromParent called from parent: \"+e),$(JSON.parse(e)),R(\" --\")},message:function(){var e=i();R(\"onMessage called from parent: \"+e),G(JSON.parse(e)),R(\" --\")}};function o(){return t.data.split(\"]\")[1].split(\":\")[0]}function i(){return t.data.slice(t.data.indexOf(\":\")+1)}function r(){return t.data.split(\":\")[2]in{true:1,false:1}}function e(){var e=o();e in n?n[e]():(\"undefined\"==typeof module||!module.exports)&&\"iFrameResize\"in window||window.jQuery!==a&&\"iFrameResize\"in window.jQuery.prototype||r()||x(\"Unexpected message (\"+t.data+\")\")}y===(\"\"+t.data).slice(0,J)&&(!1===q?e():r()?n.init():R('Ignored message of type \"'+o()+'\". Received before initialization.'))}),k(window,\"readystatechange\",Ne),Ne()}function ae(){}function k(e,t,n,o){e.addEventListener(t,n,!!te&&(o||{}))}function ue(e){return e.charAt(0).toUpperCase()+e.slice(1)}function ce(e){return y+\"[\"+w+\"] \"+e}function R(e){p&&\"object\"==typeof window.console&&console.log(ce(e))}function x(e){\"object\"==typeof window.console&&console.warn(ce(e))}function se(){function e(e){return\"true\"===e}function t(e,t){return\"function\"==typeof e&&(R(\"Setup custom \"+t+\"CalcMethod\"),_[t]=e,e=\"custom\"),e}{var n;n=W.slice(J).split(\":\"),w=n[0],u=a===n[1]?u:Number(n[1]),d=a===n[2]?d:e(n[2]),p=a===n[3]?p:e(n[3]),g=a===n[4]?g:Number(n[4]),r=a===n[6]?r:e(n[6]),c=n[7],m=a===n[8]?m:n[8],P=n[9],D=n[10],E=a===n[11]?E:Number(n[11]),h.enable=a!==n[12]&&e(n[12]),b=a===n[13]?b:n[13],M=a===n[14]?M:n[14],v=a===n[15]?v:e(n[15]),R(\"Initialising iFrame (\"+window.location.href+\")\"),\"iFrameResizer\"in window&&Object===window.iFrameResizer.constructor&&(n=window.iFrameResizer,R(\"Reading data from page: \"+JSON.stringify(n)),Object.keys(n).forEach(de,n),G=\"onMessage\"in n?n.onMessage:G,Z=\"onReady\"in n?n.onReady:Z,T=\"targetOrigin\"in n?n.targetOrigin:T,m=\"heightCalculationMethod\"in n?n.heightCalculationMethod:m,M=\"widthCalculationMethod\"in n?n.widthCalculationMethod:M,m=t(m,\"height\"),M=t(M,\"width\"))}function o(e){F(0,0,e.type,e.screenY+\":\"+e.screenX)}function i(e,t){R(\"Add event listener: \"+t),k(window.document,e,o)}R(\"TargetOrigin for parent set to: \"+T),le(\"margin\",function(e,t){-1!==t.indexOf(\"-\")&&(x(\"Negative CSS value ignored for \"+e),t=\"\");return t}(\"margin\",c=a===c?u+\"px\":c)),le(\"background\",P),le(\"padding\",D),(n=document.createElement(\"div\")).style.clear=\"both\",n.style.display=\"block\",n.style.height=\"0\",document.body.appendChild(n),he(),ge(),document.documentElement.style.height=\"\",document.body.style.height=\"\",R('HTML & body height set to \"auto\"'),R(\"Enable public methods\"),Q.parentIFrame={autoResize:function(e){return!0===e&&!1===r?(r=!0,pe()):!1===e&&!0===r&&(r=!1,fe(\"remove\"),null!==s&&s.disconnect(),clearInterval(B)),F(0,0,\"autoResize\",JSON.stringify(r)),r},close:function(){F(0,0,\"close\")},getId:function(){return w},getPageInfo:function(e){\"function\"==typeof e?($=e,F(0,0,\"pageInfo\")):($=function(){},F(0,0,\"pageInfoStop\"))},moveToAnchor:function(e){h.findTarget(e)},reset:function(){Ie(\"parentIFrame.reset\")},scrollTo:function(e,t){F(t,e,\"scrollTo\")},scrollToOffset:function(e,t){F(t,e,\"scrollToOffset\")},sendMessage:function(e,t){F(0,0,\"message\",JSON.stringify(e),t)},setHeightCalculationMethod:function(e){m=e,he()},setWidthCalculationMethod:function(e){M=e,ge()},setTargetOrigin:function(e){R(\"Set targetOrigin: \"+e),T=e},size:function(e,t){L(\"size\",\"parentIFrame.size(\"+((e||\"\")+(t?\",\"+t:\"\"))+\")\",e,t)}},!0===v&&(i(\"mouseenter\",\"Mouse Enter\"),i(\"mouseleave\",\"Mouse Leave\")),pe(),h=function(){function n(e){var e=e.getBoundingClientRect(),t={x:window.pageXOffset===a?document.documentElement.scrollLeft:window.pageXOffset,y:window.pageYOffset===a?document.documentElement.scrollTop:window.pageYOffset};return{x:parseInt(e.left,10)+parseInt(t.x,10),y:parseInt(e.top,10)+parseInt(t.y,10)}}function o(e){var e=e.split(\"#\")[1]||e,t=decodeURIComponent(e),t=document.getElementById(t)||document.getElementsByName(t)[0];a===t?(R(\"In page link (#\"+e+\") not found in iFrame, so sending to parent\"),F(0,0,\"inPageLink\",\"#\"+e)):(t=n(t=t),R(\"Moving to in page link (#\"+e+\") at x: \"+t.x+\" y: \"+t.y),F(t.y,t.x,\"scrollToOffset\"))}function e(){var e=window.location.hash,t=window.location.href;\"\"!==e&&\"#\"!==e&&o(t)}function t(){Array.prototype.forEach.call(document.querySelectorAll('a[href^=\"#\"]'),function(e){\"#\"!==e.getAttribute(\"href\")&&k(e,\"click\",function(e){e.preventDefault(),o(this.getAttribute(\"href\"))})})}function i(){Array.prototype.forEach&&document.querySelectorAll?(R(\"Setting up location.hash handlers\"),t(),k(window,\"hashchange\",e),setTimeout(e,l)):x(\"In page linking not fully supported in this browser! (See README.md for IE8 workaround)\")}h.enable?i():R(\"In page linking not enabled\");return{findTarget:o}}(),L(\"init\",\"Init message from host page\"),Z()}function de(e){var t=e.split(\"Callback\");2===t.length&&(this[t=\"on\"+t[0].charAt(0).toUpperCase()+t[0].slice(1)]=this[e],delete this[e],x(\"Deprecated: '\"+e+\"' has been renamed '\"+t+\"'. The old method will be removed in the next major version.\"))}function le(e,t){a!==t&&\"\"!==t&&\"null\"!==t&&R(\"Body \"+e+' set to \"'+(document.body.style[e]=t)+'\"')}function t(n){var e={add:function(e){function t(){L(n.eventName,n.eventType)}ee[e]=t,k(window,e,t,{passive:!0})},remove:function(e){var t=ee[e];delete ee[e],window.removeEventListener(e,t,!1)}};n.eventNames&&Array.prototype.map?(n.eventName=n.eventNames[0],n.eventNames.map(e[n.method])):e[n.method](n.eventName),R(ue(n.method)+\" event listener: \"+n.eventType)}function fe(e){t({method:e,eventType:\"Animation Start\",eventNames:[\"animationstart\",\"webkitAnimationStart\"]}),t({method:e,eventType:\"Animation Iteration\",eventNames:[\"animationiteration\",\"webkitAnimationIteration\"]}),t({method:e,eventType:\"Animation End\",eventNames:[\"animationend\",\"webkitAnimationEnd\"]}),t({method:e,eventType:\"Input\",eventName:\"input\"}),t({method:e,eventType:\"Mouse Up\",eventName:\"mouseup\"}),t({method:e,eventType:\"Mouse Down\",eventName:\"mousedown\"}),t({method:e,eventType:\"Orientation Change\",eventName:\"orientationchange\"}),t({method:e,eventType:\"Print\",eventNames:[\"afterprint\",\"beforeprint\"]}),t({method:e,eventType:\"Ready State Change\",eventName:\"readystatechange\"}),t({method:e,eventType:\"Touch Start\",eventName:\"touchstart\"}),t({method:e,eventType:\"Touch End\",eventName:\"touchend\"}),t({method:e,eventType:\"Touch Cancel\",eventName:\"touchcancel\"}),t({method:e,eventType:\"Transition Start\",eventNames:[\"transitionstart\",\"webkitTransitionStart\",\"MSTransitionStart\",\"oTransitionStart\",\"otransitionstart\"]}),t({method:e,eventType:\"Transition Iteration\",eventNames:[\"transitioniteration\",\"webkitTransitionIteration\",\"MSTransitionIteration\",\"oTransitionIteration\",\"otransitioniteration\"]}),t({method:e,eventType:\"Transition End\",eventNames:[\"transitionend\",\"webkitTransitionEnd\",\"MSTransitionEnd\",\"oTransitionEnd\",\"otransitionend\"]}),\"child\"===b&&t({method:e,eventType:\"IFrame Resized\",eventName:\"resize\"})}function me(e,t,n,o){return t!==e&&(e in n||(x(e+\" is not a valid option for \"+o+\"CalculationMethod.\"),e=t),R(o+' calculation method set to \"'+e+'\"')),e}function he(){m=me(m,n,C,\"height\")}function ge(){M=me(M,K,z,\"width\")}function pe(){var e;!0===r?(fe(\"add\"),e=g<0,window.MutationObserver||window.WebKitMutationObserver?e?ve():s=function(){function t(e){function t(e){!1===e.complete&&(R(\"Attach listeners to \"+e.src),e.addEventListener(\"load\",i,!1),e.addEventListener(\"error\",r,!1),u.push(e))}\"attributes\"===e.type&&\"src\"===e.attributeName?t(e.target):\"childList\"===e.type&&Array.prototype.forEach.call(e.target.querySelectorAll(\"img\"),t)}function o(e){R(\"Remove listeners from \"+e.src),e.removeEventListener(\"load\",i,!1),e.removeEventListener(\"error\",r,!1),u.splice(u.indexOf(e),1)}function n(e,t,n){o(e.target),L(t,n+\": \"+e.target.src)}function i(e){n(e,\"imageLoad\",\"Image loaded\")}function r(e){n(e,\"imageLoadFailed\",\"Image load failed\")}function a(e){L(\"mutationObserver\",\"mutationObserver: \"+e[0].target+\" \"+e[0].type),e.forEach(t)}var u=[],c=window.MutationObserver||window.WebKitMutationObserver,s=function(){var e=document.querySelector(\"body\");return s=new c(a),R(\"Create body MutationObserver\"),s.observe(e,{attributes:!0,attributeOldValue:!1,characterData:!0,characterDataOldValue:!1,childList:!0,subtree:!0}),s}();return{disconnect:function(){\"disconnect\"in s&&(R(\"Disconnect body MutationObserver\"),s.disconnect(),u.forEach(o))}}}():(R(\"MutationObserver not supported in this browser!\"),ve())):R(\"Auto Resize disabled\")}function ve(){0!==g&&(R(\"setInterval: \"+g+\"ms\"),B=setInterval(function(){L(\"interval\",\"setInterval: \"+g)},Math.abs(g)))}function ye(e,t){return t=t||document.body,t=null===(t=document.defaultView.getComputedStyle(t,null))?0:t[e],parseInt(t,10)}function we(e,t){for(var n,o=t.length,i=0,r=ue(e),a=Date.now(),u=0;u<o;u++)i<(n=t[u].getBoundingClientRect()[e]+ye(\"margin\"+r,t[u]))&&(i=n);return a=Date.now()-a,R(\"Parsed \"+o+\" HTML elements\"),R(\"Element position calculated in \"+a+\"ms\"),O/2<(a=a)&&R(\"Event throttle increased to \"+(O=2*a)+\"ms\"),i}function e(e){return[e.bodyOffset(),e.bodyScroll(),e.documentElementOffset(),e.documentElementScroll()]}function be(e,t){var n=document.querySelectorAll(\"[\"+t+\"]\");return 0===n.length&&(x(\"No tagged elements (\"+t+\") found on page\"),document.querySelectorAll(\"body *\")),we(e,n)}function Te(){return document.querySelectorAll(\"body *\")}function Ee(e,t,n,o){function i(){e in{init:1,interval:1,size:1}||!(m in U||d&&M in U)?e in{interval:1}||R(\"No change in size detected\"):Ie(t)}function r(e,t){return!(Math.abs(e-t)<=E)}n=a===n?C[m]():n,o=a===o?z[M]():o,r(f,n)||d&&r(S,o)||\"init\"===e?(Se(),F(f=n,S=o,e)):i()}function Oe(){A=Date.now(),N=null,ie=oe.apply(o,I),N||(o=I=null)}function L(e,t,n,o){i&&e in j?R(\"Trigger event cancelled: \"+e):(e in{reset:1,resetPage:1,init:1}||R(\"Trigger event: \"+t),(\"init\"===e?Ee:re)(e,t,n,o))}function Se(){i||(i=!0,R(\"Trigger event lock on\")),clearTimeout(Y),Y=setTimeout(function(){i=!1,R(\"Trigger event lock off\"),R(\"--\")},l)}function Me(e){f=C[m](),S=z[M](),F(f,S,e)}function Ie(e){var t=m;m=n,R(\"Reset trigger event: \"+e),Se(),Me(\"reset\"),m=t}function F(e,t,n,o,i){!0===V&&(a===i?i=T:R(\"Message targetOrigin: \"+i),R(\"Sending message to host page (\"+(e=w+\":\"+(e+\":\"+t)+\":\"+n+(a===o?\"\":\":\"+o))+\")\"),X.postMessage(y+e,i))}function Ne(){\"loading\"!==document.readyState&&window.parent.postMessage(\"[iFrameResizerChild]Ready\",\"*\")}}();\n";
        stream << "//# sourceMappingURL=iframeResizer.contentWindow.map\n";

        fjs2.close();
    }
}

void CDM_StepListDocExporter::createTextContent(QString outDirPath, QString fileName, QString content)
{
    QFile ftitleText(outDirPath + "/content/text/" + fileName + ".html");
    if (ftitleText.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream stream(&ftitleText);
        stream << "<!DOCTYPE html><html><head><link rel=\"stylesheet\" href=\"../css/style.css\" /></head><body><script src=\"../js/iframeResizer_child.js\"></script>\n";
        stream << content;
        stream << "<br></body></html>\n";
        ftitleText.close();
    }
}
