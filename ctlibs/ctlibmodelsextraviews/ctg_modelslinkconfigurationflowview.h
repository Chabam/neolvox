#ifndef CTG_MODELSLINKCONFIGURATIONFLOWVIEW_H
#define CTG_MODELSLINKCONFIGURATIONFLOWVIEW_H

#include "ctlibmodelsextraviews_global.h"

#include "ct_model/inModel/tools/ct_instdresultmodelpossibility.h"
#include "ct_model/inModel/views/iinmodelpossibilitieschoice.h"

#include <QWidget>
#include <QStyledItemDelegate>
#include <QUrl>

#include <memory>

namespace QtNodes {
    class FlowView;
    class FlowScene;
    class Node;
    class NodeData;
    class NodeDataModel;
    class Connection;
}

namespace Ui {
class CTG_ModelsLinkConfigurationFlowView;
}

class CT_AbstractModel;
class CT_InAbstractModel;
class CT_InResultModelGroup;
class QTreeWidget;
class QTreeWidgetItem;

/**
 * @brief Manage creation of nodes for one input result model possibility and manage the interaction
 *        with nodes and connections.
 *
 *        An input result model possibility is a link between a specific input result model of the
 *        current step and an output model of another step.
 *
 *        When you set an input result model possibility this class will construct two nodes :
 *
 *        - One node will display a tree view with input models of the current step
 *        - The second node will display a tree view with output models of the other step that can be
 *          linked with input models of the current step
 *
 *        Example :
 *
 *        NODE 1
 *          |-- In Result Model
 *                  |-- In Zero or more root group  (possibilities = RT1)
 *                          |-- In group            (possibilities = RT1)
 *                                  |-- In item     (possibilities = I1, I2 or I3)
 *
 *        NODE 2
 *          |-- Out result model
 *                  |-- Root group          (RT1)
 *                          |-- Out item    (I1)
 *                          |-- Out item    (I2)
 *                          |-- Out item    (I3)
 *                          |-- Out item    (I4 not displayed because it is not a possibility of an input model)
 *
 */
class CTLIBMODELSEXTRAVIEWS_EXPORT CTG_ModelsLinkConfigurationFlowView : public IInModelPossibilitiesChoice
{
    Q_OBJECT
    using SuperClass = IInModelPossibilitiesChoice;

public:
    enum CTG_PortType
    {
        PT_IN = 1,
        PT_OUT = 2
    };

    explicit CTG_ModelsLinkConfigurationFlowView(QUrl helpPath, QWidget *parent = nullptr);
    ~CTG_ModelsLinkConfigurationFlowView();

    bool isReadOnly() const final;

    void scaleUp();
    void scaleDown();
    void askForHelp();

public slots:
    /**
     * @brief Set which "input result model possibility" to display
     *
     * @param possibility : possibility to use to construct nodes
     */
    void setInResultModelPossibility(const CT_InStdResultModelPossibility* possibility) final;

    /**
     * @brief Set to true if you want that the user cannot change anything but just display nodes and connections
     */
    void setReadOnly(bool enabled) final;

    /**
     * @brief Call it to fit view to the scene bounding rect
     */
    void fitViewToScene();

public:
    /**
     * @brief Call it to change the port type for the node that display the input result model. This will reconstruct all tree views and nodes and connections.
     *
     * @param portType : PT_OUT if you want that the node that display the input result model show output connection port and is on the left, and
     *                   the node that display the output result model show input connection port and is on the right.
     *                   PT_IN if you want that the node that display the input result model show input connection port and is on the left, and
     *                   the node that display the output result model show output connection port and is on the right.
     */
    void changeInNodePortType(CTG_PortType portType);

    /**
     * @brief Returns the current port type used for the input model's node.
     */
    CTG_PortType inNodePortType() const;

    virtual int getContentHeight() override;

private:
    using ConnectionByPossibilityCollectionType = QHash<const CT_InStdModelPossibility*, QtNodes::Connection*>;
    using InModelByPossibilityCollectionType = QHash<const CT_InStdModelPossibility*, const CT_InAbstractModel*>;
    using PossibilitiesByModelCollectionType = QMultiHash<const CT_AbstractModel*, const CT_InStdModelPossibility*>;
    using PreviewConnectionsCollectionType = QHash<QtNodes::Connection*, const CT_InStdModelPossibility*>;

    Ui::CTG_ModelsLinkConfigurationFlowView*    ui;

    CT_InResultModelGroup*                      mInResultModelGroup;
    CT_OutAbstractModel*                        mOutResultModelGroup;

    CTG_PortType                                mInModelPortType;

    QtNodes::FlowScene*                         mFlowScene;
    QtNodes::FlowView*                          mFlowView;

    QtNodes::Node*                              mInNode;
    QtNodes::Node*                              mOutNode;

    QTreeWidget*                                mInTreeWidget;
    QTreeWidget*                                mOutTreeWidget;

    QMetaObject::Connection                     mConnectionDeletedSignalSlotQtConnection;
    QList<QMetaObject::Connection>              mPossibilitiesSignalSlotQtConnection;

    ConnectionByPossibilityCollectionType       mConnectionByPossibility;
    InModelByPossibilityCollectionType          mInModelByPossibility;
    PossibilitiesByModelCollectionType          mPossibilitiesByModel;
    PreviewConnectionsCollectionType            mPreviewConnections;

    bool                                        mRestoreConnectionOrCreatePreviewInProgress;

    QVector<CT_AbstractModel*>                  mModelOfLastIndexClicked;

    bool                                        mKeyboardShiftModifierEnabled;

    int                                         mFirstStatePressed;

    bool                                        mInDestructor;

    QUrl                                        mhelpPath;

    int                                         mcontentHeight;

    /**
     * @brief Reset (clear all) before construct
     */
    void reset();

    /**
     * @brief Re-construct nodes, connections, tree views
     */
    void construct();

    /**
     * @brief Create or get the QTreeWidgetItem for an output model. If the parent of the model doesn't exist it will be created too and add to the
     *        collection.
     *
     * @param model : the output model to get the QTreeWidgetItem
     * @param outItems : the collection of created QTreeWidgetItem
     * @return A new or an existing QTreeWidgetItem.
     */
    QTreeWidgetItem* createOrGetOutTreeItemForModel(CT_AbstractModel* model, QHash<const CT_AbstractModel*, QTreeWidgetItem*>& outItems);

    /**
     * @brief Iterate over all possibilities in the collection "mInModelByPossibility" and create a connection if the possibility is selected. Call
     *        this method after you have constructed nodes to restore existing connections.
     */
    void createConnectionForSelectedPossibilities();

    /**
     * @brief Create a connection between an input port and an output port.
     *
     * @param inPortIndex : the input port index
     * @param outPortIndex : the output port index
     * @param isAPreview ; true if the connection must be a preview connection.
     * @param data : a data if you want
     * @return The created connection.
     */
    std::shared_ptr<QtNodes::Connection> createConnection(const int& inPortIndex, const int& outPortIndex, const bool& isAPreview, void* data = nullptr);

    /**
     * @brief Get the input model for the first connection in the collection "mPreviewConnections". Iterate over all connections in the collection
     *        and if the connection is selected :
     *           - delete the connection
     *           - add the possibility that was linked to this connection to a list only if number of possibilities in the collection does not exceed the number
     *             of possibilities that the input model can have selected in same time.
     *
     *        For each possibility keeped select it.
     */
    void convertPreviewConnectionsSelectedToConnections();

    /**
     * @brief Delete all preview connections in the collection "mPreviewConnections" and clear the collection
     */
    void deletePreviewConnections();

    /**
     * @brief Returns true if the input model has a possibility that contains the output model
     */
    bool doesAcceptInputDataForNodeDataModel(const QtNodes::NodeDataModel* inNodeDataModel, std::shared_ptr<QtNodes::NodeData> outNodeData, int inPortIndex);

    /**
     * @brief Select the possibility that match with the connection (only if the connection is not a preview or if restore connection or create preview is not in progress)
     */
    void setInputDataForNodeDataModel(QtNodes::NodeDataModel* inNodeDataModel, const QtNodes::Connection* connection, std::shared_ptr<QtNodes::NodeData> outNodeData, int inPortIndex);

    /**
     * @brief If the possibility is selected (state == true) and a connection doesn't already exist : a connection is created fo this possibility. If
     *        the possibility is deslected (state == false) and a connection exist : the connection is deleted
     *
     * @param possibility : the possibility that was selected or deselected
     * @param state : true if selected, false if deselected
     */
    void createOrDeleteConnectionWhenPossibilitySelectionStateHasChanged(const CT_InStdModelPossibility* possibility, bool state);

    /**
     * @brief Get input model and output model function of "mInModelPortType" == PT_IN or PT_OUT
     */
    bool resolveInModelAndOutModel(const QtNodes::NodeDataModel* inNodeDataModel,
                                   std::shared_ptr<QtNodes::NodeData> outNodeData,
                                   int inPortIndex,
                                   const CT_InAbstractModel*& inModel,
                                   const CT_OutAbstractModel*& outModel);

    /**
     * @brief Delete previous preview connections, for each unselected possibility linked to the specified model : create a preview connection
     *        and add it to the collection "mPreviewConnections".
     *
     * @param model : it can be an input model or an output model or nullptr.
     */
    void displayPreviewConnectionsForModel(CT_AbstractModel* model);

    CT_InStdModelPossibility* findPossibilityBetweenInAndOutModels(const CT_AbstractModel* inOrOutModel1, const CT_AbstractModel* inOrOutModel2) const;

    void resizeColumnsToContents();

protected:
    void keyPressEvent(QKeyEvent *event) override;

    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    /**
     * @brief Connected to the signal "selectStateChange" of a possibility to create or delete a connection. Call the method
     *        "selectStateChange" that want two parameters.
     *
     * @param state : true if selected, false if unselected.
     */
    void createOrDeleteConnectionWhenPossibilitySelectionStateHasChanged(bool state);

    /**
     * @brief Connected to the signal "connectionDeleted" of the scene to unselect a possibility when a connection is
     *        deleted (only if the connection is not a preview).
     *
     * @param c : the connection that will be deleted
     */
    void unselectPossibilityWhenConnectionIsDeleted(QtNodes::Connection const& c);

    /**
     * @brief Connected to the signal "QTreeWidget::clicked" to get the input or output model from the specified index and call
     *        the method "displayPreviewConnectionsForModel"
     *
     * @param index : the index that contains the input or output model
     */
    void displayPreviewConnectionsForIndexClicked(const QModelIndex& index);

    /**
     * @brief Connected to the signal "QTreeWidget::entered" to convert the preview connection between the first clicked index
     *        and the index entered (if exist) if shift modifier is pressed.
     *
     * @param index : the index that is entered
     */
    void convertPreviewConnectionBetweenPreviousIndexClickedAndIndexEnteredToConnection(const QModelIndex& index, bool mouseButtonPressed = false);

    /**
     * @brief Connected to signal "FlowScene::connectionSelected" to convert the connection to a real connection if it is a preview.
     * @param c : the connection to convert if it is a preview
     */
    void connectionSelected(QtNodes::Connection& c);

    /**
     * @brief Connected to signal "FlowScene::connectionDoubleClicked" to delete the connection if it is not a preview.
     * @param c : the connection to delete if it is not a preview
     */
    void connectionDoubleClicked(QtNodes::Connection& c);

    /**
     * @brief Connected to signal "FlowScene::nodeSelected" to clear preview connections and selection in tree view of the two nodes.
     * @param c : the node selected (not used)
     */
    void nodeSelected(QtNodes::Node& n);

    /**
     * @brief Connected to signal "FlowView::rubberBandChanged" to call method "convertPreviewConnectionsSelectedToConnections" if
     *        rubberband selection ends.
     */
    void rubberBandChanged(QRect rubberBandRect, QPointF fromScenePoint, QPointF toScenePoint);

    /**
     * @brief Connected to signal "QTreeWidget::customContextMenuRequested" to display a context menu
     */
    void treeWidgetContextMenuRequested(const QPoint& p);

    /**
     * @brief Convert all preview connections displayed to real connections (select possibilities)
     */
    void convertAllPreviewConnectionsToConnections();

    /**
     * @brief Remove all connections really linked
     */
    void removeAllConnections();
};

/**
 * @brief A delegate to return a specific spacing between element in the tree view and to
 *        get the maximum width inside the tree view to adjust the size of the node that contains
 *        the tree view.
 */
class RowDelegate: public QStyledItemDelegate
{
public:
    RowDelegate(){ maxWidth = 0; }

    /**
     * @brief Maximum width inside the tree view
     */
    int maxWidth;

private:
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const final;
};

#endif // CTG_MODELSLINKCONFIGURATIONFLOWVIEW_H
