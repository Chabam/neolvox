#include "ctg_modelslinkconfigurationflowview.h"
#include "ui_ctg_modelslinkconfigurationflowview.h"

#include "tools/ct_modelflowdatamodel.h"
#include "tools/ct_modelflowdata.h"
#include "tools/ct_indatatypetooutdatatypeconverter.h"
#include "tools/ct_helpgraphicsitem.h"

#include "ct_model/inModel/ct_inresultmodelgroup.h"
#include "ct_model/inModel/abstract/ct_inabstractsingularitemmodel.h"
#include "ct_model/inModel/abstract/ct_inabstractitemattributemodel.h"
#include "ct_model/outModel/abstract/ct_outabstractsingularitemmodel.h"
#include "ct_model/outModel/abstract/ct_outabstractitemattributemodel.h"

#include <nodes/DataModelRegistry>
#include <nodes/Node>
#include <nodes/FlowView>
#include <nodes/FlowScene>
#include <nodes/ConnectionStyle>

#include "nodes/internal/PortType.hpp"

#include <QPushButton>
#include <QLabel>
#include <QGraphicsProxyWidget>
#include <QTreeWidget>
#include <QHeaderView>
#include <QScrollBar>
#include <QAction>
#include <QTimer>
#include <QMenu>
#include <QApplication>
#include <QKeyEvent>
#include <QtGlobal>
#include <QDesktopServices>

using QtNodes::FlowView;
using QtNodes::FlowScene;
using QtNodes::DataModelRegistry;
using QtNodes::Node;
using QtNodes::TypeConverter;
using QtNodes::Connection;
using QtNodes::ConnectionStyle;

#if defined(_WIN32) && defined(_MSC_VER) // Microsoft Visual Studio Compiler
#define TREE_WIDGET_ITEM_SPACING 21
#elif (defined(__linux__) || defined(_WIN32)) && defined(__GNUC__) // GNU Compiler (gcc,g++) for Linux, Unix, and MinGW (Windows)
#define TREE_WIDGET_ITEM_SPACING 21+2
#elif defined(__APPLE__) // Clang Compiler (Apple)
#define TREE_WIDGET_ITEM_SPACING 21+2
#endif

CTG_ModelsLinkConfigurationFlowView::CTG_ModelsLinkConfigurationFlowView(QUrl helpPath, QWidget *parent) :
    SuperClass(parent),
    ui(new Ui::CTG_ModelsLinkConfigurationFlowView),
    mInResultModelGroup(nullptr),
    mOutResultModelGroup(nullptr),
    mInModelPortType(PT_OUT),
    mInNode(nullptr),
    mOutNode(nullptr),
    mInTreeWidget(nullptr),
    mOutTreeWidget(nullptr),
    mRestoreConnectionOrCreatePreviewInProgress(true),
    mKeyboardShiftModifierEnabled(false),
    mInDestructor(false)
{
    ui->setupUi(this);

    mModelOfLastIndexClicked.fill(nullptr, 2);

    std::shared_ptr<DataModelRegistry> ret = std::make_shared<DataModelRegistry>();
    ret->registerModel<CT_ModelFlowDataModel>();

    mFlowScene = new FlowScene(ret);
    mFlowView = new FlowView(mFlowScene, this);


// replaced by QPushButton to allow onclick action
//    auto hgi = new CT_HelpGraphicsItem();
//    hgi->setPos(800, -100);
//    hgi->setToolTip(tr("Cette interface permet de faire correspondre à chaque donnée rechechée, une donnée disponible dans les données d'entrée.<br><br>Pour chaque donnée recherchée :<br>1. Séléctionner la donnée recherchée<br>2. Séléctionner la donnée disponible à lui attribuer<br><br>Cela crée un lien entre les deux éléments.<br><br><i>Les données recherchées en italique sont optionnelles.</i><br><br>Les compteurs à droite des données recherchées indiquent :<br>nombre d'éléments séléctionnés / nombre d'éléments nécessaires."));
//    mFlowScene->addItem(hgi);

    // used to garanty some display buffer at the left of in model box (for screen capture export)
    QLabel* lbEmpty = new QLabel(" ");
    lbEmpty->setStyleSheet("border: 0px; background-color: rgba(0,0,0, 0);");
    QGraphicsProxyWidget* itemLb = mFlowScene->addWidget(lbEmpty);
    itemLb->setPos(-20,-20);
    itemLb->setZValue(1);


    QPushButton* hgi = new QPushButton();
    QPixmap pixmapHgi(":/Icones/Icones/help.png");
    QIcon iconHgi = QIcon(pixmapHgi);
    hgi->setIcon(iconHgi);
    hgi->setIconSize(QSize(48, 48));
    hgi->setStyleSheet("border: 0px; background-color: rgba(0,0,0, 0);");
    QGraphicsProxyWidget* itemHgi = mFlowScene->addWidget(hgi);
    itemHgi->setPos(900, 0);
    itemHgi->setZValue(1);
    //hgi->setToolTip(tr("Cette interface permet de faire correspondre à chaque donnée rechechée, une donnée disponible dans les données d'entrée.<br><br>Pour chaque donnée recherchée :<br>1. Séléctionner la donnée recherchée<br>2. Séléctionner la donnée disponible à lui attribuer<br><br>Cela crée un lien entre les deux éléments.<br><br><i>Les données recherchées en italique sont optionnelles.</i><br><br>Les compteurs à droite des données recherchées indiquent :<br>nombre d'éléments séléctionnés / nombre d'éléments nécessaires."));

    // RM : Zoom buttons
    QPushButton* sizeUp = new QPushButton();
    QPushButton* sizeDown = new QPushButton();
    QPixmap pixmapUp(":/Icones/Icones/plus.png");
    QPixmap pixmapDown(":/Icones/Icones/minus.png");
    QIcon iconUp = QIcon(pixmapUp);
    QIcon iconDown = QIcon(pixmapDown);
    sizeUp->setIcon(iconUp);
    sizeDown->setIcon(iconDown);
    sizeUp->setIconSize(QSize(48, 48));
    sizeDown->setIconSize(QSize(48, 48));
//    sizeUp->setIconSize(pixmapUp.rect().size());
//    sizeDown->setIconSize(pixmapDown.rect().size());
    sizeUp->setStyleSheet("border: 0px; background-color: rgba(0,0,0, 0);");
    sizeDown->setStyleSheet("border: 0px; background-color: rgba(0,0,0, 0);");

    QGraphicsProxyWidget* itemUp = mFlowScene->addWidget(sizeUp);
    itemUp->setPos(900,50);
    itemUp->setZValue(1);
    QGraphicsProxyWidget* itemDown = mFlowScene->addWidget(sizeDown);
    itemDown->setPos(900,100);
    itemDown->setZValue(1);

    connect(sizeUp, &QPushButton::pressed, this, &CTG_ModelsLinkConfigurationFlowView::scaleUp);
    connect(sizeDown, &QPushButton::pressed, this, &CTG_ModelsLinkConfigurationFlowView::scaleDown);
    connect(hgi, &QPushButton::pressed, this, &CTG_ModelsLinkConfigurationFlowView::askForHelp);

    connect(mFlowScene, &FlowScene::connectionSelected, this, &CTG_ModelsLinkConfigurationFlowView::connectionSelected);
    connect(mFlowScene, &FlowScene::connectionDoubleClicked, this, &CTG_ModelsLinkConfigurationFlowView::connectionDoubleClicked);
    connect(mFlowScene, &FlowScene::nodeSelected, this, &CTG_ModelsLinkConfigurationFlowView::nodeSelected);

    connect(mFlowView, &FlowView::rubberBandChanged, this, &CTG_ModelsLinkConfigurationFlowView::rubberBandChanged);

    ui->verticalLayout->addWidget(mFlowView);

    // Nothing was already clicked
    mFirstStatePressed = 0;

    mhelpPath = helpPath;
}

void CTG_ModelsLinkConfigurationFlowView::scaleUp()
{
    mFlowView->scaleUp();
}

void CTG_ModelsLinkConfigurationFlowView::scaleDown()
{
    mFlowView->scaleDown();
}

void CTG_ModelsLinkConfigurationFlowView::askForHelp()
{
    qDebug() << mhelpPath;
    QDesktopServices::openUrl(mhelpPath);
}

CTG_ModelsLinkConfigurationFlowView::~CTG_ModelsLinkConfigurationFlowView()
{
    mInDestructor = true;

    mInTreeWidget->disconnect(this); // TODODISCONNECT
    mOutTreeWidget->disconnect(this); // TODODISCONNECT

    mFlowScene->disconnect(this);
    mFlowView->disconnect(this);

    disconnect(mConnectionDeletedSignalSlotQtConnection);

    for(QMetaObject::Connection c : mPossibilitiesSignalSlotQtConnection) {
        disconnect(c);
    }

    mPreviewConnections.clear();

    delete ui;
    delete mFlowScene;
}

bool CTG_ModelsLinkConfigurationFlowView::isReadOnly() const
{
    return mFlowView->isReadOnly();
}

void CTG_ModelsLinkConfigurationFlowView::setInResultModelPossibility(const CT_InStdResultModelPossibility* possibility)
{
    mInResultModelGroup = nullptr;
    mOutResultModelGroup = nullptr;

    if(possibility != nullptr)
    {
        mInResultModelGroup = dynamic_cast<CT_InResultModelGroup*>(possibility->inResultModel());
        mOutResultModelGroup = possibility->outModel();
    }

    changeInNodePortType(mInModelPortType);
}

void CTG_ModelsLinkConfigurationFlowView::setReadOnly(bool enabled)
{
    mFlowView->setReadOnly(enabled);
}

void CTG_ModelsLinkConfigurationFlowView::fitViewToScene()
{
    auto items = mFlowScene->items();
    QRectF br;
    for(auto item : items)
    {
        br = br.united(item->mapToScene(item->boundingRect()).boundingRect());
    }

    mFlowView->setSceneRect(br);
}

void CTG_ModelsLinkConfigurationFlowView::changeInNodePortType(CTG_PortType portType)
{
    reset();

    mInModelPortType = portType;

    if((mInResultModelGroup != nullptr)
            && (mOutResultModelGroup != nullptr))
    {
        construct();

        auto inputDataValidator = [this](const NodeDataModel* inNodeDataModel, std::shared_ptr<NodeData> outNodeData, PortIndex inPortIndex) -> bool {
            if(mInDestructor)
                return false;

            return doesAcceptInputDataForNodeDataModel(inNodeDataModel, outNodeData, inPortIndex);
        };

        auto inputDataSetter = [this](NodeDataModel* inNodeDataModel, const Connection* connection, std::shared_ptr<NodeData> outNodeData, PortIndex inPortIndex) -> void {
            if(mInDestructor)
                return;

            setInputDataForNodeDataModel(inNodeDataModel, connection, outNodeData, inPortIndex);
        };

        std::unique_ptr<NodeDataModel> inType = std::make_unique<CT_ModelFlowDataModel>(tr("Données recherchées"), mInTreeWidget->model(), PortType(mInModelPortType), mInTreeWidget);

        if(mInModelPortType == PT_IN)
        {
            inType->setInDataValidator(inputDataValidator);
            inType->setInDataSetter(inputDataSetter);
        }

        mInNode = &mFlowScene->createNode(std::move(inType));
        mInNode->nodeGraphicsObject().setPos(mInModelPortType == PT_OUT ? 0 : 450, 0);
        mInNode->nodeGraphicsObject().setRef(true);
        mFlowScene->nodePlaced(*mInNode);

        std::unique_ptr<NodeDataModel> outType = std::make_unique<CT_ModelFlowDataModel>(tr("Données disponibles"), mOutTreeWidget->model(), oppositePort(PortType(mInModelPortType)), mOutTreeWidget);

        if(mInModelPortType == PT_OUT)
        {
            outType->setInDataValidator(inputDataValidator);
            outType->setInDataSetter(inputDataSetter);
        }

        mOutNode = &mFlowScene->createNode(std::move(outType));
        mOutNode->nodeGraphicsObject().setPos(mInModelPortType == PT_IN ? 0 : 450, 0);
        mOutNode->nodeGraphicsObject().setRef(false);
        mFlowScene->nodePlaced(*mOutNode);

        createConnectionForSelectedPossibilities();

        fitViewToScene();
    }
}

CTG_ModelsLinkConfigurationFlowView::CTG_PortType CTG_ModelsLinkConfigurationFlowView::inNodePortType() const
{
    return mInModelPortType;
}

int CTG_ModelsLinkConfigurationFlowView::getContentHeight()
{
    if (mInNode == nullptr) {return -1;}

    fitViewToScene();

    if (mInNode->nodeGraphicsObject().pos().y() < 0)
    {
          return mcontentHeight - mInNode->nodeGraphicsObject().pos().y();
    }

    return mcontentHeight;
}

void CTG_ModelsLinkConfigurationFlowView::reset()
{
    mModelOfLastIndexClicked.fill(nullptr);

    mConnectionByPossibility.clear();
    mInModelByPossibility.clear();

    disconnect(mConnectionDeletedSignalSlotQtConnection);

    for(QMetaObject::Connection c : mPossibilitiesSignalSlotQtConnection) {
        disconnect(c);
    }

    mPreviewConnections.clear();

    if(mInNode != nullptr)
        mFlowScene->removeNode(*mInNode);

    if(mOutNode != nullptr)
        mFlowScene->removeNode(*mOutNode);

    mInTreeWidget = nullptr;
    mOutTreeWidget = nullptr;

    mInNode = nullptr;
    mOutNode = nullptr;

    mConnectionDeletedSignalSlotQtConnection = connect(mFlowScene, &FlowScene::connectionDeleted, this, &CTG_ModelsLinkConfigurationFlowView::unselectPossibilityWhenConnectionIsDeleted);
}

void CTG_ModelsLinkConfigurationFlowView::construct()
{
    mModelOfLastIndexClicked.fill(nullptr);

    mInTreeWidget = new QTreeWidget;
    mInTreeWidget->setColumnCount(2);
    mInTreeWidget->setHeaderHidden(true);
    mInTreeWidget->setItemDelegate(new RowDelegate());
    mInTreeWidget->setStyleSheet(QString("QTreeWidget                {background-color: rgba(0,0,0, 0); border: 0px; color: white}"
                                         "QTreeWidget::item          {background-color: rgba(0,0,0, 0); border: 0px; color: white}"
                                         "QTreeWidget::item:selected {background-color: rgba(0,0,0,80);}"
                                         "QTreeWidget::item:hover    {background-color: rgba(0,0,0,40);}"));
    mInTreeWidget->setWindowFlags(Qt::FramelessWindowHint);
    mInTreeWidget->setAttribute(Qt::WA_NoSystemBackground);
    mInTreeWidget->setAttribute(Qt::WA_TranslucentBackground);
    mInTreeWidget->setAttribute(Qt::WA_TransparentForMouseEvents);
    mInTreeWidget->setItemsExpandable(false);
    //mInTreeWidget->setItemsExpandable(true);
    mInTreeWidget->header()->setStretchLastSection(false);

    mOutTreeWidget = new QTreeWidget;
    mOutTreeWidget->setColumnCount(2);
    mOutTreeWidget->setHeaderHidden(true);
    mOutTreeWidget->setItemDelegate(new RowDelegate());
    mOutTreeWidget->setStyleSheet(QString("QTreeWidget                {background-color: rgba(0,0,0, 0); border: 0px; color: white}"
                                          "QTreeWidget::item          {background-color: rgba(0,0,0, 0); border: 0px; color: white}"
                                          "QTreeWidget::item:selected {background-color: rgba(0,0,0,80);}"
                                          "QTreeWidget::item:hover    {background-color: rgba(0,0,0,40);}"));
    mOutTreeWidget->setWindowFlags(Qt::FramelessWindowHint);
    mOutTreeWidget->setAttribute(Qt::WA_NoSystemBackground);
    mOutTreeWidget->setAttribute(Qt::WA_TranslucentBackground);
    mOutTreeWidget->setAttribute(Qt::WA_TransparentForMouseEvents);
    mOutTreeWidget->setItemsExpandable(false);
    //mOutTreeWidget->setItemsExpandable(true);
    mOutTreeWidget->header()->setStretchLastSection(false);
    //mOutTreeWidget->horizontalScrollBar()->setStyleSheet("height:0px;");
    //mOutTreeWidget->verticalScrollBar()->setStyleSheet("width:0px;");

    QHash<const CT_AbstractModel*, QTreeWidgetItem*> inItems;
    QHash<const CT_AbstractModel*, QTreeWidgetItem*> outItems;

    mInResultModelGroup->recursiveVisitInChildrens([this, &inItems, &outItems] (const CT_InAbstractModel* child)-> bool {
        const CT_InAbstractSingularItemModel* inItemModel = dynamic_cast<const CT_InAbstractSingularItemModel*>(child);
        const CT_InAbstractItemAttributeModel* inItemAttributeModel = dynamic_cast<const CT_InAbstractItemAttributeModel*>(child);
        const QString extraName = (inItemModel != nullptr) ? inItemModel->itemNameFromType() : ((inItemAttributeModel != nullptr) ? inItemAttributeModel->valueTypeToString() : QString());
        const QString displayableName = child->displayableName();
        const QString displayableType = extraName.isEmpty() ? QString() : tr(" (%1)").arg(extraName);

        // RM : remove the first useless root group line
        if(displayableName == "*")
            return true;

        QTreeWidgetItem* inTreeitem = new QTreeWidgetItem(inItems.value(child->parentModel(), nullptr), QStringList(displayableName));
        inTreeitem->setData(0, Qt::UserRole, QVariant::fromValue(static_cast<void*>(const_cast<CT_InAbstractModel*>(child))));
        inTreeitem->setData(1, Qt::UserRole, QVariant::fromValue(static_cast<void*>(const_cast<CT_InAbstractModel*>(child))));
        int min_connection = child->minimumNumberOfPossibilityToSelect();
        int max_connection = child->maximumNumberOfPossibilityThatCanBeSelected();
        if(max_connection == -1)
            inTreeitem->setText(1, "0/"+QString().setNum(min_connection)+"+");
        else {
            if(min_connection == max_connection)
                inTreeitem->setText(1, "0/"+QString().setNum(min_connection));
            else
                inTreeitem->setText(1, "0/"+QString().setNum(min_connection)+"-"+QString().setNum(max_connection));
        }
        inTreeitem->setData(0, Qt::UserRole+1, QString().setNum(inItems.size()*100));
        inTreeitem->setData(1, Qt::UserRole+1, QString().setNum(inItems.size()*100));
        inTreeitem->setToolTip(0, tr("%1%2: %3\nDescription:\n%4").arg(displayableName, displayableType, child->shortDescription(), child->detailledDescription()));

        const bool obligatory = child->parentModel() == nullptr ? child->isObligatory() : child->recursiveAtLeastOneChildrenOrThisIsObligatory();

        QFont font = inTreeitem->font(0);
        font.setItalic(!obligatory);
        inTreeitem->setFont(0, font);
        inTreeitem->setFont(1, font);

        if(mInTreeWidget->topLevelItem(0) == nullptr)
            mInTreeWidget->addTopLevelItem(inTreeitem);

        inItems.insert(child, inTreeitem);

        child->visitPossibilities([this, &inTreeitem, &outItems](const CT_InAbstractModel* inModel, const CT_InStdModelPossibility* possibility) -> bool {

            NodeDataType inDataType;
            NodeDataType outDataType;

            if(mInModelPortType == PT_OUT)
            {
                inDataType = CT_ModelFlowData(inModel).type();
                outDataType = CT_ModelFlowData(possibility->outModel()).type();
            }
            else
            {
                outDataType = CT_ModelFlowData(inModel).type();
                inDataType = CT_ModelFlowData(possibility->outModel()).type();
            }

            QTreeWidgetItem* outTreeItem = createOrGetOutTreeItemForModel(possibility->outModel(), outItems);
            outTreeItem->setData(0, Qt::UserRole+1, inTreeitem->data(0, Qt::UserRole+1));

            mFlowScene->registry().registerTypeConverter(std::make_pair(inDataType, outDataType),
                                                         TypeConverter{CT_InDataTypeToOutDataTypeConverter()});

            mInModelByPossibility.insert(possibility, inModel);

            mPossibilitiesByModel.insert(inModel, possibility);
            mPossibilitiesByModel.insert(possibility->outModel(), possibility);

            mPossibilitiesSignalSlotQtConnection.append(connect(possibility, SIGNAL(selectStateChange(bool)), this, SLOT(createOrDeleteConnectionWhenPossibilitySelectionStateHasChanged(bool))));

            return true;
        });

        return true;
    });

    mInTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    mOutTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    mInTreeWidget->expandAll();
    mOutTreeWidget->expandAll();

    resizeColumnsToContents();

    int InWidth  = mInTreeWidget->columnWidth(0)  + mInTreeWidget->columnWidth(1)  + 50;
    int OutWidth = mOutTreeWidget->columnWidth(0) + mOutTreeWidget->columnWidth(1) + 50;

    mInTreeWidget->resize(InWidth, inItems.size() * TREE_WIDGET_ITEM_SPACING + 10);
    mOutTreeWidget->resize(OutWidth, outItems.size() * TREE_WIDGET_ITEM_SPACING + 10);

    mcontentHeight = std::max(inItems.size() * TREE_WIDGET_ITEM_SPACING + 10, outItems.size() * TREE_WIDGET_ITEM_SPACING + 10);

    connect(mInTreeWidget, &QTreeWidget::pressed, this, &CTG_ModelsLinkConfigurationFlowView::displayPreviewConnectionsForIndexClicked);
    connect(mOutTreeWidget, &QTreeWidget::pressed, this, &CTG_ModelsLinkConfigurationFlowView::displayPreviewConnectionsForIndexClicked);

    connect(mInTreeWidget, &QTreeWidget::entered, [this](const QModelIndex& index) { convertPreviewConnectionBetweenPreviousIndexClickedAndIndexEnteredToConnection(index, false); });
    connect(mOutTreeWidget, &QTreeWidget::entered, [this](const QModelIndex& index) { convertPreviewConnectionBetweenPreviousIndexClickedAndIndexEnteredToConnection(index, true); });

    connect(mInTreeWidget, &QTreeWidget::customContextMenuRequested, this, &CTG_ModelsLinkConfigurationFlowView::treeWidgetContextMenuRequested);
    //connect(mOutTreeWidget, &QTreeWidget::customContextMenuRequested, this, &CTG_ModelsLinkConfigurationFlowView::treeWidgetContextMenuRequested);
}

QTreeWidgetItem* CTG_ModelsLinkConfigurationFlowView::createOrGetOutTreeItemForModel(CT_AbstractModel* model, QHash<const CT_AbstractModel*, QTreeWidgetItem*>& outItems)
{
    QTreeWidgetItem* outTreeItem = outItems.value(model, nullptr);

    if(outTreeItem != nullptr)
        return outTreeItem;

    QTreeWidgetItem* parentOutTreeItem = nullptr;

    const bool isRootGroup = (model->parentModel()->parentModel() == nullptr);

    // get or create parentOutTreeItem only if this model is not the root group
    if(!isRootGroup)
    {
        parentOutTreeItem = outItems.value(model->parentModel(), nullptr);

        if(parentOutTreeItem == nullptr)
            parentOutTreeItem = createOrGetOutTreeItemForModel(model->parentModel(), outItems);
    }

    const CT_OutAbstractSingularItemModel* outItemModel = dynamic_cast<const CT_OutAbstractSingularItemModel*>(model);
    const CT_OutAbstractItemAttributeModel* outItemAttributeModel = dynamic_cast<const CT_OutAbstractItemAttributeModel*>(model);
    const QString extraName = ((outItemModel != nullptr) && (outItemModel->itemDrawable() != nullptr)) ? outItemModel->itemDrawable()->itemToolForModel()->nameFromType() : (((outItemAttributeModel != nullptr) && (outItemAttributeModel->itemAttribute() != nullptr)) ? outItemAttributeModel->itemAttribute()->itemAttributeToolForModel()->valueTypeToString() : QString());
    const QString displayableName = model->displayableName();
    const QString displayableType = extraName.isEmpty() ? QString() : tr(" (%1)").arg(extraName);

    outTreeItem = new QTreeWidgetItem(parentOutTreeItem, QStringList(displayableName));
    outTreeItem->setForeground(0, Qt::white);
    outTreeItem->setData(0, Qt::UserRole, QVariant::fromValue(static_cast<void*>(model)));
    outTreeItem->setData(0, Qt::UserRole+1, QString());
    outTreeItem->setToolTip(0, tr("%1%2: %3\nDescription:\n%4").arg(displayableName, displayableType, model->shortDescription(), model->detailledDescription()));

    if((mOutTreeWidget->topLevelItem(0) == nullptr) || (parentOutTreeItem == nullptr))
        mOutTreeWidget->addTopLevelItem(outTreeItem);

    outItems.insert(model, outTreeItem);

    return outTreeItem;
}

void CTG_ModelsLinkConfigurationFlowView::createConnectionForSelectedPossibilities()
{
    mRestoreConnectionOrCreatePreviewInProgress = true;

    QHashIterator<const CT_InStdModelPossibility*, const CT_InAbstractModel*> it(mInModelByPossibility);

    while(it.hasNext())
    {
        it.next();

        const CT_InStdModelPossibility* possibility = it.key();

        if(possibility->isSelected() && !mConnectionByPossibility.contains(possibility))
            createOrDeleteConnectionWhenPossibilitySelectionStateHasChanged(possibility, true);
    }

    mRestoreConnectionOrCreatePreviewInProgress = false;
}

std::shared_ptr<QtNodes::Connection> CTG_ModelsLinkConfigurationFlowView::createConnection(const int& inPortIndex, const int& outPortIndex, const bool& isAPreview, void* data)
{
    return (mInModelPortType == PT_IN) ? mFlowScene->createConnection(*mInNode, inPortIndex, *mOutNode, outPortIndex, TypeConverter{CT_InDataTypeToOutDataTypeConverter()}, isAPreview, data) : mFlowScene->createConnection(*mOutNode, outPortIndex, *mInNode, inPortIndex, TypeConverter{CT_InDataTypeToOutDataTypeConverter()}, isAPreview, data);
}

bool portIndexLessThan(const QtNodes::Connection* connection1, const QtNodes::Connection* connection2)
{
    return connection1->getPortIndex(PortType::In) < connection2->getPortIndex(PortType::In);
}

void CTG_ModelsLinkConfigurationFlowView::convertPreviewConnectionsSelectedToConnections()
{
    QMutableHashIterator<QtNodes::Connection*, const CT_InStdModelPossibility*> it(mPreviewConnections);

    const CT_InAbstractModel* inModel = nullptr;

    it.toFront();
    if(it.hasNext())
        inModel = mInModelByPossibility.value(it.next().value());
    else
        return;

    QList<const CT_InStdModelPossibility*> possibilitiesToSelect;
    const int maxToSelect = inModel->maximumNumberOfPossibilityThatCanBeSelected();

    QList<QtNodes::Connection*> connections;
    it.toFront();
    while(it.hasNext())
    {
        it.next();
        connections.append(it.key());
    }

    std::sort(connections.begin(), connections.end(), portIndexLessThan);

    for(auto connection : connections)
    {
        it.toFront();
        while(it.hasNext())
        {
            it.next();
            if(it.key() == connection)
                break;
        }

        if(connection->getConnectionGraphicsObject().isSelected())
        {
            const CT_InStdModelPossibility* possibility = it.value();

            it.remove();
            mFlowScene->deleteConnection(*connection);
            possibilitiesToSelect.append(possibility);
        }

        if((maxToSelect != -1) && (possibilitiesToSelect.size() >= maxToSelect))
            break;
    }

    for(const CT_InStdModelPossibility* p : possibilitiesToSelect)
    {
        const_cast<CT_InStdModelPossibility*>(p)->setSelected(true);
    }

    displayPreviewConnectionsForModel(mModelOfLastIndexClicked[0]);
}

void CTG_ModelsLinkConfigurationFlowView::deletePreviewConnections()
{
    QHashIterator<QtNodes::Connection*, const CT_InStdModelPossibility*> it(mPreviewConnections);

    while(it.hasNext())
    {
        it.next();
        mFlowScene->deleteConnection(*it.key());
    }

    mPreviewConnections.clear();
}

bool CTG_ModelsLinkConfigurationFlowView::doesAcceptInputDataForNodeDataModel(const QtNodes::NodeDataModel* inNodeDataModel,
                                                                              std::shared_ptr<QtNodes::NodeData> outNodeData,
                                                                              int inPortIndex)
{
    const CT_InAbstractModel* inModel = nullptr;
    const CT_OutAbstractModel* outModel = nullptr;

    // get the out model at the end of the connection
    // and the in model at the other end of the connection
    resolveInModelAndOutModel(inNodeDataModel,
                              outNodeData,
                              inPortIndex,
                              inModel,
                              outModel);

    // only accept the input data if the in model has a possibility that use the out model
    return !inModel->visitPossibilities([&outModel](const CT_InAbstractModel*, const CT_InStdModelPossibility* possibility) -> bool {
        return possibility->outModel() != outModel;
    });
}

void CTG_ModelsLinkConfigurationFlowView::setInputDataForNodeDataModel(QtNodes::NodeDataModel* inNodeDataModel,
                                                                       const QtNodes::Connection* connection,
                                                                       std::shared_ptr<QtNodes::NodeData> outNodeData,
                                                                       int inPortIndex)
{
    static auto selectPossibilityAndParentFunc = [](const CT_InAbstractModel* inModel, const CT_OutAbstractModel* outModel, const CT_InStdModelPossibility* possibility) {

        // select the possibility
        const_cast<CT_InStdModelPossibility*>(possibility)->setSelected(true);

        // go up (recursively) to select parents
        CT_InAbstractModel* parentInModel = static_cast<CT_InAbstractModel*>(inModel->parentModel());

        int nP = 0;

        // if the current model has a parent model and the parent model has at least one possibility saved
        if(parentInModel != nullptr
                && (nP = parentInModel->nPossibilitySaved()) > 0)
        {
            for(int i=0; i<nP; ++i)
            {
                CT_InStdModelPossibility* parentPossibility = parentInModel->possibilitySavedAt(i);

                // if the out model used by the possibility if the parent of the current out model
                if(parentPossibility->outModel() == outModel->parentModel()) {
                    // we select it (this will call the method "createOrDeleteConnectionWhenPossibilitySelectionStateHasChanged"
                    // that will create a connection and this method will be called again until the out model has no parent)
                    parentPossibility->setSelected(true);
                    break;
                }
            }
        }
    };

    // if the connection is a preview we don't select the possibility
    if(connection->connectionState().isAPreview() || mRestoreConnectionOrCreatePreviewInProgress)
        return;

    CT_ModelFlowData* myOutNodeData = static_cast<CT_ModelFlowData*>(outNodeData.get());

    // happen when connection will be deleted (she propagate an invalid node data)
    if(myOutNodeData == nullptr)
        return;

    const CT_InAbstractModel* inModel = nullptr;
    const CT_OutAbstractModel* outModel = nullptr;

    // get the out model at the end of the connection
    // and the in model at the other end of the connection
    resolveInModelAndOutModel(inNodeDataModel,
                              outNodeData,
                              inPortIndex,
                              inModel,
                              outModel);

    if(connection->data() != nullptr)
    {
        CT_InStdModelPossibility* possibility = static_cast<CT_InStdModelPossibility*>(connection->data());

        if(possibility->outModel() != outModel)
        {
            possibility = findPossibilityBetweenInAndOutModels(inModel, outModel);
            Q_ASSERT(possibility != nullptr);
            if (possibility == nullptr) {qDebug() << "CTG_ModelsLinkConfigurationFlowView::setInputDataForNodeDataModel" << ", " <<  "possibility == nullptr"; return;}

            const_cast<QtNodes::Connection*>(connection)->setData(possibility);
            mConnectionByPossibility.insert(possibility, const_cast<QtNodes::Connection*>(connection));

        }

        selectPossibilityAndParentFunc(inModel, outModel, possibility);
        return;
    }

    // visit possibilities to find the possibility that use the out model
    inModel->visitPossibilities([this, &outModel, &connection](const CT_InAbstractModel* inModel, const CT_InStdModelPossibility* possibility) -> bool
    {
        // if it is this possibility
        if(possibility->outModel() == outModel)
        {
            // hold the connection that match this possibility for future use
            mConnectionByPossibility.insert(possibility, const_cast<Connection*>(connection));

            // select the possibility
            const_cast<CT_InStdModelPossibility*>(possibility)->setSelected(true);

            // go up (recursively) to select parents
            CT_InAbstractModel* parentInModel = static_cast<CT_InAbstractModel*>(inModel->parentModel());

            int nP = 0;

            // if the current model has a parent model and the parent model has at least one possibility saved
            if(parentInModel != nullptr
                    && (nP = parentInModel->nPossibilitySaved()) > 0)
            {
                for(int i=0; i<nP; ++i)
                {
                    CT_InStdModelPossibility* parentPossibility = parentInModel->possibilitySavedAt(i);

                    // if the out model used by the possibility if the parent of the current out model
                    if(parentPossibility->outModel() == outModel->parentModel()) {
                        // we select it (this will call the method "createOrDeleteConnectionWhenPossibilitySelectionStateHasChanged"
                        // that will create a connection and this method will be called again until the out model has no parent)
                        parentPossibility->setSelected(true);
                        break;
                    }
                }
            }

            return false;
        }

        return true;
    });
}

void getItemFromPortNumberScan(QTreeWidgetItem* currentItem, int &currentPortIndex, QTreeWidgetItem* &returnItem, PortIndex portIndex)
{
    if(currentPortIndex == portIndex)
        returnItem = currentItem;
    else
    {
        int childCount = currentItem->childCount();
        for (int i = 0; i < childCount; i++)
        {
            QTreeWidgetItem *item = currentItem->child(i);
            currentPortIndex++;
            getItemFromPortNumberScan(item, currentPortIndex, returnItem, portIndex);
            if(currentPortIndex >= portIndex)
                break;
        }
    }
}

QTreeWidgetItem* getItemFromPortNumber(QTreeWidget* treeWidget, PortIndex portIndex)
{
    QTreeWidgetItem* root = treeWidget->topLevelItem(0);
    QTreeWidgetItem* returnItem = nullptr;
    int currentPort = 0;

    getItemFromPortNumberScan(root, currentPort, returnItem, portIndex);

    return returnItem;
}

void CTG_ModelsLinkConfigurationFlowView::createOrDeleteConnectionWhenPossibilitySelectionStateHasChanged(const CT_InStdModelPossibility* possibility, bool state)
{
    Connection* c = mConnectionByPossibility.value(possibility, nullptr);

    if((c == nullptr) && (state == true))
    {
        PortIndex inPortIndex = static_cast<CT_ModelFlowDataModel*>(mInNode->nodeDataModel())->portIndexOfModel(mInModelByPossibility.value(possibility, nullptr));
        PortIndex outPortIndex = static_cast<CT_ModelFlowDataModel*>(mOutNode->nodeDataModel())->portIndexOfModel(possibility->outModel());

        auto sharedC = createConnection(inPortIndex, outPortIndex, false, static_cast<void*>(const_cast<CT_InStdModelPossibility*>(possibility)));
        mConnectionByPossibility.insert(possibility, sharedC.get());

        // RM : Find QTreeWidgetItem in mInTreeWidget and modify current count of connection (x/y)
        QTreeWidgetItem* inItem = getItemFromPortNumber(mInTreeWidget, inPortIndex);
        QStringList inItemFields = inItem->text(1).split("/");
        QString newText = QString::number(inItemFields[0].toInt() + 1) + "/" + inItemFields[1];
        inItem->setText(1, newText);

        // RM : When selection is done on an attribute, the upper parent (item) font must be bold
        QTreeWidgetItem* outItem = getItemFromPortNumber(mOutTreeWidget, outPortIndex);
        if(outItem->childCount() == 0)
        {
            // This is an attribute, then :
            QTreeWidgetItem* outItemParent = outItem->parent();
            QFont font = outItemParent->font(0);
            font.setBold(true);
            outItemParent->setFont(0, font);
            resizeColumnsToContents();
        }
    }
    else if((c != nullptr) && (state == false))
    {
        PortIndex inPortIndex = static_cast<CT_ModelFlowDataModel*>(mInNode->nodeDataModel())->portIndexOfModel(mInModelByPossibility.value(possibility, nullptr));
        PortIndex outPortIndex = static_cast<CT_ModelFlowDataModel*>(mOutNode->nodeDataModel())->portIndexOfModel(possibility->outModel());

        mConnectionByPossibility.remove(possibility);
        mFlowScene->deleteConnection(*c);

        // RM : Find QTreeWidgetItem in mInTreeWidget and modify current count of connection (x/y)
        QTreeWidgetItem* inItem = getItemFromPortNumber(mInTreeWidget, inPortIndex);
        QStringList inItemFields = inItem->text(1).split("/");
        QString newText = QString::number(inItemFields[0].toInt() - 1) + "/" + inItemFields[1];
        inItem->setText(1, newText);

        // RM : When selection is done on an attribute, the upper parent (item) font must be bold
        QTreeWidgetItem* outItem = getItemFromPortNumber(mOutTreeWidget, outPortIndex);
        if(outItem->childCount() == 0)
        {
            // This is an attribute, then :
            QTreeWidgetItem* outItemParent = outItem->parent();
            QFont font = outItemParent->font(0);
            font.setBold(false);
            outItemParent->setFont(0, font);
            resizeColumnsToContents();
        }
    }
}

bool CTG_ModelsLinkConfigurationFlowView::resolveInModelAndOutModel(const QtNodes::NodeDataModel* inNodeDataModel,
                                                                    std::shared_ptr<QtNodes::NodeData> outNodeData,
                                                                    int inPortIndex,
                                                                    const CT_InAbstractModel*& inModel,
                                                                    const CT_OutAbstractModel*& outModel)
{
    CT_ModelFlowData* myOutNodeData = static_cast<CT_ModelFlowData*>(outNodeData.get());

    // get the out model at the end of the connection
    if(mInModelPortType == PT_IN)
        outModel = dynamic_cast<const CT_OutAbstractModel*>(myOutNodeData->model());
    else if(myOutNodeData != nullptr)
        inModel = dynamic_cast<const CT_InAbstractModel*>(myOutNodeData->model());

    // get the in model at the other end of the connection
    std::shared_ptr<NodeData> inNodeData = inNodeDataModel->inData(inPortIndex);
    CT_ModelFlowData* myInNodeData = static_cast<CT_ModelFlowData*>(inNodeData.get());

    if(mInModelPortType == PT_IN)
        inModel = dynamic_cast<const CT_InAbstractModel*>(myInNodeData->model());
    else if(myInNodeData != nullptr)
        outModel = dynamic_cast<const CT_OutAbstractModel*>(myInNodeData->model());

    return (inModel != nullptr) && (outModel != nullptr);
}

void CTG_ModelsLinkConfigurationFlowView::displayPreviewConnectionsForModel(CT_AbstractModel* model)
{
    mRestoreConnectionOrCreatePreviewInProgress = true;

    deletePreviewConnections();

    const QList<const CT_InStdModelPossibility*> possibilities = mPossibilitiesByModel.values(model);

    for(const CT_InStdModelPossibility* possibility : possibilities)
    {
        if(!possibility->isSelected())
        {
            const PortIndex inPortIndex = static_cast<CT_ModelFlowDataModel*>(mInNode->nodeDataModel())->portIndexOfModel(mInModelByPossibility.value(possibility, nullptr));
            const PortIndex outPortIndex = static_cast<CT_ModelFlowDataModel*>(mOutNode->nodeDataModel())->portIndexOfModel(possibility->outModel());

            if(inPortIndex != -1 && outPortIndex != -1)
                mPreviewConnections.insert(createConnection(inPortIndex, outPortIndex, true, static_cast<void*>(const_cast<CT_InStdModelPossibility*>(possibility))).get(), possibility);
        }
    }

    mRestoreConnectionOrCreatePreviewInProgress = false;
}

CT_InStdModelPossibility* CTG_ModelsLinkConfigurationFlowView::findPossibilityBetweenInAndOutModels(const CT_AbstractModel* inOrOutModel1, const CT_AbstractModel* inOrOutModel2) const
{
    const QList<const CT_InStdModelPossibility*> p1 = mPossibilitiesByModel.values(inOrOutModel1);
    const QList<const CT_InStdModelPossibility*> p2 = mPossibilitiesByModel.values(inOrOutModel2);

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    const QSet<const CT_InStdModelPossibility*> pI = p1.toSet().intersect(p2.toSet());
#else
    const QSet<const CT_InStdModelPossibility*> pI = QSet<const CT_InStdModelPossibility*>(p1.begin(), p1.end()).intersect(QSet<const CT_InStdModelPossibility*>(p2.begin(), p2.end()));
#endif
    if(pI.isEmpty())
        return nullptr;

    return const_cast<CT_InStdModelPossibility*>(*pI.begin());
}

void CTG_ModelsLinkConfigurationFlowView::keyPressEvent(QKeyEvent* event)
{
    mKeyboardShiftModifierEnabled = event->modifiers().testFlag(Qt::ShiftModifier);
    SuperClass::keyPressEvent(event);
}

void CTG_ModelsLinkConfigurationFlowView::keyReleaseEvent(QKeyEvent* event)
{
    mKeyboardShiftModifierEnabled = event->modifiers().testFlag(Qt::ShiftModifier);
    SuperClass::keyReleaseEvent(event);
}

void CTG_ModelsLinkConfigurationFlowView::createOrDeleteConnectionWhenPossibilitySelectionStateHasChanged(bool state)
{
    const CT_InStdModelPossibility* possibility = static_cast<CT_InStdModelPossibility*>(sender());

    createOrDeleteConnectionWhenPossibilitySelectionStateHasChanged(possibility, state);
}

void CTG_ModelsLinkConfigurationFlowView::unselectPossibilityWhenConnectionIsDeleted(const QtNodes::Connection& c)
{
    if(c.connectionState().isAPreview())
        return;

    Node* inNode = c.getNode(PortType::In);
    Node* outNode = c.getNode(PortType::Out);
    PortIndex inPortIndex = c.getPortIndex(PortType::In);
    PortIndex outPortIndex = c.getPortIndex(PortType::Out);

    const CT_InAbstractModel* inModel = nullptr;
    const CT_OutAbstractModel* outModel = nullptr;

    // get the out model at the end of the connection
    // and the in model at the other end of the connection
    resolveInModelAndOutModel(inNode->nodeDataModel(),
                              outNode->nodeDataModel()->outData(outPortIndex),
                              inPortIndex,
                              inModel,
                              outModel);

    // unselect the possibility that use this out model
    inModel->visitPossibilities([this, &outModel](const CT_InAbstractModel*, const CT_InStdModelPossibility* possibility) -> bool
    {
        if(possibility->outModel() == outModel)
        {
            mConnectionByPossibility.remove(possibility);
            const_cast<CT_InStdModelPossibility*>(possibility)->setSelected(false);
            return false;
        }

        return true;
    });

    // unselect selected possibility and delete connection of childrens (if the out model of the child's possibility is a child of the current out model)
    inModel->visitInChildrens([this, &outModel](const CT_InAbstractModel* child) -> bool
    {
        CT_InAbstractModel::Possibilities childSelectedPossibilities = child->selectedPossibilities();

        for(CT_InStdModelPossibility* childSelectedPossibility : childSelectedPossibilities)
        {
            CT_OutAbstractModel* outChildModel = childSelectedPossibility->outModel();

            // check if the out model of this possibility is a child or the current out model
            bool outModelOfPossibilityIsAChildOfTheCurrentOutModel = false;

            auto visitor = [&outModelOfPossibilityIsAChildOfTheCurrentOutModel, &outChildModel](const CT_AbstractModel* m) -> bool {
                outModelOfPossibilityIsAChildOfTheCurrentOutModel = (m == outChildModel);
                return !outModelOfPossibilityIsAChildOfTheCurrentOutModel;
            };

            outModel->visitChildrens(visitor);

            // only delete the connection if it is a child of the current out model
            if(outModelOfPossibilityIsAChildOfTheCurrentOutModel)
            {
                Connection* c = mConnectionByPossibility.take(childSelectedPossibility);
                PortIndex inPortIndexChild = c->getPortIndex(PortType::Out); // Port is inverted in NodeEditor logic
                PortIndex outPortIndexChild = c->getPortIndex(PortType::In);
                childSelectedPossibility->setSelected(false);
                mFlowScene->deleteConnection(*c);

                // RM : Find QTreeWidgetItem in mInTreeWidget and modify current count of connection (x/y)
                QTreeWidgetItem* inItem = getItemFromPortNumber(mInTreeWidget, inPortIndexChild);
                QStringList inItemFields = inItem->text(1).split("/");
                QString newText = QString::number(inItemFields[0].toInt() - 1) + "/" + inItemFields[1];
                inItem->setText(1, newText);

                // RM : When selection is done on an attribute, the upper parent (item) font must be bold
                QTreeWidgetItem* outItem = getItemFromPortNumber(mOutTreeWidget, outPortIndexChild);
                if(outItem->childCount() == 0)
                {
                    // This is an attribute, then :
                    QTreeWidgetItem* outItemParent = outItem->parent();
                    QFont font = outItemParent->font(0);
                    font.setBold(false);
                    outItemParent->setFont(0, font);
                    resizeColumnsToContents();
                }
            }
        }

        return true;
    });

    displayPreviewConnectionsForModel(mModelOfLastIndexClicked[0]);
}

void CTG_ModelsLinkConfigurationFlowView::displayPreviewConnectionsForIndexClicked(const QModelIndex& index)
{
    if(isReadOnly())
        return;

    bool mouseButtonPressed = false;
    if(mKeyboardShiftModifierEnabled || (mouseButtonPressed = index.model() == mOutTreeWidget->model()/*(qApp->mouseButtons() == Qt::MiddleButton)*/)) {
        if(mFirstStatePressed != 0) mFirstStatePressed = 0;
        convertPreviewConnectionBetweenPreviousIndexClickedAndIndexEnteredToConnection(index, mouseButtonPressed);
        return;
    }

    if(index.model() == mInTreeWidget->model())
        mOutTreeWidget->setCurrentIndex(QModelIndex());
    else
        return;
        // RM : Only the InTreeWidget shall be interacting to select the OutTreeWidget possibilities
        //mInTreeWidget->setCurrentIndex(QModelIndex());

    CT_AbstractModel* model = static_cast<CT_AbstractModel*>(index.data(Qt::UserRole).value<void*>());
    mModelOfLastIndexClicked[1] = mModelOfLastIndexClicked[0];
    mModelOfLastIndexClicked[0] = model;

    displayPreviewConnectionsForModel(model);
}

void CTG_ModelsLinkConfigurationFlowView::convertPreviewConnectionBetweenPreviousIndexClickedAndIndexEnteredToConnection(const QModelIndex& index, bool mouseButtonPressed)
{
    if(isReadOnly() || (mModelOfLastIndexClicked[0] == nullptr) || (!mKeyboardShiftModifierEnabled && !mouseButtonPressed))
    {
        mFirstStatePressed = 0;
        return;
    }

    CT_AbstractModel* model = static_cast<CT_AbstractModel*>(index.data(Qt::UserRole).value<void*>());

    CT_InStdModelPossibility* possibility = findPossibilityBetweenInAndOutModels(model, mModelOfLastIndexClicked[0]);

    if(possibility != nullptr)
    {
        if(mFirstStatePressed == 0)
        {
            if(possibility->isSelected())
            {
                mFirstStatePressed = -1;
                possibility->setSelected(false);
            }
            else
            {
                mFirstStatePressed = 1;
                possibility->setSelected(true);
            }
        }
        else
        {
            if(mFirstStatePressed == 1)
                possibility->setSelected(true);
            else
                possibility->setSelected(false);
        }
    }
}

void CTG_ModelsLinkConfigurationFlowView::connectionSelected(QtNodes::Connection& c)
{
    if(c.connectionState().isAPreview()) {
        mFlowScene->deleteConnection(c);
        const CT_InStdModelPossibility* possibility = mPreviewConnections.take(&c);
        const_cast<CT_InStdModelPossibility*>(possibility)->setSelected(true);

        displayPreviewConnectionsForModel(mModelOfLastIndexClicked[0]);
    }
}

void CTG_ModelsLinkConfigurationFlowView::connectionDoubleClicked(QtNodes::Connection& c)
{
    if(!c.connectionState().isAPreview())
    {
        PortIndex inPortIndex = c.getPortIndex(PortType::Out); // Port is inverted in NodeEditor logic
        PortIndex outPortIndex = c.getPortIndex(PortType::In);

        mFlowScene->deleteConnection(c);

        // RM : Find QTreeWidgetItem in mInTreeWidget and modify current count of connection (x/y)
        QTreeWidgetItem* inItem = getItemFromPortNumber(mInTreeWidget, inPortIndex);
        QStringList inItemFields = inItem->text(1).split("/");
        QString newText = QString::number(inItemFields[0].toInt() - 1) + "/" + inItemFields[1];
        inItem->setText(1, newText);

        // RM : When selection is done on an attribute, the upper parent (item) font must be bold
        QTreeWidgetItem* outItem = getItemFromPortNumber(mOutTreeWidget, outPortIndex);
        if(outItem->childCount() == 0)
        {
            // This is an attribute, then :
            QTreeWidgetItem* outItemParent = outItem->parent();
            QFont font = outItemParent->font(0);
            font.setBold(false);
            outItemParent->setFont(0, font);
            resizeColumnsToContents();
        }
    }
}

void CTG_ModelsLinkConfigurationFlowView::nodeSelected(QtNodes::Node& n)
{
    Q_UNUSED(n);

    mInTreeWidget->clearSelection();
    mOutTreeWidget->clearSelection();

    mOutTreeWidget->setCurrentIndex(QModelIndex());
    mInTreeWidget->setCurrentIndex(QModelIndex());

    deletePreviewConnections();

    mModelOfLastIndexClicked.fill(nullptr);
}

void CTG_ModelsLinkConfigurationFlowView::rubberBandChanged(QRect rubberBandRect, QPointF fromScenePoint, QPointF toScenePoint)
{
    // all is null when rubberband selection ends
    if(rubberBandRect.isNull() && fromScenePoint.isNull() && toScenePoint.isNull())
        convertPreviewConnectionsSelectedToConnections();
}

void CTG_ModelsLinkConfigurationFlowView::treeWidgetContextMenuRequested(const QPoint& p)
{
    if(isReadOnly())
        return;

    QTreeWidget* tw = static_cast<QTreeWidget*>(sender());

    QTreeWidgetItem* it = tw->itemAt(p);

    CT_AbstractModel* model = static_cast<CT_AbstractModel*>(tw->currentIndex().data(Qt::UserRole).value<void*>());
    mModelOfLastIndexClicked[1] = mModelOfLastIndexClicked[0];
    mModelOfLastIndexClicked[0] = model;

    if(it == nullptr)
        return;

    displayPreviewConnectionsForIndexClicked(tw->currentIndex());

    QMenu menu(this);
    menu.addAction(tr("Tout sélectionner sur ce niveau"), this, &CTG_ModelsLinkConfigurationFlowView::convertAllPreviewConnectionsToConnections);
    menu.addAction(tr("Tout dé-sélectionner sur ce niveau"), this, &CTG_ModelsLinkConfigurationFlowView::removeAllConnections);
    menu.exec(tw->mapToGlobal(p));
}

void CTG_ModelsLinkConfigurationFlowView::convertAllPreviewConnectionsToConnections()
{
    QHashIterator<QtNodes::Connection*, const CT_InStdModelPossibility*> it(mPreviewConnections);

    it.toFront();
    while(it.hasNext())
    {
        it.next();
        it.key()->getConnectionGraphicsObject().setSelected(true);
    }

    convertPreviewConnectionsSelectedToConnections();
}

void CTG_ModelsLinkConfigurationFlowView::removeAllConnections()
{
    if(mModelOfLastIndexClicked[0] != nullptr)
    {
        const QList<const CT_InStdModelPossibility*> possibilities = mPossibilitiesByModel.values(mModelOfLastIndexClicked[0]);

        for(const CT_InStdModelPossibility* possibility : possibilities)
        {
            if(possibility->isSelected())
            {
                Connection* c = mConnectionByPossibility.take(possibility);
                const_cast<CT_InStdModelPossibility*>(possibility)->setSelected(false);
                PortIndex inPortIndexChild = c->getPortIndex(PortType::Out); // Port is inverted in NodeEditor logic
                PortIndex outPortIndexChild = c->getPortIndex(PortType::In);
                mFlowScene->deleteConnection(*c);

                // RM : Find QTreeWidgetItem in mInTreeWidget and modify current count of connection (x/y)
                QTreeWidgetItem* inItem = getItemFromPortNumber(mInTreeWidget, inPortIndexChild);
                QStringList inItemFields = inItem->text(1).split("/");
                QString newText = QString::number(inItemFields[0].toInt() - 1) + "/" + inItemFields[1];
                inItem->setText(1, newText);

                // RM : When selection is done on an attribute, the upper parent (item) font must be bold
                QTreeWidgetItem* outItem = getItemFromPortNumber(mOutTreeWidget, outPortIndexChild);
                if(outItem->childCount() == 0)
                {
                    // This is an attribute, then :
                    QTreeWidgetItem* outItemParent = outItem->parent();
                    QFont font = outItemParent->font(0);
                    font.setBold(false);
                    outItemParent->setFont(0, font);
                    resizeColumnsToContents();
                }
            }
        }
    }
}

void CTG_ModelsLinkConfigurationFlowView::resizeColumnsToContents()
{
    mInTreeWidget->resizeColumnToContents(0);
    mOutTreeWidget->resizeColumnToContents(0);
    mInTreeWidget->resizeColumnToContents(1);
    mOutTreeWidget->resizeColumnToContents(1);
}


QSize RowDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    const QSize s = QStyledItemDelegate::sizeHint(option, index);

    const_cast<RowDelegate*>(this)->maxWidth = qMax(maxWidth, s.width());

    return QSize(s.width(), TREE_WIDGET_ITEM_SPACING);
}
