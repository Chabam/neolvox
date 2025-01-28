#ifndef CTG_INRESULTMODELCONFIGURATION_H
#define CTG_INRESULTMODELCONFIGURATION_H

#include "ct_model/inModel/manager/ct_inmodelstructuremanager.h"
#include "ct_model/inModel/views/iinmodelpossibilitieschoice.h"

#include <QDialog>
#include <QString>

namespace Ui {
class CTG_InResultModelConfiguration;
}

/**
 * @brief Dialog that present all input models and their possibilities and let the user choose
 *        which possibility use with which input model.
 */
class CTLIBMODELS_EXPORT CTG_InResultModelConfiguration : public QDialog
{
    Q_OBJECT
    
public:
    explicit CTG_InResultModelConfiguration(IInModelPossibilitiesChoice* widgetInModelPossibilities, QWidget* parent = nullptr);
    ~CTG_InResultModelConfiguration() final;
    
    /**
     * @brief Set the input result models manager to use
     */
    void setInResultModelManager(const CT_InModelStructureManager* manager);

    /**
     * @brief Set if the dialog must be read only or not
     * @param enabled : true if you want that it was read only so the user cannot change anything.
     */
    void setReadOnly(bool enabled);


    /**
     * @brief Export screen capture of IN model configuration dialog
     */
    bool exportViewCapture(QString exportPath);

    /**
     * @brief Return the number of results to configure
     */
    int getSelectableResultsNumber();


    /**
     * @brief Select result by rank
     */
    void selectResultByRank(int rank);

private:
    Ui::CTG_InResultModelConfiguration* ui;

    /**
     * @brief Manager of input result models
     */
    CT_InModelStructureManager*         m_inputResultModelManager;

    IInModelPossibilitiesChoice*        mInModelPossibilities;

public slots:
    /**
     * @brief Overrided to display a message box if somthing is wrong in configuration and to disable the close action of the dialog
     */
    void accept() override;

private slots:
    void captureView();

};

#endif // CTG_INRESULTMODELCONFIGURATION_H
