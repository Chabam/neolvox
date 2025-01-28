#ifndef CTG_CONFIGURABLEELEMENTSSELECTOR_H
#define CTG_CONFIGURABLEELEMENTSSELECTOR_H

#include "ct_element/abstract/ct_abstractconfigurableelement.h"

#include <QDialog>

namespace Ui {
class CTG_ConfigurableElementsSelector;
}

class QListWidgetItem;

/**
 * @brief Dialog to allow a user to select multiple "CT_AbstractConfigurableElement" and configure it.
 */
class CTLIBWIDGET_EXPORT CTG_ConfigurableElementsSelector : public QDialog
{
    Q_OBJECT

public:
    explicit CTG_ConfigurableElementsSelector(QWidget *parent = 0, bool readOnly = false);
    ~CTG_ConfigurableElementsSelector();

    /**
     * @brief Set elements that you want the user can choose
     */
    void setElementsAvailable(const QList<CT_AbstractConfigurableElement*> &elements);

    /**
     * @brief Set elements that was already selected. This list will be changed if user accept the change.
     * @return true if all elements was found in available elements list, false otherwise.
     */
    bool setElementsSelected(QList<CT_AbstractConfigurableElement *> *elements);

    /**
     * @brief Returns a list of elements choosed by the user and configured.
     * @warning Elements in the list was copies ! So you must delete it if you do not want to use it later.
     */
    QList<CT_AbstractConfigurableElement*> takeElementsSelectedAndConfigured();

    /**
     * @brief Returns true if the dialog is read only
     */
    bool readOnly() const;


protected:

    virtual void resizeEvent(QResizeEvent *e) override;

private:
    Ui::CTG_ConfigurableElementsSelector        *ui;
    QList<CT_AbstractConfigurableElement*>      *m_selectedContainerToModify;
    bool                                        m_readOnly;

    /**
     * @brief Add elements to widget list available
     */
    void addAvailableElementsToUI(const QList<CT_AbstractConfigurableElement*> &elements);

    /**
     * @brief Add elements to widget list selected
     */
    bool addSelectedElementsToUI(const QList<CT_AbstractConfigurableElement*> &elements, QList<QListWidgetItem *> &transfered);

    /**
     * @brief Get the CT_AbstractConfigurableElement from a item
     */
    CT_AbstractConfigurableElement* configurableElementFromItem(const QListWidgetItem *item) const;

    /**
     * @brief Get the configurable widget from a item
     */
    CT_AbstractConfigurableWidget* configurableWidgetFromItem(const QListWidgetItem *item) const;

    /**
     * @brief Set the configurable element to the item passed in parameter
     */
    void setConfigurableElementToItem(CT_AbstractConfigurableElement *ce, QListWidgetItem *item) const;

    /**
     * @brief Set the configurable widget to the item passed in parameter
     */
    void setConfigurableWidgetToItem(CT_AbstractConfigurableWidget *cw, QListWidgetItem *item) const;

    /**
     * @brief Create item
     */
    QListWidgetItem* createItem(CT_AbstractConfigurableElement *ce) const;

    /**
     * @brief Copy the item passed in parameter (copy the element and the configuration widget)
     */
    QListWidgetItem* copyItem(QListWidgetItem *item) const;

    /**
     * @brief Save configuration of all elements selected
     * @return false if it was error when save one element
     */
    bool saveConfigurationOfAllElementsSelected();

    /**
     * @brief Update the selected element list defined in method "setElementsSelected"
     */
    bool updateSelectedElementContainerToModify();

    /**
     * @brief Returns true if the member "m_selectedContainerToModify" is not nullptr and contains element passed in parameter
     */
    bool selectedContainerIsSetAndContains(CT_AbstractConfigurableElement *el) const;

    /**
     * @brief Returns the item in available widget with a configurable element with unique name passed in parameter
     */
    QListWidgetItem* findItemInAvailableElementByUniqueName(const QString &uniqueName) const;

    void clearSelectedListWithoutSave();
    void cleanAvailableListOfElementsFromContainerToModify();
    void cleanItem(QListWidgetItem *item);
    void clearItem(QListWidgetItem *item);
    void clearAvailable();

private slots:
    /**
     * @brief Transfer a available element to selected element
     * @param item : the item to transfer
     */
    void transferFromAvailableToSelected(QListWidgetItem *item);

    /**
     * @brief Transfer a selected element to available element
     */
    void transferFromSelectedToAvailable(QListWidgetItem *item);

    /**
     * @brief Push button "Add all" to transfer all available element to selected element
     */
    void on_pushButtonAddAll_clicked();

    /**
     * @brief Push button "Remove all" to transfer all selected element to available element
     */
    void on_pushButtonRemoveAll_clicked();

    /**
     * @brief Push button "Remove" to transfer all "selected" selected element to available element
     */
    void on_pushButtonRemove_clicked();

    /**
     * @brief When current item in selected element list has changed
     */
    void on_listWidgetSelected_currentItemChanged(QListWidgetItem *current,QListWidgetItem *previous);

    /**
     * @brief Ask for showing help page
     */
    void on_buttonBox_helpRequested();

public slots:
    void accept();
signals:
    void showHelp();
};

#endif // CTG_CONFIGURABLEELEMENTSSELECTOR_H
