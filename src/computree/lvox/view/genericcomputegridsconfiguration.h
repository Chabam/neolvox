/**
 * @author Michael Krebs (AMVALOR)
 * @date 25.01.2017
 * @version 1
 */
#ifndef GENERICCOMPUTEGRIDSCONFIGURATION_H
#define GENERICCOMPUTEGRIDSCONFIGURATION_H

#include <QWidget>
#include <QComboBox>

#include "ct_view/ct_abstractconfigurablewidget.h"

#include "ctlibio/readers/ct_reader_asciigrid3d.h"
#include "tools/lvox3_genericconfiguration.h"

namespace Ui {
class GenericComputeGridsConfiguration;
}

class QTableWidgetItem;

/**
 * @brief Use this widget to let the user create as many output grids as he want and configure what
 *        test or compute to apply for each output grids.
 */
class GenericComputeGridsConfiguration : public CT_AbstractConfigurableWidget
{
    Q_OBJECT

public:

    explicit GenericComputeGridsConfiguration(QWidget *parent = 0);
    ~GenericComputeGridsConfiguration();

    /**
     * @brief Set all input grids
     */
    void setInputGridConfiguration(const QList<lvox::InGridConfiguration>& confs);

    /**
     * @brief Set all output grids (replace all)
     */
    void setOutputGridConfiguration(const QList<lvox::OutGridConfiguration>& confs);

    /**
     * @brief Add other output grids (add so don't replace all)
     */
    void addOutputGridConfiguration(const QList<lvox::OutGridConfiguration>& confs);

    /**
     * @brief Add a predefined configuration
     * @return false if the predefined configuration is not valid
     */
    bool addPredefinedConfiguration(const lvox::PredefinedConfiguration& conf);

    /**
     * @brief Set all input grids
     */
    QList<lvox::InGridConfiguration> getInputGridConfiguration() const;

    /**
     * @brief Set all output grids
     */
    QList<lvox::OutGridConfiguration> getOutputGridConfiguration() const;

    /**
     * @brief Returns true if it was no error and "updateElement" method can be called
     * @param err : (optionnal) will not be empty if an error can be displayed and if different of NULL
     */
    bool canAcceptIt(QString *err = NULL) const;

    /**
     * @brief Returns true if user has modified something
     */
    bool isSettingsModified() const;

public slots:
    /**
     * @brief Update the element to match the configuration defined by the user
     * @param err : (optionnal) will not be empty if an error can be displayed and if different of NULL
     * @return true if update was successfull, otherwise returns false and filled the "err" parameter if it was not NULL
     */
    bool updateElement(QString *err = NULL);

private:
    Ui::GenericComputeGridsConfiguration    *ui;

    QList<lvox::InGridConfiguration>            m_inputconfiguration;
    QList<lvox::OutGridConfiguration>           m_outputconfiguration;
    QList<lvox::PredefinedConfiguration>        m_predefinedConfiguration;
    bool                                        m_reset;

    /**
     * @brief Update UI to reflect the input/output configuration
     */
    void resetAndUpdateUI(int outCurrentRow = 0);

    /**
     * @brief Returns a new combobox with all grid letter that can be used
     */
    QComboBox* createLetterComboBox() const;

    /**
     * @brief Returns a new combobox with all data type that can be used
     */
    QComboBox* createDataTypeComboBox() const;

    /**
     * @brief Save the configuration (from UI) of the item specified
     */
    void saveConfiguration(int row);

    /**
     * @brief Restore the configuration (to UI) of the item specified
     */
    void restoreConfiguration(int row);

    /**
     * @brief Init header of tables
     */
    void initTableHeaders();

    /**
     * @brief Adjust all table widget columns to content
     */
    void adjustTableWidgetColumnsToContents();

    /**
     * @brief Set (add = false) or add (add = true) the current predefined configuration
     */
    void setOrAddCurrentPredefinedConfiguration(bool add = false);

private slots:

    /**
     * @brief Save the configuration of the current ouput grid
     */
    void saveCurrentConfiguration();

    /**
     * @brief Called when the current item in out grid table changed
     */
    void on_tableWidgetOutGrid_cellChanged(int row, int column);

    /**
     * @brief Called when the current item in check grid table has changed
     */
    void on_tableWidgetChecks_cellChanged(int row, int column);

    /**
     * @brief User want to apply a predefined configuration
     */
    void on_pushButtonApplyPredefinedCompute_clicked();

    /**
     * @brief User want to add a predefined configuration
     */
    void on_pushButtonAddPredefinedCompute_clicked();

    /**
     * @brief User want to add a new out grid
     */
    void on_pushButtonAddOutGrid_clicked();

    /**
     * @brief User want to delete an out grid
     */
    void on_pushButtonDeleteOutGrid_clicked();

    /**
     * @brief User want to add a new check
     */
    void on_pushButtonAddCheck_clicked();

    /**
     * @brief User want to delete a check
     */
    void on_pushButtonDeleteCheck_clicked();

    /**
     * @brief User change the predefined configuration in the combobox
     */
    void on_comboBoxPredefinedCompute_currentIndexChanged(int index);

    /**
     * @brief User change the out grid to configure in the combobox
     */
    void on_comboBoxOutGridToConfigure_currentIndexChanged(int index);

    /**
     * @brief User change the name of a variable in output table
     */
    void comboBoxLetterChanged(int index);

    /**
     * @brief User change the name of a variable in output table
     */
    void comboBoxDataTypeChanged(int index);

    /**
     * @brief User change NA Value
     */
    void spinBoxNAValueChanged(double value);

    /**
     * @brief User change default Value
     */
    void spinBoxDefaultValueChanged(double value);

    /**
     * @brief Update the label
     */
    void updateLabelConfigureOutGrid();
};

#endif // GENERICCOMPUTEGRIDSCONFIGURATION_H
