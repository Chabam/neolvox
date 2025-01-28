#ifndef CDM_INTERNATIONALIZATION_H
#define CDM_INTERNATIONALIZATION_H

#include "computreeCore_global.h"

#include <QString>
#include <QFileInfo>
#include <QMultiMap>
#include <QTranslator>

class COMPUTREECORESHARED_EXPORT CDM_Internationalization
{
public:
    CDM_Internationalization();
    ~CDM_Internationalization();

    /**
     * @brief Returns a list of language available (converted to displayable string)
     */
    QStringList displayableLanguageAvailable() const;

    /**
     * @brief Returns a list of language available
     */
    QStringList languageAvailable() const;

    /**
     * @brief Return the index of the current language used
     */
    int currentLanguage() const;

    /**
     * @brief Set the new language to use (it will be available only at the next startup)
     */
    void useLanguage(int index);

    /**
     * @brief Load the configuration and set the language
     */
    void loadConfiguration();

    /**
     * @brief Save the configuration to the settings file
     */
    void saveConfiguration() const;

private:
    int                             m_currentLanguageIndex;
    int                             m_saveLanguageIndex;
    QString                         m_currentLanguageBaseName;
    QString                         m_languageDirectory;
    QMultiMap<QString, QFileInfo>   m_languageAvailable;
    QList<QTranslator*>             m_translators;
};

#endif // CDM_INTERNATIONALIZATION_H
