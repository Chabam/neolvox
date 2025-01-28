#ifndef CDM_SCRIPTMANAGERABSTRACT_H
#define CDM_SCRIPTMANAGERABSTRACT_H

#include "computreeCore_global.h"

#include "cdm_stepmanager.h"
#include "cdm_pluginmanager.h"
#include "cdm_scriptproblem.h"

/**
  * \brief Classe permettant d'ecrire/lire un script d'etapes afin de creer les etapes et les ajouter
  *        a un manageur d'etape.
  */
class COMPUTREECORESHARED_EXPORT CDM_ScriptManagerAbstract
{
public:

    class IScriptLoadCallBack {
    public:
        virtual ~IScriptLoadCallBack() {}

        /**
         * @brief Method called when a script problem occurs
         * @param problem : the problem to use to set the solution
         */
        virtual void loadScriptError(CDM_ScriptProblem &problem) = 0;
    };

    CDM_ScriptManagerAbstract(CDM_PluginManager &pluginManager);
    virtual ~CDM_ScriptManagerAbstract() {}

    /**
     * @brief Set the object who get the callback error when a script is not successfully loaded
     */
    void setScriptLoadCallBack(IScriptLoadCallBack *c);

    /**
     * @brief Returns the object that was called when an error ocurred when a script is loaded
     */
    IScriptLoadCallBack* scriptLoadCallBack() const;

    /*!
     *  \brief Defini le plugin manager utilise pour creer les etapes
     */
    void setPluginManager(CDM_PluginManager &pluginManager);

    /*!
     *  \brief Retourne le manageur de plugin utilise
     */
    CDM_PluginManager* getPluginManager() const;

    /*!
     *  \brief Liste les extensions des fichiers de script acceptes
     */
    virtual QList<QString> getFileExtensionAccepted() const = 0;

    /*!
     *  \brief Accepte ce fichier ?
     *
     *  \return Retourne vrai si le script pourra etre lu (si l'extension du fichier est bonne).
     */
    virtual bool acceptFile(const QString &filePath) const;

    /*!
     *  \brief Verifie le script et renvoie un message d'erreur si il y en a une, sinon renvoie ""
     */
    virtual QString verifyScript(const QString &filePath) = 0;
    virtual QString verifyScriptText(const QString &text) = 0;

    /*!
     *  \brief Charge le script (cre les etapes a l'aide du PluginManager et les ajoutes au StepManager)
     */
    virtual QString loadScript(const QString &filePath,
                               CDM_StepManager &stepManager) = 0;
    virtual QString loadScriptText(const QString &text,
                                   CDM_StepManager &stepManager) = 0;

    /*!
     *  \brief Ecrit les identifiants et options de chaque etapes contenu dans le StepManager, a l'aide du PluginManager
     */
    virtual bool writeScript(const QString &filePath,
                             bool saveResult,
                             CDM_StepManager &stepManager) = 0;

private:

    CDM_PluginManager       *_pluginManager;
    IScriptLoadCallBack     *m_callBack;

protected:
    virtual void scriptLoadCallBackChanged() {}
};

#endif // CDM_SCRIPTMANAGERABSTRACT_H
