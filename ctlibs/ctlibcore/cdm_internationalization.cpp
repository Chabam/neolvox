#include "cdm_internationalization.h"

#include "cdm_configfile.h"

#include <QTranslator>
#include <QApplication>
#include <QDir>
#include <QLocale>

CDM_Internationalization::CDM_Internationalization()
{
    m_currentLanguageIndex = -1;
    m_saveLanguageIndex = -1;
    m_currentLanguageBaseName = "en";
#if defined(_WIN32) && defined(_MSC_VER) // Microsoft Visual Studio Compiler
    m_languageDirectory = "/languages";
#elif (defined(__linux__) || defined(_WIN32)) && defined(__GNUC__) // GNU Compiler (gcc,g++) for Linux, Unix, and MinGW (Windows)
    m_languageDirectory = "/../languages";
#elif defined(__APPLE__) // Clang Compiler (Apple)
    m_languageDirectory = "/../Languages";
#endif
}

CDM_Internationalization::~CDM_Internationalization()
{
    qDeleteAll(m_translators.begin(), m_translators.end());
}

QStringList CDM_Internationalization::displayableLanguageAvailable() const
{
    QStringList r;
    QList<QString> l = m_languageAvailable.uniqueKeys();

    foreach (QString i, l) {
        r.append(QLocale::languageToString(QLocale(i).language()));
    }

    return r;
}

QStringList CDM_Internationalization::languageAvailable() const
{
    return m_languageAvailable.uniqueKeys();
}

int CDM_Internationalization::currentLanguage() const
{
    int i = 0;

    QList<QString> l = m_languageAvailable.uniqueKeys();
    QListIterator<QString> it(l);

    while(it.hasNext())
    {
        QString baseName = it.next();

        if(baseName == m_currentLanguageBaseName)
            return i;

        ++i;
    }

    return -1;
}

void CDM_Internationalization::useLanguage(int index)
{
    m_saveLanguageIndex = index;
}

void CDM_Internationalization::loadConfiguration()
{
    CONFIG_FILE->beginGroup("InternationalizationManager");
    m_languageDirectory = QDir::toNativeSeparators(CONFIG_FILE->value("LanguageDirectory", m_languageDirectory).toString());
    m_currentLanguageBaseName = CONFIG_FILE->value("CurrentLanguage", m_currentLanguageBaseName).toString();
    CONFIG_FILE->endGroup();

    QDir dir(qApp->applicationDirPath() + m_languageDirectory);
    QFileInfoList il = dir.entryInfoList(QStringList() << "*.qm");

    // Multiple translation files is used because we have at least one translation file by plugin
    // Each file must have this form : pluginName_XX_XX.qm where XX_XX can be fr or fr_ca or en etc...
    foreach (QFileInfo inf, il)
        m_languageAvailable.insert(inf.baseName().mid(inf.baseName().lastIndexOf('_')+1).toLower(), inf);

    m_currentLanguageIndex = currentLanguage();

    if(m_currentLanguageIndex == -1)
    {
        m_currentLanguageBaseName = "";

        if(!m_languageAvailable.isEmpty())
        {
            m_currentLanguageBaseName = m_languageAvailable.uniqueKeys().first();
            m_currentLanguageIndex = 0;
        }
    }

    il = m_languageAvailable.values(m_currentLanguageBaseName);

    foreach (QFileInfo inf, il) {
        QTranslator *translator = new QTranslator();
        m_translators.append(translator);

        if(translator->load(inf.absoluteFilePath()))
            qApp->installTranslator(translator);
    }

    m_saveLanguageIndex = m_currentLanguageIndex;
}

void CDM_Internationalization::saveConfiguration() const
{
    CONFIG_FILE->beginGroup("InternationalizationManager");
    CONFIG_FILE->setValue("LanguageDirectory", m_languageDirectory);
    CONFIG_FILE->setValue("CurrentLanguage", m_languageAvailable.isEmpty() ? "" : m_languageAvailable.uniqueKeys().at(m_saveLanguageIndex));
    CONFIG_FILE->endGroup();
}
