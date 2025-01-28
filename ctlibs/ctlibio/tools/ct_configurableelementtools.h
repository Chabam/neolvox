#ifndef CT_CONFIGURABLEELEMENTTOOLS_H
#define CT_CONFIGURABLEELEMENTTOOLS_H

#include "ct_global/ct_context.h"
#include "ct_element/abstract/ct_abstractconfigurableelement.h"
#include <QDebug>

/**
 * @brief Class tools for CT_AbstractConfigurableElement with static methods to disable duplicated code
 */
class CT_ConfigurableElementTools
{
public:

    /**
     * @brief Search an element with the specified unique name in a collection that contains
     *        pointer to CT_AbstractConfigurableElement.
     * @param begin, end : range of elements to examine
     * @param uniqueName : unique name to search
     * @return If founded the configurable element with the sepcified unique name, otherwise nullptr
     */
    template<class InIt>
    static CT_AbstractConfigurableElement* findAConfigurableElementByUniqueNameInCollection(InIt begin, InIt end, const QString& uniqueName) {
        InIt current = std::find_if(begin, end,
                                   [&uniqueName](CT_AbstractConfigurableElement* el)->bool {
                                       return el->getUniqueName() == uniqueName;
                                   });

        if(current != end)
            return *current;

        return nullptr;
    }

    /**
     * @brief Save settings of a collection that contains pointer to CT_AbstractConfigurableElement
     * @param c : the collection
     * @param caller : the caller to use for the writer
     * @param elementName : name to save to describe the element (per example "Metric" if it was metric, etc...)
     * @param writer : the writer to use
     */
    template<class Collection>
    static void saveSettingsOfACollectionOfConfigurableElement(const Collection& c, const QObject* caller, const QString& elementName, SettingsWriterInterface& writer) {
        for(CT_AbstractConfigurableElement* element : c) {
            writer.addParameter(caller, elementName, element->getUniqueName());
            element->saveSettings(writer);
        }
    }

    /**
     * @brief Restore settings of CT_AbstractConfigurableElement and add a copy to the specified collection
     * @param cToAdd : the collection where to add the copy of the restored configurable element
     * @param cToSearch : the collection where to search configurable element with same unique name to copy to be able to restore his setting
     * @param caller : the caller to use for the reader
     * @param elementName : name that describe the element when it was saved (per example "Metric" if it was metric, etc...)
     * @param reader : the reader to use
     */
    template<class Collection>
    static bool restoreSettingsOfConfigurableElementAndSaveItInACollection(Collection& cToAdd, const Collection& cToSearch, const QObject* caller, const QString& elementName, SettingsReaderInterface& reader) {
        qDeleteAll(cToAdd.begin(), cToAdd.end());
        cToAdd.clear();

        QVariant value;

        const int nElements = reader.parameterCount(caller, elementName);

        for(int i=0; i<nElements; ++i) {
            if(reader.parameter(caller, elementName, value)) {
                CT_AbstractConfigurableElement* elementFound = findAConfigurableElementByUniqueNameInCollection(cToSearch.begin(), cToSearch.end(), value.toString());

                if(elementFound == NULL)
                    return false;

                elementFound = elementFound->copy();

                if(elementFound->restoreSettings(reader))
                    cToAdd.append(elementFound);
                else {
                    delete elementFound;
                    return false;
                }
            }
        }

        return true;
    }

    /**
     * @brief For all plugins loaded, get the list of configurable elements that you want and add all elements that
     *        pass your filter to your collection.
     * @param cToAdd : collection where to add all elements that pass your filter
     * @param getCollection : a function or a lambda expression that return the list of configurable elements to loop over
     * @param canBeAdded : a function that return true if the configurable element can be added to the collection.
     */
    template<class Collection, class PluginCollection>
    static void initAvailableConfigurableElements(Collection& cToAdd,
                                                  std::function<PluginCollection(const CT_AbstractStepPlugin*)> getCollection,
                                                  std::function<bool(const CT_AbstractConfigurableElement*)> canBeAdded) {

        if(getCollection == nullptr)
            return;

        const PluginManagerInterface* pm = CT_Context::staticInstance()->pluginManager();

        const int nPlugins = pm->countPluginLoaded();

        for(int pluginIndex=0; pluginIndex<nPlugins; ++pluginIndex)
        {
            const CT_AbstractStepPlugin* p = pm->getPlugin(pluginIndex);
            const PluginCollection elements = getCollection(p);

            for(CT_AbstractConfigurableElement* element : elements) {
                if((canBeAdded == nullptr) || canBeAdded(element))
                    cToAdd.append(element);
            }
        }
    }

    /**
     * @brief Format a string to be representative of the detailled description of a step that use a collection of configurable elements
     * @param c : the collection of configurable elements
     * @return Returns the detailled description in html format
     */
    template<class Collection>
    static QString formatHtmlStepDetailledDescription(const Collection& c) {
        QString ret;

        QListIterator<CT_AbstractConfigurableElement*> it(c);

        for(const CT_AbstractConfigurableElement* element : c) {

            ret += QObject::tr("<h3>%2</h3>"
                               "<div class=\"descBlocklvl2\">"
                               "<strong>Plugin</strong> : %5, <strong>Nom de classe</strong> : %1<br><br>"
                               "<strong>Description</strong>"
                               "<br><br>"
                               "%3"
                               "<br><br>"
                               "%6%4"
                               "</div>")
                    .arg(element->getUniqueName()) //%1
                    .arg(element->getShortDisplayableName()) //%2
                    .arg(element->getShortDescription()) //%3
                    .arg(element->getDetailledDescription()) //%4
                    .arg(element->pluginOfficialName()) //%5
                    .arg(element->getDetailledDescription().isEmpty()?"":QObject::tr("<strong>Détails</strong><br><br>")); //%6

            if(it.hasNext())
                ret += QObject::tr("<br>");
        }

        return ret;
    }
};

#endif // CT_CONFIGURABLEELEMENTTOOLS_H
