#include "cdm_tools.h"

#include "ct_abstractstepplugin.h"
#include "ct_exporter/ct_standardexporterseparator.h"

CDM_Tools::CDM_Tools(const CDM_PluginManager *pm)
{
    m_pm = (CDM_PluginManager*)pm;
}

QMenu* CDM_Tools::createMenuForAllExporters(const QObject *receiver, const char *slot) const
{
    QMenu *menu = new QMenu();

    int s = m_pm->countPluginLoaded();

    for(int i=0; i<s; ++i)
    {
        CT_AbstractStepPlugin *plugin = m_pm->getPlugin(i);
        QList<CT_StandardExporterSeparator*> separators = plugin->getExportersAvailable();

        if(!separators.isEmpty())
        {
            QString pluginName = m_pm->getPluginName(i);
            if (pluginName.left(5) == "plug_")
            {
                pluginName.remove(0, 5);
            }

            QMenu *plugMenu = new QMenu(pluginName, menu);

            QListIterator<CT_StandardExporterSeparator*> itS(separators);

            while(itS.hasNext())
            {
                CT_StandardExporterSeparator *sep = itS.next();

                int eSize = sep->exporters().size();

                if(eSize > 0)
                {
                    int nAdded = 0;

                    QMenu *sepMenu = nullptr;

                    if(eSize > 1)
                        sepMenu = new QMenu(sep->title(), plugMenu);

                    QListIterator<CT_AbstractExporter*> itE(sep->exporters());

                    while(itE.hasNext())
                    {
                        CT_AbstractExporter *exp = itE.next();

                        QString name = createNameForExporter(exp);

                        if(!name.isEmpty())
                        {
                            QAction *expAction = new QAction(name, exp);

                            QObject::connect(expAction, SIGNAL(triggered()), receiver, slot);

                            if(sepMenu != nullptr)
                                sepMenu->addAction(expAction);
                            else
                                plugMenu->addAction(expAction);

                            ++nAdded;
                        }
                    }

                    if(sepMenu != nullptr)
                    {
                        if(nAdded == 1)
                        {
                            QAction *act = sepMenu->actions().at(0);
                            sepMenu->removeAction(act);
                            plugMenu->addAction(act);

                            delete sepMenu;
                            sepMenu = nullptr;
                        }
                        else if(sepMenu->isEmpty())
                        {
                            delete sepMenu;
                            sepMenu = nullptr;
                        }

                        if(sepMenu != nullptr)
                            plugMenu->addMenu(sepMenu);
                    }
                }
            }

            if(plugMenu->isEmpty())
                delete plugMenu;
            else
                menu->addMenu(plugMenu);
        }
    }

    if(menu->isEmpty())
    {
        delete menu;
        menu = nullptr;
    }

    return menu;
}

QString CDM_Tools::createFileExtensionForExporter(CT_AbstractExporter *exporter) const
{
    QString fileExtension;

    fileExtension += QObject::tr("All Valid Files (");

    // VALID FILES
    QSet<QString> uniqueFormats;

    QListIterator<FileFormat> itFormat(exporter->exportFormats());

    if(itFormat.hasNext())
    {
        const FileFormat &format = itFormat.next();

        QListIterator<QString> it(format.suffixes());

        while(it.hasNext())
        {
            const QString &f = it.next();

            if(!uniqueFormats.contains(f)) {
                uniqueFormats.insert(f);
                fileExtension += "*." + f;
            }
        }
    }

    fileExtension += ");;";

    return fileExtension;
}

QString CDM_Tools::createNameForExporter(CT_AbstractExporter *exporter) const
{
//    QString ret;

//    int n = 0;

//    QListIterator<FileFormat> it(exporter->exportFormats());

//    while(it.hasNext())
//    {
//        const FileFormat &format = it.next();

//        ret += "/";

//        QListIterator<QString> itS(format.suffixes());

//        if((n>0) && (itS.hasNext()))
//            ret += "/";

//        while(itS.hasNext())
//        {
//            ret += itS.next();

//            if(itS.hasNext())
//                ret += "/";
//        }

//        ++n;
//    }

    return exporter->displayableName();
}
