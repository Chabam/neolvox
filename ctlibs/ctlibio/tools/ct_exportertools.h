#ifndef CT_EXPORTERTOOLS_H
#define CT_EXPORTERTOOLS_H

#include "ct_global/ct_context.h"
#include "ct_exporter/abstract/ct_abstractexporter.h"
#include "ct_exporter/ct_standardexporterseparator.h"
#include "ct_abstractstepplugin.h"

class CT_ExporterTools {
public:
    /**
     * @brief For all plugins loaded, get the list of exporters and add all exporter that
     *        pass your filter to your collection.
     * @param cToAdd : collection where to add all exporters that pass your filter
     * @param canBeAdded : a function that return true if the exporter can be added to the collection.
     */
    template<class Collection>
    static void initAvailableExporters(Collection& cToAdd,
                                       std::function<bool(const CT_AbstractExporter*)> canBeAdded) {
        const PluginManagerInterface* pm = CT_Context::staticInstance()->pluginManager();
        const int nPlugins = pm->countPluginLoaded();

        for(int pluginIndex=0; pluginIndex<nPlugins; ++pluginIndex)
        {
            const CT_AbstractStepPlugin* p = pm->getPlugin(pluginIndex);

            const QList<CT_StandardExporterSeparator*> rsl = p->getExportersAvailable();

            for(const CT_StandardExporterSeparator* sep : rsl) {
                const QList<CT_AbstractExporter*> exporters = sep->exporters();

                for(const CT_AbstractExporter* exporter : exporters) {
                    if((canBeAdded == nullptr) || canBeAdded(exporter)) {
                        CT_AbstractExporter* exporterCpy = exporter->copy();

                        const QList<FileFormat>& formats = exporterCpy->exportFormats();
                        const int nFormats = formats.size();

                        for(int n=0; n<nFormats; ++n)
                        {
                            const FileFormat& format = formats.at(n);

                            QString key = QString("%2 - %1").arg(exporter->uniqueName()).arg(format.description());
                            cToAdd.insert(key, exporterCpy);
                        }
                    }
                }
            }
        }
    }

    /**
     * @brief Construct the string to use in a QFileDialog to choose or save a file
     * @param reader : the reader to use to construct the string
     * @return Returns the string with all formats returned by the reader or an empty string if the reader is nullptr or no formats is available.
     */
    static QString constructStringForFileDialog(const CT_AbstractExporter* exporter) {
        if(exporter != nullptr) {

            QString formatText;

            for(const FileFormat& fileFormat : exporter->exportFormats()) {
                const QList<QString> &suffixes = fileFormat.suffixes();
                const int nSuffixes = suffixes.size();

                if(nSuffixes > 0) {

                    if(!formatText.isEmpty())
                        formatText += ";;";

                    formatText += fileFormat.description();
                    formatText+= " (";

                    for(int i=0; i<nSuffixes; ++i)
                    {
                        if(i != 0)
                            formatText += " ";

                        formatText += "*.";
                        formatText += suffixes.at(i);
                    }

                    formatText += ")";
                }
            }

            return formatText;
        }

        return "";
    }
};

#endif // CT_EXPORTERTOOLS_H
