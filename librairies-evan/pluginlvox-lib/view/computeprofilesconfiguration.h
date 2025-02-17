#ifndef COMPUTEPROFILESCONFIGURATION_H
#define COMPUTEPROFILESCONFIGURATION_H

#include <QWidget>

#include "ct_view/ct_abstractconfigurablewidget.h"

namespace Ui {
class ComputeProfilesConfiguration;
}

class ComputeProfilesConfiguration : public CT_AbstractConfigurableWidget
{
    Q_OBJECT

public:
    struct Configuration {
        double              minValue;
        double              maxValue;
        QString             ordonneeAxis;
        QString             genAxis;
        QString             unitType;
        bool                gridCustomized;
        bool                genValuesInPourcent;
        double              minGen;
        double              maxGen;
        int                 stepGen;
        bool                terminalCustomized;
        bool                abscisseOrdonneeValuesInPourcent;
        bool                averaged;
        double              minAbscisse;
        double              maxAbscisse;
        double              minOrdonnee;
        double              maxOrdonnee;
    };

    explicit ComputeProfilesConfiguration(QWidget *parent = 0);
    ~ComputeProfilesConfiguration();

    /**
     * @brief Set the configuration (the UI will change)
     */
    void setConfiguration(const Configuration& c);

    /**
     * @brief Get configuration from UI
     */
    Configuration getConfiguration() const;

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
    Ui::ComputeProfilesConfiguration *ui;
    Configuration m_defaultConfiguration;

private slots:
    void on_pushButtonDefaultConfiguration_clicked();
    void on_comboBoxOrdonnee_currentIndexChanged(int index);
    void on_radioButtonAllGrid_toggled(bool checked);
    void on_checkBoxPersonalized_toggled(bool checked);
};

#endif // COMPUTEPROFILESCONFIGURATION_H
