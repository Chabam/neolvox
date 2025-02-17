/**
 * @author Michael Krebs (AMVALOR)
 * @date 25.01.2017
 * @version 1
 */
#ifndef LOADFILECONFIGURATION_H
#define LOADFILECONFIGURATION_H

#include <QWidget>

#include "Eigen/Core"

#include "ct_view/ct_abstractconfigurablewidget.h"
#include "tools/lvox3_scannerutils.h"

namespace Ui {
class LoadFileConfiguration;
}

class QListWidgetItem;
class CT_AbstractReader;

class LoadFileConfiguration : public CT_AbstractConfigurableWidget
{
    Q_OBJECT

public:
    struct Configuration {
        Configuration() :
            scannerId(ScannerSphericPointCloud),
            clockWise(true),
            radians(false),
            filepath(),
            scannerPosition(0., 0., 0.),
            scannerResolution(0.036, 0.036),
            scannerThetaRange(0., 360.),
            scannerPhiRange(0, 180.),
            scannerDirection(0., 0., -1.) {}

        ScannerTypeEnum scannerId;
        bool clockWise;
        bool radians;
        QString filepath;
        Eigen::Vector3d scannerPosition;
        Eigen::Vector2d scannerResolution;
        Eigen::Vector2d scannerThetaRange;
        Eigen::Vector2d scannerPhiRange;
        Eigen::Vector3d scannerDirection;
    };

    explicit LoadFileConfiguration(QWidget *parent = 0);
    ~LoadFileConfiguration();

    /**
     * @brief Set readers to use
     */
    void setReaders(const QList<CT_AbstractReader*>& readers);

    /**
     * @brief Set the current reader
     */
    void setCurrentReaderByClassName(const QString& readerClassName);

    /**
     * @brief Set all configurations
     */
    void setConfiguration(const QList<Configuration>& configs);

    /**
     * @brief Set the current scanner type
     * @param scanLabel from scanner definitions
     */
    void setCurrentScannerType(ScannerTypeEnum scanId);

    /**
     * @brief enable or disable filter points at origin
     */
    void setFilterPointsOrigin(bool enable);

    /**
     * @brief returns if the user requested filtering points at origin
     */
    bool isFilterPointsOrigin() const;

    /**
     * @brief enable or disable restrict scene
     */
    void setRestrictScene(bool enable);

    /**
     * @brief returns if the user requested restrict scene
     */
    bool isRestrictScene() const;

    /**
     * @brief sets the value of restrict scene spinbox
     */
    void setRestrictRadius(int radius);

    /**
     * @brief returns if the user requested restrict radius
     */
    int getRestrictRadius() const;

    /**
     * @brief Returns all configuration
     */
    QList<Configuration> getConfiguration() const;

    /**
     * @brief Return the reader to use (you must copy it to use it!)
     */
    CT_AbstractReader* getReaderToUse() const;

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

    Ui::LoadFileConfiguration               *ui;
    QMap<QListWidgetItem*, Configuration>   m_filesScannerConfiguration;
    QList<CT_AbstractReader*>               m_availableReaders;

    /**
     * @brief Clear the current combobox and use the variable "m_availableReaders" to populate the combobox
     */
    void initReaderComboBox();

    /**
     * @brief Change the UI to show value of the configuration of the item specified
     */
    void editItem(QListWidgetItem *item);

    /**
     * @brief Update the Configuration of the item specified by use the UI values
     */
    void updateConfiguration(QListWidgetItem *item);

    /**
     * @brief Show a QFileDialog to choose a file that can be oppened by the current reader
     * @param filepath (OUT) : will have the list of filepath choosed by user
     * @param multiple : to true if the user can select multiple files
     * @return false if the user cancel, true otherwise.
     */
    bool chooseFiles(QStringList &filepath, bool multiple);

    /**
     * @brief Returns the current reader selected in the combobox
     */
    CT_AbstractReader* getSelectedReader() const;

    /**
     * @brief Returns the current item selected in the listwidget
     */
    QListWidgetItem* getSelectedItem() const;

private slots:
    /**
     * @brief Call it to add one or more files to the list (it will show a QFileDialog to choose it)
     */
    void addFile();

    /**
     * @brief Call it to modify the current item in the list (it will show a QFileDialog to choose another file)
     */
    void modifySelectedFile();

    /**
     * @brief import a file with multiple scans
     */
    void importFile();

    /**
     * @brief Export all loaded .xyb files to a .in file
     */
    void exportToInFile();

    /**
     * @brief Remove the current item in the list
     */
    void removeSelectedFile();

    /**
     * @brief Remove all item in the list
     */
    void removeAllFile();

    /**
     * @brief Apply the current configuration to all item in the list
     */
    void on_pushButtonApplyConfigurationToNextFile_clicked();

    /**
     * @brief Apply the current configuration to the next item in the list
     */
    void on_pushButtonApplyConfigurationToAll_clicked();

    /**
     * @brief Set enable/disable pushbutton, checkbox, etc... regardless the state of other element in UI
     */
    void on_listWidgetFiles_currentRowChanged(int currentRow);

    /**
     * @brief Update the configuration of the previous item and update UI to show the configuration of the current
     */
    void on_listWidgetFiles_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    /**
     * @brief Call "modifySelectedFile"
     */
    void on_listWidgetFiles_itemDoubleClicked(QListWidgetItem *item);

    /**
     * @brief Clear the item list if the reader change (show a message box to user to inform it)
     */
    void on_comboBoxReaderType_currentIndexChanged(int index);

    /**
     * @brief Change the current scanner type to use
     */
    void on_comboBoxScannerType_currentIndexChanged(int index);

    /**
     * @brief Change the enabled state of label_7 and spinboxRestrictRadius
     */
    void on_checkBoxRestrictScene_toggled(bool checked);
};

#endif // LOADFILECONFIGURATION_H
