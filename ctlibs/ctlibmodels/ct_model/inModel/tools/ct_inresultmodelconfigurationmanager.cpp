#include "ct_inresultmodelconfigurationmanager.h"

#include "ct_model/inModel/abstract/ct_inabstractresultmodel.h"
#include "ct_model/inModel/tools/ct_modelselectionhelper.h"

#include <QFileInfo>
#include <QTimer>
#include <QEventLoop>


CT_InResultModelConfigurationManager::CT_InResultModelConfigurationManager(CT_InModelStructureManager& inManager) : m_inModelsStructureManager(inManager)
{
    m_inputModelsConfigurationDialog = nullptr;
}

CT_InResultModelConfigurationManager::~CT_InResultModelConfigurationManager()
{
    delete m_inputModelsConfigurationDialog;
}

CT_InResultModelConfigurationManager::ConfigureReturn CT_InResultModelConfigurationManager::configureInResultModel(bool forceShow)
{
    if(checkIfMustCreateOrShowConfigurationDialog()) {
        Q_ASSERT(m_inputModelsConfigurationDialog != nullptr);
        if (m_inputModelsConfigurationDialog == nullptr) {qDebug() << "CT_InResultModelConfigurationManager::ConfigureReturn" << ", " <<  "m_inputModelsConfigurationDialog == nullptr";}

        return (m_inputModelsConfigurationDialog->exec() == QDialog::Accepted) ? CT_InResultModelConfigurationManager::ConfigureSuccess : CT_InResultModelConfigurationManager::ConfigureCanceled;
    }

    // select all possibilities by default
    const bool ok = m_inModelsStructureManager.visitResults([](const CT_InAbstractResultModel* resultModel) -> bool {

        const_cast<CT_InAbstractResultModel*>(resultModel)->recursiveVisitPossibilities([](const CT_InAbstractModel*, const CT_InStdModelPossibility* possibility) -> bool {
            const_cast<CT_InStdModelPossibility*>(possibility)->setSelected(false);
            return true;
        });

        return CT_ModelSelectionHelper(resultModel).recursiveSelectAllPossibilitiesByDefault();
    });

    if(forceShow && ok) {
        Q_ASSERT(m_inputModelsConfigurationDialog != nullptr);
        if (m_inputModelsConfigurationDialog == nullptr) {qDebug() << "CT_InResultModelConfigurationManager::configureInResultModel" << ", " <<  "m_inputModelsConfigurationDialog == nullptr";}

        return (m_inputModelsConfigurationDialog->exec() == QDialog::Accepted) ? CT_InResultModelConfigurationManager::ConfigureSuccess : CT_InResultModelConfigurationManager::ConfigureCanceled;
    }

    return ok ? CT_InResultModelConfigurationManager::ConfigureSuccess : CT_InResultModelConfigurationManager::ConfigureError;
}

void CT_InResultModelConfigurationManager::showReadOnlyInResultModel()
{
    if(m_inputModelsConfigurationDialog != nullptr)
    {
        m_inputModelsConfigurationDialog->setReadOnly(true);
        m_inputModelsConfigurationDialog->exec();
        m_inputModelsConfigurationDialog->setReadOnly(false);
    }
}

bool CT_InResultModelConfigurationManager::exportViewForINModelConfig(QString exportPath, QStringList &exportedFiles)
{
    bool isValid = true;

    if(m_inputModelsConfigurationDialog != nullptr)
    {
        int nresults = m_inputModelsConfigurationDialog->getSelectableResultsNumber();

        for (int i = 0 ; i < nresults ; i++)
        {
            m_inputModelsConfigurationDialog->setReadOnly(true);
            m_inputModelsConfigurationDialog->open();

            m_inputModelsConfigurationDialog->selectResultByRank(i);

            QSize size = m_inputModelsConfigurationDialog->rect().size();

            QSize size2(1400, 400);
            m_inputModelsConfigurationDialog->resize(size2);

            QString exportPath2 = exportPath;

            QFileInfo finfo(exportPath);
            QString exportFileName = QString("%1.%2").arg(finfo.baseName(), finfo.suffix());

            if (nresults > 1)
            {
                exportPath2 = QString("%1/%2_%3.%4").arg(finfo.path()).arg(finfo.baseName()).arg(i+1).arg(finfo.suffix());
                exportFileName = QString("%1_%2.%3").arg(finfo.baseName()).arg(i+1).arg(finfo.suffix());
            }

            bool exportOk = m_inputModelsConfigurationDialog->exportViewCapture(exportPath2);
            isValid = isValid && exportOk;

            if (exportOk)
            {
                exportedFiles.append(exportFileName);
            }


            m_inputModelsConfigurationDialog->resize(size);
            m_inputModelsConfigurationDialog->accept();

            m_inputModelsConfigurationDialog->setReadOnly(false);
        }

        return isValid;
    }
    return false;
}

void CT_InResultModelConfigurationManager::saveSettings(SettingsWriterInterface& writer) const
{
    m_inModelsStructureManager.visitResults([&writer](const CT_InAbstractResultModel* resultModel) -> bool {
        resultModel->saveSettings(writer);
        return true;
    });
}

bool CT_InResultModelConfigurationManager::checkIfMustCreateOrShowConfigurationDialog() const
{
    return !m_inModelsStructureManager.visitResults([](const CT_InAbstractResultModel* resultModel) -> bool {
        // if there is at least one possibility already checked we must show the configuration dialog
        if(resultModel->isAtLeastOnePossibilitySelected())
            return false;

        // we check if it can select all possibilities by default
        return CT_ModelSelectionHelper(resultModel).recursiveCanSelectAllPossibilitiesByDefault();
    });
}

void CT_InResultModelConfigurationManager::aboutToBeDeleted()
{
    delete m_inputModelsConfigurationDialog;
    m_inputModelsConfigurationDialog = nullptr;
}
