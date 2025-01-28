#ifndef SETTINGSINTERFACES_H
#define SETTINGSINTERFACES_H

#include <QObject>
#include <QString>
#include <QVariant>

/**
 * @brief The settings writer is used to save settings of elements in a script. It will use an xml file to write all
 *        parameters that you want to save.
 */
class SettingsWriterInterface
{
public:

    virtual ~SettingsWriterInterface() {}

    /**
     * @brief Set a global description for the caller (the caller is use to set the name of the group)
     *
     *        In the file the result will be like this :
     *
     *        <CALLER_CLASSNAME description="DESCRIPTION">
     *          ...
     *        </CALLER_CLASSNAME>
     *
     * @param caller : always pass "this". The manager use the object to get the classname and set the name of the group.
     * @param description : the description to write in the file for this group
     */
    virtual void setCallerDescription(const QObject* caller, const QString& description) = 0;

    /**
     * @brief Set a unique ID for this caller so you can ensure the compatibility between script when you add/remove a caller
     *
     *        In the file the result will be like this :
     *
     *        <CALLER_CLASSNAME id="UNIQUE_ID">
     *          ...
     *        </CALLER_CLASSNAME>
     * @param caller : always pass "this"
     * @param uniqueID : A positive and unique index if you want to ensure the compatibility between script when you add/remove a caller
     */
    virtual void setCallerUniqueID(const QObject* caller, const int& uniqueID) = 0;

    /**
     * @brief Save a parameter.
     *
     *        In the xml file the result will be like this if it was only one parameter :
     *
     *        <CALLER_CLASSNAME description="DESCRIPTION" id="UNIQUE_ID" PARAMETER_NAME="PARAMETER_VALUE" pDescription="DESC_VALUE" pID="ID_VALUE" />
     *
     *        In the xml file the result will be like this if it was multiple parameters :
     *
     *        <CALLER_CLASSNAME>
     *          <PARAMETER_NAME value="PARAMETER_VALUE" pDescription="DESC_VALUE" pID="ID_VALUE" />
     *          <PARAMETER_NAME value="PARAMETER_VALUE" pDescription="DESC_VALUE" pID="ID_VALUE" />
     *          ...
     *        </CALLER_CLASSNAME>
     *
     *        You can call this method multiple times with the same parameter name. In this situation when the script is readed you must call multiple times
     *        the method "parameter" (until the method return "false").
     *
     * @param caller : always pass "this". The manager use the object to get the classname and set the name of the group.
     * @param parameterName : name of the parameter that you want to save
     * @param parameterValue : value of the parameter to save
     * @param description : a short description to inform the user (that modify a script manually) what this parameter is able to do
     * @param uniqueID : (optionnal)-1 if you don't want to use it. A positive and unique index if you want to ensure the compatibility between script when you add/remove a parameter
     * @return a generated unique and positive ID that represent this parameter and can be used in method "addParameterExtraInfo" per example. 0 or a negative value if it was an error.
     */
    virtual int addParameter(const QObject* caller, const QString& parameterName, const QVariant& parameterValue, const QString& description = "", const int& uniqueID = -1) = 0;

    /**
     * @brief Save a PATH. Use it when you want to save a filepath or a directory path. Same result as the method "addParameter" but
     *        will convert the path to a relative path and add an absolute directory path to another section in the file. So it will simplier for a user to modify path.
     *
     *        In the xml file the result will always be like this if it was one or multiple parameters :
     *
     *        <CALLER_CLASSNAME>
     *          <PARAMETER_NAME path="PARAMETER_VALUE" pDescription="DESC_VALUE" pID="ID_VALUE" />
     *          ...
     *        </CALLER_CLASSNAME>
     *
     *        You can call this method multiple times with the same parameter name. In this situation when the script is readed you must call multiple times
     *        the method "parameterPath" (until the method return "false").
     *
     * @param caller : always pass "this". The manager use the object to get the classname and set the name of the group.
     * @param parameterName : name of the parameter that you want to save
     * @param fileOrDirectoryPath : absolute file or directory path to save
     * @param description : a short description to inform the user (that modify a script manually) what this path will say
     * @param uniqueID : (optionnal)-1 if you don't want to use it. A positive and unique index if you want to ensure the compatibility between script when you add/remove a parameter
     * @return a generated unique and positive ID that represent this parameter and can be used in method "addParameterInfo" per example.
     */
    virtual int addParameterPath(const QObject* caller, const QString& parameterName, const QString& fileOrDirectoryPath, const QString& description = "", const int& uniqueID = -1) = 0;

    /**
     * @brief Save extra informations for a parameter. Only use it to add some extra informations that can help the user that modify
     *        the script manually to have more informations about this parameter.
     *
     *        In the xml file the result is a couple of a key (EXTRA_INFO_NAME) and a value (EXTRA_INFO_VALUE) like this :
     *
     *        <CALLER_CLASSNAME PARAMETER_NAME="PARAMETER_VALUE" DESCRIPTION="DESC_VALUE" UNIQUE_ID="ID_VALUE" EXTRA_INFO_NAME="EXTRA_INFO_VALUE" />
     *
     * @param caller : always pass "this". The manager use the object to get the classname and set the name of the group.
     * @param parameterUniqueID : the unique ID returned by the method "addParameter"
     * @param extraInfoName : a unique name
     * @param extraInfoValue : a value (always a text)
     * @return false if the "extraInfoName" already exist.
     */
    virtual bool addParameterInfo(const QObject* caller, const int& parameterUniqueID, const QString& extraInfoName, const QVariant& extraInfoValue) = 0;

    /**
     * @brief Call it when you want to write settings of childrens of your class
     */
    virtual void beginWriteChildrens(const QObject* caller) = 0;

    /**
     * @brief Call it when you have finished to write settings of childrens in your class
     */
    virtual void endWriteChildrens(const QObject* caller) = 0;

    /**
     * @brief Create a group that will only be an informative part for the user that modify a script manually. Per example if you have a parameter
     *        that can be set with a value among a list of values you can add the list of values in a group.
     *
     *        In the xml file the result will always be like this :
     *
     *        <CALLER_CLASSNAME description="GROUP_DESCRIPTION" vgID="X" >
     *          <VALUES_NAME ... />
     *          ...
     *        </CALLER_CLASSNAME>
     *
     * @param caller : always pass "this". The manager use the object to get the classname and set the name of the group.
     * @param valuesName : the name of all values in the group
     * @param groupDescription : a description to inform the user what this group contains
     * @return a unique ID that you must use in the method "addExtraParameter" and "addParameter" to link with this group
     */
    virtual int createExtraParametersGroup(const QObject* caller, const QString& valuesName, const QString& groupDescription = "") = 0;

    /**
     * @brief Link a parameter and a group of extra parameter
     * @param caller : always pass "this".
     * @param parameterUniqueID : The unique ID returned by the method "addParameter".
     * @param parameterValuesGroupID : The unique ID returned by the method "createParameterValuesGroup".
     * @return false if group was not found or if it was already linked. True otherwise.
     */
    virtual bool linkParameterAndExtraParametersGroup(const QObject* caller, const int& parameterUniqueID, const int& parameterValuesGroupID) = 0;

    /**
     * @brief Add a value in "ExtraParameters" section. This section is only to inform user about the value that
     *        can be set to a parameter among a list of possible values.
     *
     *        In the xml file the result will always be like this :
     *
     *        <CALLER_CLASSNAME description="GROUP_DESCRIPTION" vgID="X" >
     *          <VALUES_NAME value="PARAMETER_VALUE" />
     *          <VALUES_NAME value="PARAMETER_VALUE" />
     *          ...
     *        </CALLER_CLASSNAME>
     *
     * @param caller : always pass "this".
     * @param value : value to save
     * @param parameterValuesGroupID : The unique ID returned by the method "createParameterValuesGroup".
     * @param description : a description of this value.
     * @return false if the group ID doesn't exist.
     */
    virtual bool addExtraParametersValueToGroup(const QObject* caller, const QVariant& value, const int& parameterValuesGroupID, const QString& description = "") = 0;
};

/**
 * @brief The settings reader is used to restore settings of elements from a script. It will use an xml file to read all
 *        parameters that you want to restore.
 */
class SettingsReaderInterface
{
public:

    virtual ~SettingsReaderInterface() {}

    /**
     * @brief Call it if you want to ensure the compatibility between script when you add/remove a parameter
     * @param caller : always pass "this"
     * @param uniqueID : pass the unique ID that you have used with the writer.
     * @return false if the group of the caller was found with this uniqueID, false otherwise.
     */
    virtual bool findCallerWithUniqueIDAndSetToCurrent(const QObject* caller, const int& uniqueID) = 0;

    /**
     * @brief Restore a parameter. If you have multiple parameter with the same name and same unique id you
     *        can call this method multiple times until it will return "false".
     *
     * @param caller : always pass "this". The manager use the object to get the classname to use for the name of the group.
     * @param parameterName : name of the parameter that you want to restore.
     * @param parameterValueRestored : a variable that will receive the readed value if founded or the "defaultParameterValue" otherwise
     * @param defaultParameterValue : a default value to use if the parameter was not found
     * @param uniqueID : (optionnal)-1 if you don't want to use it. A positive and unique index if you want to ensure the compatibility between script when you add/remove a parameter.
     * @return 0 or a negative value if the parameter was not found or it was an error, positive unique ID otherwise.
     */
    virtual int parameter(const QObject* caller, const QString& parameterName, QVariant& parameterValueRestored, const QVariant& defaultParameterValue = QVariant(), const int& uniqueID = -1) = 0;

    /**
     * @brief Count the number of parameter with the same "parameterName" and "uniqueID".
     * @param caller : always pass "this"
     * @param parameterName : name of the parameter that you want to count.
     * @param uniqueID : (optionnal)-1 if you don't want to use it. A positive and unique index if you want to ensure the compatibility between script when you add/remove a parameter.
     * @return the number of parameters founded
     */
    virtual int parameterCount(const QObject *caller, const QString &parameterName, const int& uniqueID = -1) = 0;

    /**
     * @brief Restore a PATH. Use when you want to restore a filepath or a directory path. Same result as the method "parameter" but
     *        will convert the relative path to an absolute path. If you have multiple parameter with the same name and same unique id you
     *        can call this method multiple times until it will return "false".
     *
     * @param caller : always pass "this". The manager use the object to get the classname and set the name of the group.
     * @param parameterName : name of the parameter that you want to restore.
     * @param fileOrDirectoryPathRestored : a variable that will receive the readed path if founded or the "defaultFileOrDirectoryPath" otherwise
     * @param defaultFileOrDirectoryPath : a default path to use if the parameter was not found
     * @param uniqueID : (optionnal)-1 if you don't want to use it. A positive and unique index if you want to ensure the compatibility between script when you add/remove a parameter
     * @return 0 or a negative value if the parameter was not found or it was an error, positive unique ID otherwise.
     */
    virtual int parameterPath(const QObject* caller, const QString& parameterName, QString& fileOrDirectoryPathRestored, const QString& defaultFileOrDirectoryPath = "", const int& uniqueID = -1) = 0;

    /**
     * @brief Count the number of path with the same "parameterName" and "uniqueID". "parameterName" if you want to count all parameters.
     * @param caller : always pass "this"
     * @param parameterName : name of the parameter that you want to count.
     * @param uniqueID : (optionnal)-1 if you don't want to use it. A positive and unique index if you want to ensure the compatibility between script when you add/remove a parameter.
     * @return the number of parameters founded
     */
    virtual int parameterPathCount(const QObject *caller, const QString &parameterName, const int& uniqueID = -1) = 0;

    /**
     * @brief Restore a extra info.
     *
     * @param caller : always pass "this". The manager use the object to get the classname to use for the name of the group.
     * @param parameterUniqueID : The unique ID returned by the method "parameter".
     * @param extraInfoName : name of the extra info that you want to restore
     * @param extraInfoValueRestored : a variable that will receive the readed value if founded or the "defaultValue" otherwise
     * @param defaultValue : a default value to use if the extra info was not found
     * @return false if "extraInfoName" was not found.
     */
    virtual bool parameterInfo(const QObject* caller, const int& parameterUniqueID, const QString& extraInfoName, QVariant& extraInfoValueRestored, const QVariant& defaultValue = QVariant()) = 0;

    /**
     * @brief Call it when you want to read settings of childrens of your class
     */
    virtual void beginReadChildrens(const QObject* caller) = 0;

    /**
     * @brief Call it when you have finished to read settings of childrens in your class
     */
    virtual void endReadChildrens(const QObject* caller) = 0;
};

#endif // SETTINGSINTERFACES_H
