/**
 * @author Michael Krebs (AMVALOR)
 * @date 25.01.2017
 * @version 1
 */
#ifndef PREDEFINEDMAPPERCONFIGURATION_H
#define PREDEFINEDMAPPERCONFIGURATION_H

#include "genericcomputegridsconfiguration.h"

namespace Ui {
class PredefinedMapperConfiguration;
}

/**
 * @brief Use this widget to map between available grids in input and grids defined in a predefined configuration
 */
class PredefinedMapperConfiguration : public CT_AbstractConfigurableWidget
{
    Q_OBJECT

public:
    explicit PredefinedMapperConfiguration(QWidget *parent = 0);
    ~PredefinedMapperConfiguration();

    /**
     * @brief Set all necessary
     */
    bool setNecessaryConfiguration(const QList<lvox::InGridConfiguration>& input, lvox::PredefinedConfiguration predefined);

    /**
     * @brief Return the predefined configuration mapped
     */
    lvox::PredefinedConfiguration getPredefinedConfigurationMapped() const;

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
    Ui::PredefinedMapperConfiguration *ui;

    QList<lvox::InGridConfiguration>    m_inputs;
    lvox::PredefinedConfiguration       m_predefined;

    /**
     * @brief Reset ui
     */
    void resetAndIniUI();

    /**
     * @brief Create a new combobox with var available
     */
    QComboBox* createNewVarComboBox() const;

    /**
     * @brief Create a new combobox with name available
     */
    QComboBox* createNewNameAndTypeComboBox() const;

    /**
     * @brief Construct the hash table to know what letter replace by what. The hash table
     *        contains in key the letter to replace in the formula and in value the letter replacement.
     */
    bool constructMapper(QMap<char, char> &gridsLettersMap) const;

    /**
     * @brief Replace all letters in the formula by those that the user choosed. The hash table
     *        must contains in key the letter to replace in the formula and in value the letter replacement.
     */
    void mapFormula(QString& formula, const QMap<char, char> &gridsLettersMap);

    /**
     * @brief Returns the grid letter to replace by those specified in parameter
     */
    char findGridLetterToReplace(const char& gridLetterOfComboBox);
};

#endif // PREDEFINEDMAPPERCONFIGURATION_H
