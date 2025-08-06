#ifndef COMPUTEPADCONFIGURATION_H
#define COMPUTEPADCONFIGURATION_H

#include "ui_computepadconfiguration.h"
#include <QDialog>
#include "ct_view/ct_abstractconfigurablewidget.h"

class ComputePadConfiguration : public CT_AbstractConfigurableWidget
{
    Q_OBJECT

public:
    struct Configuration
    {
        QHash<QString, bool> methodsStatus;
        uint32_t NThreshold;
        bool isRelativeThreshold;
        bool grilleSup;
    };

    explicit ComputePadConfiguration(QWidget *parent = nullptr);
    ~ComputePadConfiguration();

    /**
     * @brief Set the configuration (the UI will change)
     */
    void setConfiguration(const Configuration& c);

    /**
     * @brief Get configuration from UI
     */
    Configuration getConfiguration() const;

    /**
     * @brief Returns true if user has modified something
     */
    bool isSettingsModified() const final;


    bool canAcceptIt(QString *err) const final;

    uint32_t getThreshold();
    bool isRelativeThreshold();
    bool createSummationsGrids();
    bool isMethodSelected(QString methodName);

    /**
    * \brief addPADComputationMethods
    * \param displayableName
    * \param description
    * \param value
    **/
    void setPADComputationMethod(QString displayableName, QString description, bool value);
    void setNThreshold(uint32_t value);
    void setUseRelativeThreshold(bool value);
    void setCreateSummationGrids(bool value);


public slots:
    /**
     * @brief Update the element to match the configuration defined by the user
     * @param err : (optionnal) will not be empty if an error can be displayed and if different of nullptr
     * @return true if update was successfull, otherwise returns false and filled the "err" parameter if it was not nullptr
     */
    bool updateElement(QString *err = nullptr) final;


    /**
     * @brief Update the ui to match the configuration of the element. Do nothing by default.
     */
    virtual void updateUIFromElement() {}

private:
    Ui::ComputePadConfiguration *ui;
    QHash<QString, bool> choices;
    Configuration _c;
};

#endif // COMPUTEPADCONFIGURATION_H
