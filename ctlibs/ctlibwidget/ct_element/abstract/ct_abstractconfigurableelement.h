#ifndef CT_ABSTRACTCONFIGURABLEELEMENT_H
#define CT_ABSTRACTCONFIGURABLEELEMENT_H

#include "ctlibwidget_global.h"

#include "settingsinterfaces.h"
#include "ct_view/ct_abstractconfigurablewidget.h"

#include <QString>

/**
 * @brief You can inherit from this class if you want to be used by in the widget "CTG_ConfigurableElementsSelector" or in
 *        a class that manage this type of object.
 *
 *        A configurable element is just an object that :
 *
 *        - Have a unique name (class name), a displayable name and a description.
 *        - Can be configured or not (returns a configurable widget if yes otherwise nullptr)
 *        - Can save and restore his settings
 *        - Can be copied
 */
class CTLIBWIDGET_EXPORT CT_AbstractConfigurableElement : public QObject
{
    Q_OBJECT

public:
    CT_AbstractConfigurableElement(QString pluginName);

    CT_AbstractConfigurableElement(const CT_AbstractConfigurableElement& other);

    virtual ~CT_AbstractConfigurableElement();

    /**
     * @brief Returns the unique name of this object (class name)
     * @warning This method returns a correct answer only if you add the macro Q_OBJECT in
     *          the beginning of your class that inherit from this class. Otherwise the result
     *          will be the classname of the ancestor class that has this macro declared.
     * @returns Returns the classname
     */
    QString getUniqueName() const;

    /**
     * @brief Returns a short displayable name.
     *        You can redefine it in your class if you want to returns other things
     * @returns Returns a short displayable name. By default returns the result of "getUniqueName".
     */
    virtual QString getShortDisplayableName() const;

    /**
     * @brief Returns a detailled displayable name.
     *        You can redefine it in your class if you want to returns other things
     * @returns Returns a more detailled displayable name. By default returns the result of "getShortDisplayableName".
     */
    virtual QString getDetailledDisplayableName() const;

    /**
     * @brief Returns a short description
     *        You must redefine it in your class to returns a short description for the user
     * @returns Returns a short description.
     */
    virtual QString getShortDescription() const = 0;

    /**
     * @brief Returns a detailled description.
     *        You can redefine it in your class if you want to returns other things
     * @returns Returns a more detailled description. By default returns the result of "getShortDescription".
     */
    virtual QString getDetailledDescription() const;

    /**
     * @brief This method is called when the core will save settings of this object
     *        You can redefine it in your class to save your settings
     */
    virtual void saveSettings(SettingsWriterInterface& writer) const = 0;

    /**
     * @brief This method is called when the core will restore settings of this object
     *        You can redefine it in your class to restore your settings
     * @returns Returns true if all settings was perfectly restored, false if a setting cannot
     *          be restored and this object must be configured manually.
     */
    virtual bool restoreSettings(SettingsReaderInterface& reader) = 0;

    /**
     * @brief Returns a full copy of this object
     *        You must redefine it in your class to returns a full copy of the object
     * @returns Returns an exact (full) copy of this object.
     */
    virtual CT_AbstractConfigurableElement* copy() const = 0;

    /**
     * @brief Create and return a widget that configure this object. By default return nullptr.
     * @return Can be nullptr if nothing must be configured
     */
    virtual CT_AbstractConfigurableWidget* createConfigurationWidget() { return nullptr; }

    /**
     * @brief Called after configuration
     */
    virtual void finalizeConfiguration() = 0;

    /**
     * @brief Return official plugin Name
     */
    QString pluginOfficialName() const {return m_pluginName;}

private:
    QString         m_pluginName;
};

#endif // CT_ABSTRACTCONFIGURABLEELEMENT_H
