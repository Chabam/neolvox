#ifndef CDM_CONFIGFILE_H
#define CDM_CONFIGFILE_H

#include "computreeCore_global.h"

#include <QSettings>
#include <QColor>

/**
  * \brief Cette classe peut-etre utilise en singleton. On peut y acceder a partir de cette methode statique :
  */
#define CONFIG_FILE CDM_ConfigFile::createInstance()

/**
  * \brief Classe permettant de lire/ecrire au fichier de configuration de l'application
  *        des parametres.
  */
class COMPUTREECORESHARED_EXPORT CDM_ConfigFile : public QSettings
{
public:

    CDM_ConfigFile(const QString &fileName, QSettings::Format format);

    static CDM_ConfigFile* createInstance();

    /**
      * \brief Ecriture d'une couleur.
      */
    void setColorValue(const QString &key, const QColor &value);

    /**
      * \brief Lecture d'une couleur.
      */
    QColor colorValue(const QString &key, const QColor &defaultValue) const;

private:

    static CDM_ConfigFile   *_uniqueInstance;
};

#endif // CDM_CONFIGFILE_H
