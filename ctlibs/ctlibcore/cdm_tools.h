#ifndef CDM_TOOLS_H
#define CDM_TOOLS_H

#include <QMenu>

#include "cdm_pluginmanager.h"
#include "ct_exporter/abstract/ct_abstractexporter.h"

class COMPUTREECORESHARED_EXPORT CDM_Tools
{
public:
    CDM_Tools(const CDM_PluginManager *pm);

    /********** EXPORTERS *************/

    /*!
     *  \brief Crée des actions que vous pouvez utilisé dans votre programme.
     *
     *  \param receiver : l'objet à qui appartient le SLOT "slot"
     *  \param slot : un SLOT que vous voulez connecter au SIGNAL "triggered" d'une QAction d'un exportateur
     *
     *  \return nullptr si aucun exportateur n'existe.
     */
    QMenu* createMenuForAllExporters(const QObject *receiver, const char *slot) const;

    /*!
     *  \brief Crée un filtre que vous pouvez utiliser dans une fenêtre d'ouverture/sauvegarde de fichier pour l'exportateur passé en paramètre
     */
    QString createFileExtensionForExporter(CT_AbstractExporter *exporter) const;

    QString createNameForExporter(CT_AbstractExporter *exporter) const;

private:
    CDM_PluginManager   *m_pm;
};

#endif // CDM_TOOLS_H
