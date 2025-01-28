#ifndef CTG_INRESULTMODELPOSSIBILITIES_H
#define CTG_INRESULTMODELPOSSIBILITIES_H

#include "ct_model/inModel/manager/ct_inmodelstructuremanager.h"
#include "ct_model/inModel/tools/ct_instdresultmodelpossibility.h"

#include <QWidget>
#include <QStandardItemModel>

namespace Ui {
class CTG_InResultModelPossibilities;
}

/**
 * @brief This widget will construct a tree view that contains all possibilities for each input result model in the
 *        collection of the "input result model manager". Signals is emitted when the user select or unselect a possibility.
 */
class CTG_InResultModelPossibilities : public QWidget
{
    Q_OBJECT
    
public:
    explicit CTG_InResultModelPossibilities(QWidget *parent = 0);
    ~CTG_InResultModelPossibilities();

    /**
     * @brief Set the "input result model manager" to use to get the collection of input result model and their possibilities
     * @param manager : the input result model manager to use
     */
    void setInResultModelManager(const CT_InModelStructureManager* manager);

    /**
     * @brief If true set the widget read only so the user can not select/unselect a possibility.
     * @param enabled : to true if you want that this widget was read only, false otherwise
     */
    void setReadOnly(bool enabled);

    int resultCount();

    bool selectResultByRank(int rank);

private:
    Ui::CTG_InResultModelPossibilities          *ui;

    /**
     * @brief Manager of input result models
     */
    CT_InModelStructureManager*                 m_inputResultModelManager;

    /**
     * @brief To true when it must be read only
     */
    bool                                        m_readOnly;

    /**
     * @brief The model for the treeview
     */
    QStandardItemModel                          m_treeViewModel;

    /**
     * @brief Clear the treeview
     */
    void clearModel();

    /**
     * @brief Re-Construct the model for the treeview. It will clear the model before construct it.
     */
    void constructModel();

    /**
     * @brief Construct and set the header for the treeview
     */
    void constructHeader();

    /**
     * @brief Only update the model (check/uncheck checkbox)
     */
    void updateModel();

    /**
     * @brief Show the first selected result possibility or the first if exist
     */
    void showFirstSelectedOrFirstResultPossibility();

    /**
     * @brief Create all QStandardItem necessary to represent the input result model and his possibilities
     * @param resModel : the input result model to convert in a collection of QStandardItem
     * @return the collection of QStandardItem to add to the treeview model
     */
    QList<QStandardItem*> createItemsForResultModel(const CT_InAbstractResultModel* resModel) const;

    /**
     * @brief Call to select the possibility and emit a signal to display it
     * @param model : we must have the model too to make some default selection
     * @param possibility : the possibility to select
     */
    void enableResultPossibility(const CT_InAbstractResultModel* model, CT_InStdResultModelPossibility* possibility);

    /**
     * @brief Call to unselect the possibility and emit a signal to display none
     * @param possibility : the possibility to unselect
     */
    void disableResultPossibility(CT_InStdResultModelPossibility* possibility);

    /**
     * @brief Call to emit a signal to display the possibility
     * @param possibility : the possibility to display
     */
    void showResultPossibility(const CT_InStdResultModelPossibility* possibility);

    void selectChildByRank(const QStandardItem *parent, int rank);

    void clearSelection();

protected:
    /**
     * @brief Overrided to update the model when showEvent happen
     */
    void showEvent(QShowEvent* event) override;

signals:
    /**
     * @brief Called to change the current possibility displayed
     * @param possibility : nullptr if we want that nothing was displayed otherwise the possibility to display
     */
    void showInResultModelPossibility(const CT_InStdResultModelPossibility* possibility);

private slots:
    /**
     * @brief Called when the user do a single click on the treeview
     */
    void on_treeView_clicked(const QModelIndex &index);

    /**
     * @brief Called when the user do a double click on the treeview
     */
    void on_treeView_doubleClicked(const QModelIndex &index);
};

#endif // CTG_INRESULTMODELPOSSIBILITIES_H
