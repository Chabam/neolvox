#ifndef CT_READERSTOOLS_H
#define CT_READERSTOOLS_H

#include "ct_global/ct_context.h"
#include "ct_reader/abstract/ct_abstractreader.h"
#include "ct_reader/ct_standardreaderseparator.h"
#include "ct_abstractstepplugin.h"

class CT_ReadersTools {
public:

    /**
     * @brief For all plugins loaded, get the list of readers and add all readers that
     *        pass your filter to your collection.
     * @param cToAdd : collection where to add all readers that pass your filter
     * @param canBeAdded : a function that return true if the reader can be added to the collection.
     */
    template<class Collection>
    static void initAvailableReaders(Collection& cToAdd,
                                     std::function<bool(const CT_AbstractReader*)> canBeAdded) {
        // get the plugin manager
        const PluginManagerInterface* pm = CT_Context::staticInstance()->pluginManager();
        const int nPlugins = pm->countPluginLoaded();

        // for each plugin
        for(int pluginIndex=0; pluginIndex<nPlugins; ++pluginIndex)
        {
            const CT_AbstractStepPlugin* p = pm->getPlugin(pluginIndex);

            const QList<CT_StandardReaderSeparator*> rsl = p->getReadersAvailable();

            for(const CT_StandardReaderSeparator* sep : rsl) {
                const QList<CT_AbstractReader*> readers = sep->readers();

                for(const CT_AbstractReader* reader : readers) {
                    if((canBeAdded == nullptr) || canBeAdded(reader)) {
                        CT_AbstractReader *readerCpy = reader->copyFull();
                        //readerCpy->init(false);

                        cToAdd.append(readerCpy);
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
    static QString constructStringForFileDialog(const CT_AbstractReader* reader) {
        if(reader != nullptr) {

            QString formatText;

            for(const FileFormat& fileFormat : reader->readableFormats()) {
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

    /**
     * @brief Construct the string list to use in a QDirIterator per example to filter files that was compatible with the reader
     * @param reader : the reader to use to construct the string list
     * @return Returns the string list with all formats returned by the reader or an empty list if the reader is nullptr or no formats is available.
     */
    static QStringList constructStringListToFilterFiles(const CT_AbstractReader* reader) {

        QStringList formats;

        if(reader != nullptr) {
            for(const FileFormat& fileFormat : reader->readableFormats()) {
                const QList<QString> &suffixes = fileFormat.suffixes();
                const int nSuffixes = suffixes.size();

                for(int i=0; i<nSuffixes; ++i)
                {
                    formats.append("*." + suffixes.at(i));
                }
            }
        }
        return formats;
    }

    /**
     * @brief Construct a list with classname of all readers in the collection
     * @param readers : the collection of readers
     * @return Returns the list with classname of all readers in the collection
     */
    template<class Collection>
    static QStringList constructReadersUniqueNameList(const Collection& readers) {

        QStringList ret;

        for(const CT_AbstractReader* reader : readers) {
            ret.append(reader->uniqueName());
        }

        if (ret.isEmpty())
            ret.append(QObject::tr("ERREUR : aucun reader disponible"));

        return ret;
    }
};

#endif // CT_READERSTOOLS_H
