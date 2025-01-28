#ifndef CT_INRESULTMODELCONFIGURATIONMANAGER_H
#define CT_INRESULTMODELCONFIGURATIONMANAGER_H

#include "ctlibmodels_global.h"
#include "ct_model/inModel/manager/ct_inmodelstructuremanager.h"
#include "ct_model/inModel/views/ctg_inresultmodelconfiguration.h"
#include "ct_model/inModel/tools/ct_instdmodelpossibility.h"

#include <QUrl>

/**
 * @brief This class manage the creation of the dialog that let the user choose all input models that he want to use
 *        among a list of possibilities
 */
class CTLIBMODELS_EXPORT CT_InResultModelConfigurationManager
{
public:

    enum CreateDialogReturn
    {
        CreateError,
        CreateSuccess,
        CreateNotNecessary
    };

    enum ConfigureReturn
    {
        ConfigureError,
        ConfigureSuccess,
        ConfigureNotChanged,
        ConfigureCanceled
    };

    CT_InResultModelConfigurationManager(CT_InModelStructureManager& inManager);
    ~CT_InResultModelConfigurationManager();

    /**
     * @brief Check if must create or not the model configuration dialog and return a create dialog code
     * @return Returns CreateNotNecessary if create is not necessary or CreateSuccess if already created or creation is a success.
     * @warning Call it in GUI thread
     */
    template<typename InModelPossibilitiesWidgetType>
    CT_InResultModelConfigurationManager::CreateDialogReturn createInResultModelConfigurationDialog(const QString& extraTitle = QString(), const QUrl helpPath = QUrl())
    {
        CT_InResultModelConfigurationManager::CreateDialogReturn returnVal = CT_InResultModelConfigurationManager::CreateSuccess;

        // si on a pas plusieurs possibilités il n'y a rien a configurer
        if(!checkIfMustCreateOrShowConfigurationDialog())
            returnVal = CT_InResultModelConfigurationManager::CreateNotNecessary;

        if(m_inputModelsConfigurationDialog == nullptr)
        {
            m_inputModelsConfigurationDialog = new CTG_InResultModelConfiguration(new InModelPossibilitiesWidgetType(helpPath));
            m_inputModelsConfigurationDialog->setWindowState(m_inputModelsConfigurationDialog->windowState() | Qt::WindowMaximized);
            m_inputModelsConfigurationDialog->setInResultModelManager(&m_inModelsStructureManager);
            m_inputModelsConfigurationDialog->setWindowFlags(m_inputModelsConfigurationDialog->windowFlags() | Qt::WindowMaximizeButtonHint);
        }

        m_inputModelsConfigurationDialog->setWindowTitle(m_inputModelsConfigurationDialog->windowTitle() + extraTitle);

        return returnVal;
    }


    /**
     * @brief Call it to display the model configuration dialog to let the user configure input models to use.
     * @return Returns ConfigureSuccess if the user has click the "OK" button of the dialog or if the dialog have not
     *         to be displayed. Returns ConfigureCanceled if the user has click the cross or the "Cancel" button of the
     *         dialog. Returns ConfigureError if it was a problem.
     * @warning Call it in GUI thread
     */
    CT_InResultModelConfigurationManager::ConfigureReturn configureInResultModel(bool forceShow = false);

    /**
     * @brief Display the model configuration dialog in read only mode so the user cannot change anything
     * @warning Call it in GUI thread
     */
    void showReadOnlyInResultModel();

    /**
     * @brief Save all necessary elements to be able to restore it with the method "restoreSettings"
     */
    void saveSettings(SettingsWriterInterface& writer) const;

    /**
     * @brief Restore the state
     */
    template<typename InModelPossibilitiesWidgetType>
    bool restoreSettings(SettingsReaderInterface& reader, QUrl pathHelp)
    {
        // unselect all possibilities
        m_inModelsStructureManager.visitResults([](const CT_InAbstractResultModel* resultModel) -> bool {

            const_cast<CT_InAbstractResultModel*>(resultModel)->recursiveVisitPossibilities([](const CT_InAbstractModel*, const CT_InStdModelPossibility* possibility) -> bool {
                const_cast<CT_InStdModelPossibility*>(possibility)->setSelected(false);
                return true;
            });

            return true;
        });

        delete m_inputModelsConfigurationDialog;
        m_inputModelsConfigurationDialog = nullptr;

        createInResultModelConfigurationDialog<InModelPossibilitiesWidgetType>(QString(), pathHelp);

        const bool restoreOK = m_inModelsStructureManager.visitResults([&reader](const CT_InAbstractResultModel* resultModel) -> bool {
            return const_cast<CT_InAbstractResultModel*>(resultModel)->restoreSettings(reader);
        });

        if(restoreOK)
            return true;

        delete m_inputModelsConfigurationDialog;
        m_inputModelsConfigurationDialog = nullptr;

        return false;
    }


    /**
     * @brief Check if possibilities can be selected by default and if not returns false.
     * @return Returns true if possibilities can be selected by default so the dialog must not be displayed
     */
    bool checkIfMustCreateOrShowConfigurationDialog() const;

    /**
     * @brief Must be called to inform that it will be deleted so the dialog can be deleted properly
     * @warning Call it in GUI thread
     */
    void aboutToBeDeleted();


    /**
     * @brief Export screen capture of IN model configuration dialog
     */
    bool exportViewForINModelConfig(QString exportPath, QStringList &exportedFiles);

private:

    /**
     * @brief The input models manager
     */
    CT_InModelStructureManager&         m_inModelsStructureManager;

    /**
     * @brief The configuration dialog (created only if necessary)
     */
    CTG_InResultModelConfiguration*     m_inputModelsConfigurationDialog;
};

#endif // CT_INRESULTMODELCONFIGURATIONMANAGER_H
