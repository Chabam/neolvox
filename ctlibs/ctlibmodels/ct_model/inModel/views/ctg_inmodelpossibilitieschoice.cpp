#include "ctg_inmodelpossibilitieschoice.h"
#include "ui_ctg_inmodelpossibilitieschoice.h"

#include "ct_model/inModel/ct_inresultmodelgroup.h"
#include "ct_model/inModel/ct_inzeroormoregroupmodel.h"
#include "ct_model/outModel/abstract/ct_outabstractitemattributemodel.h"

#include <QComboBox>
#include <QElapsedTimer>
#include <QPainter>
#include <QAction>

// CTG_InModelPossibilitiesChoiceItem //

void CTG_InModelCheckBox::setChecked(bool checked)
{
    // si il est sélectionnable et que l'état a changé
    if(isCheckable()
            && (isChecked() != checked))
    {
        // on change son état
        setCheckState(checked ? Qt::Checked : Qt::Unchecked);
    }
}

bool CTG_InModelCheckBox::isChecked() const
{
    return (checkState() == Qt::Checked);
}

bool CTG_InModelCheckBox::addInModel(const CT_InAbstractModel *model)
{
    Q_ASSERT(model != nullptr);
    if (model == nullptr) {qDebug() << "CTG_InModelCheckBox::addInModel" << ", " <<  "model == nullptr"; return false;}

    if(_outModel == nullptr)
        return false;

    bool ok = false;

    if(!containsInModel(model))
    {
        CT_InStdModelPossibility *p = staticSearchPossibilityForModel(model, _outModel);

        if(p != nullptr) {
            //qDebug() << "add " << model->displayableName() << " to out " << _outModel->displayableName();
            connect(p, SIGNAL(selectStateChange(bool)), this, SLOT(setChecked(bool)), Qt::DirectConnection);
            insertModel(model, p);
        }
    }

    return ok;
}

bool CTG_InModelCheckBox::setModelActive(const CT_InAbstractModel *model, bool active)
{
    Q_ASSERT(model != nullptr);
    if (model == nullptr) {qDebug() << "CTG_InModelCheckBox::setModelActive" << ", " <<  "model == nullptr"; return false;}

    if(active) {
        int index = m_inModelsAndPossibilityNotActivated.indexOf((CT_InAbstractModel*)model);

        if(index < 0)
            return false;

        m_inModelsAndPossibilityActivated.append(m_inModelsAndPossibilityNotActivated.takeAt(index));
        return true;
    }

    int index = m_inModelsAndPossibilityActivated.indexOf((CT_InAbstractModel*)model);

    if(index < 0)
        return false;

    m_inModelsAndPossibilityNotActivated.append(m_inModelsAndPossibilityActivated.takeAt(index));
    return true;
}

bool CTG_InModelCheckBox::isModelActive(const CT_InAbstractModel *model)
{
    return m_inModelsAndPossibilityActivated.contains((CT_InAbstractModel*)model);
}

void CTG_InModelCheckBox::clearInModels()
{
    clearInModels(m_inModelsAndPossibilityNotActivated);
    clearInModels(m_inModelsAndPossibilityActivated);
    clearInModels(m_dontChooseModelsAndPossibility);
}

bool CTG_InModelCheckBox::containsInModel(const CT_InAbstractModel *model) const
{
    if(model->maximumNumberOfPossibilityThatCanBeSelected() == 0)
        return m_dontChooseModelsAndPossibility.contains((CT_InAbstractModel*)model);

    if(m_inModelsAndPossibilityNotActivated.contains((CT_InAbstractModel*)model))
        return true;

    return m_inModelsAndPossibilityActivated.contains((CT_InAbstractModel*)model);
}

bool CTG_InModelCheckBox::hasInModelsActiveOrNot() const
{
    return !m_inModelsAndPossibilityNotActivated.isEmpty() || !m_inModelsAndPossibilityActivated.isEmpty();
}

bool CTG_InModelCheckBox::hasAtLeastOneInModel() const
{
    return hasInModelsActiveOrNot() || hasDontChooseInModels();
}

bool CTG_InModelCheckBox::hasInModelsActive() const
{
    return !m_inModelsAndPossibilityActivated.isEmpty();
}

bool CTG_InModelCheckBox::hasDontChooseInModels() const
{
    return !m_dontChooseModelsAndPossibility.isEmpty();
}

CT_InAbstractModel *CTG_InModelCheckBox::firstDontChooseInModel() const
{
    if(m_dontChooseModelsAndPossibility.isEmpty())
        return nullptr;

    //return m_dontChooseModelsAndPossibility.begin().key();
    return m_dontChooseModelsAndPossibility.first();
}

CT_InAbstractModel *CTG_InModelCheckBox::firstInModelActive() const
{
    if(m_inModelsAndPossibilityActivated.isEmpty())
        return nullptr;

    //return m_inModelsAndPossibility.begin().key();
    return m_inModelsAndPossibilityActivated.first();
}

CT_InAbstractModel *CTG_InModelCheckBox::checkedOrFirstInModelActiveOrFirstDontChooseInModel() const
{
    if(_outModel == nullptr)
        return nullptr;

    InternalModelContainerIterator it(m_inModelsAndPossibilityActivated);

    while(it.hasNext()) {
        CT_InAbstractModel *inModel = it.next();

        QList<CT_InStdModelPossibility*> l = inModel->getPossibilitiesSelected();
        QListIterator<CT_InStdModelPossibility*> itL(l);

        while(itL.hasNext()) {
            CT_InStdModelPossibility *p = itL.next();

            if(p->outModel() == _outModel)
                return inModel;
        }
    }

    CT_InAbstractModel *inModel = firstInModelActive();

    if(inModel == nullptr)
        return firstDontChooseInModel();

    return inModel;
}

CTG_InModelCheckBox::ModelContainer CTG_InModelCheckBox::getInModelsNotActive() const
{
    //return m_inModelsAndPossibility.keys();
    return m_inModelsAndPossibilityNotActivated;
}

CTG_InModelCheckBox::ModelContainer CTG_InModelCheckBox::getAllInModels() const
{
    CTG_InModelCheckBox::ModelContainer l;
    l.append(m_inModelsAndPossibilityNotActivated);
    l.append(m_inModelsAndPossibilityActivated);

    return l;
}

CTG_InModelCheckBox::ModelContainer CTG_InModelCheckBox::getInModelsActive() const
{
    return m_inModelsAndPossibilityActivated;
}

CTG_InModelCheckBox::ModelContainer CTG_InModelCheckBox::getDontChooseInModels() const
{
    //return m_dontChooseModelsAndPossibility.keys();
    return m_dontChooseModelsAndPossibility;
}

void CTG_InModelCheckBox::insertModel(const CT_InAbstractModel *model, CT_InStdModelPossibility *p)
{
    Q_UNUSED(p)

    // check if we must add this model in the "_dontChooseInModels" container or in the "_inModels" container
    if(model->maximumNumberOfPossibilityThatCanBeSelected() == 0)
        m_dontChooseModelsAndPossibility.append((CT_InAbstractModel*)model);
    else
        m_inModelsAndPossibilityActivated.append((CT_InAbstractModel*)model);
}

void CTG_InModelCheckBox::clearInModels(InternalModelContainer &container)
{
    CTG_InModelCheckBox::InternalModelContainerIterator it(container);

    while(it.hasNext())
    {
        CT_InAbstractModel *model = it.next();
        CT_InStdModelPossibility *p = getPossibilityForModel(model);
        disconnect(p, SIGNAL(selectStateChange(bool)), this, SLOT(setChecked(bool)));
    }

    container.clear();
}

CT_InStdModelPossibility *CTG_InModelCheckBox::getPossibilityForModel(const CT_InAbstractModel *model)
{
    return staticSearchPossibilityForModel(model, _outModel);
}

CT_InStdModelPossibility* CTG_InModelCheckBox::staticSearchPossibilityForModel(const CT_InAbstractModel *model, const CT_OutAbstractModel *outModel)
{
    Q_ASSERT(model != nullptr);
    if (model == nullptr) {qDebug() << "CTG_InModelCheckBox::staticSearchPossibilityForModel" << ", " <<  "model == nullptr"; return nullptr;}

    Q_ASSERT(outModel != nullptr);
    if (outModel == nullptr) {qDebug() << "CTG_InModelCheckBox::staticSearchPossibilityForModel" << ", " <<  "outModel == nullptr"; return nullptr;}

    CT_InStdModelPossibility* possibilityFound = nullptr;

    model->possibilitiesGroup()->visitPossibilities([&outModel, &possibilityFound](const CT_InStdModelPossibility* p) -> bool {
        if(p->outModel() != outModel)
            return true;

        // if this possibility match with the out model of this item
        possibilityFound = const_cast<CT_InStdModelPossibility*>(p); // we return the possibility
        return false;

    });

    return possibilityFound;
}

// CTG_InModelPossibilitiesChoiceItemCombo //

bool CTG_InModelComboBox::setPossibilityOfCurrentInModelChecked(bool checked)
{
    bool atLeastOneDontChooseSelected = false;

    // coche/decoche les DontChoose
    CTG_InModelCheckBox::ModelContainer models = m_checkBoxItem.getDontChooseInModels();
    CTG_InModelCheckBox::ModelContainerIterator it(models);

    while(it.hasNext())
    {
        CT_InStdModelPossibility *possibility = CTG_InModelCheckBox::staticSearchPossibilityForModel(it.next(), m_checkBoxItem._outModel);
        possibility->setSelected(checked);
        atLeastOneDontChooseSelected = true;
    }

    // coche la sélectionnée
    CT_InStdModelPossibility *possibility = getPossibilityOfCurrentInModel();

    if(possibility != nullptr)
        return possibility->setSelected(checked);

    return atLeastOneDontChooseSelected;
}

CT_InAbstractModel* CTG_InModelComboBox::currentInModel() const
{
    return _inModelSelected;
}

void CTG_InModelComboBox::setCurrentInModel(CT_InAbstractModel *model)
{
    if(_inModelSelected != model) {
        _inModelSelected = model;
        if(_inModelSelected != nullptr) {
            m_possibility = findPossibilityOfCurrentInModel();
            Q_ASSERT(m_possibility != nullptr);
            if (m_possibility == nullptr) {qDebug() << "CTG_InModelCheckBox::setCurrentInModel" << ", " <<  "m_possibility == nullptr"; return;}
        } else {
            m_possibility = nullptr;
        }
    }
}

void CTG_InModelComboBox::updateDisplayData()
{
    if(_inModelSelected != nullptr)
    {
        QString typeName = "";
        CT_InAbstractItemModel* itemModel = dynamic_cast<CT_InAbstractItemModel*>(_inModelSelected);
        if (itemModel != nullptr)
        {
            typeName = " [" + itemModel->itemNameFromType() + "]";
        }

        setData(QString("%1%2").arg(_inModelSelected->displayableName()).arg(typeName), Qt::DisplayRole);
    } else
    {
        setData("", Qt::DisplayRole);
    }
}

CT_InStdModelPossibility* CTG_InModelComboBox::getPossibilityOfCurrentInModel() const
{
    return m_possibility;
}

CT_InStdModelPossibility *CTG_InModelComboBox::findPossibilityOfCurrentInModel() const
{
    return CTG_InModelCheckBox::staticSearchPossibilityForModel(_inModelSelected, m_checkBoxItem._outModel);
}

// CTG_InModelPossibilitiesChoiceComboBoxDelegate //

CTG_InModelPossibilitiesChoiceComboBoxDelegate::CTG_InModelPossibilitiesChoiceComboBoxDelegate(QObject *parent): QItemDelegate(parent)
{
}

QWidget* CTG_InModelPossibilitiesChoiceComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex &/* index */) const
{
    return new QComboBox(parent);
}

void CTG_InModelPossibilitiesChoiceComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    const QStandardItemModel* model = (const QStandardItemModel*)index.model();

    CTG_InModelComboBox *comboBoxItem = dynamic_cast<CTG_InModelComboBox*>(model->itemFromIndex(index));
    CTG_InModelCheckBox *checkBoxItem = dynamic_cast<CTG_InModelCheckBox*>(model->itemFromIndex(model->sibling(index.row(), CTG_InModelPossibilitiesChoice::COLUMN_CHECK, index)));

    if((comboBoxItem != nullptr) && (checkBoxItem != nullptr))
    {
        QComboBox *comboBox = static_cast<QComboBox*>(editor);
        CTG_InModelCheckBox::ModelContainer inModelsActive = checkBoxItem->getInModelsActive();
        CTG_InModelCheckBox::ModelContainerIterator it(inModelsActive);

        while(it.hasNext()) {
            CT_InAbstractModel *inModel = it.next();

            QString typeName = "";
            CT_InAbstractItemModel* itemModel = dynamic_cast<CT_InAbstractItemModel*>(inModel);
            if (itemModel != nullptr)
            {
                typeName = " [" + itemModel->itemNameFromType() + "]";
            }

            comboBox->addItem(QString("%1%2").arg(inModel->displayableName()).arg(typeName), QVariant::fromValue((void*)inModel));
        }

        CT_InAbstractModel *selectedModel = comboBoxItem->currentInModel();
        int index = comboBox->findData(QVariant::fromValue((void*)selectedModel));

        Q_ASSERT(index != -1);
        if (index == -1) {qDebug() << "CTG_InModelPossibilitiesChoiceComboBoxDelegate::setEditorData" << ", " <<  "index == -1"; return;}

        comboBox->setCurrentIndex(index);

        connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxIndexChanged()), Qt::QueuedConnection);
    }
}

void CTG_InModelPossibilitiesChoiceComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    const QStandardItemModel* stdModel = (const QStandardItemModel*)index.model();

    CTG_InModelComboBox *comboBoxItem = dynamic_cast<CTG_InModelComboBox*>(stdModel->itemFromIndex(index));
    CTG_InModelCheckBox *checkBoxItem = dynamic_cast<CTG_InModelCheckBox*>(stdModel->itemFromIndex(model->sibling(index.row(), CTG_InModelPossibilitiesChoice::COLUMN_CHECK, index)));

    if((comboBoxItem != nullptr) && (checkBoxItem != nullptr))
    {
        QComboBox *comboBox = static_cast<QComboBox*>(editor);

        bool previousChecked = checkBoxItem->isChecked();

        // uncheck previous selected model
        comboBoxItem->setPossibilityOfCurrentInModelChecked(false);

        // set new selected model
        CT_InAbstractModel *newSelectedModel = (CT_InAbstractModel*)comboBox->currentData().value<void*>();
        comboBoxItem->setCurrentInModel(newSelectedModel);
        comboBoxItem->updateDisplayData();

        // check/uncheck the new selected model
        comboBoxItem->setPossibilityOfCurrentInModelChecked(previousChecked);

        const_cast<CTG_InModelPossibilitiesChoiceComboBoxDelegate*>(this)->closeEditor(editor, QAbstractItemDelegate::NoHint);
    }
}

void CTG_InModelPossibilitiesChoiceComboBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}

void CTG_InModelPossibilitiesChoiceComboBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const QStandardItemModel* model = (const QStandardItemModel*)index.model();

    CTG_InModelComboBox *comboBoxItem = dynamic_cast<CTG_InModelComboBox*>(model->itemFromIndex(index));

    if((comboBoxItem != nullptr) && (comboBoxItem->isEditable())) {
        QStyleOptionComboBox box;
        box.palette = option.palette;
        box.rect = option.rect;
        box.state = QStyle::State_Active;
        box.currentText = index.data().toString();
        painter->save();
        QApplication::style()->drawComplexControl(QStyle::CC_ComboBox,&box, painter);
        QApplication::style()->drawControl(QStyle::CE_ComboBoxLabel,&box, painter);
        painter->restore();
        return;
    }

    QItemDelegate::paint(painter, option, index);
}

void CTG_InModelPossibilitiesChoiceComboBoxDelegate::comboBoxIndexChanged()
{
    emit commitData((QWidget*)sender());
}

// CTG_InModelPossibilitiesChoice //

CTG_InModelPossibilitiesChoice::CTG_InModelPossibilitiesChoice(QWidget *parent) :
    SuperClass(parent),
    ui(new Ui::CTG_InModelPossibilitiesChoice)
{
    ui->setupUi(this);

    ui->treeView->setModel(&_viewModel);
    ui->treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->treeView->setItemDelegateForColumn(COLUMN_COMBO, new CTG_InModelPossibilitiesChoiceComboBoxDelegate(ui->treeView));
    ui->treeView->setEditTriggers(QAbstractItemView::AllEditTriggers);
    ui->treeView->setContextMenuPolicy(Qt::ActionsContextMenu);

    QAction* act = new QAction(tr("Select"), nullptr);
    connect(act, SIGNAL(triggered(bool)), this, SLOT(checkAllItemSelected()));
    ui->treeView->addAction(act);

    act = new QAction(tr("Unselect"), nullptr);
    connect(act, SIGNAL(triggered(bool)), this, SLOT(uncheckAllItemSelected()));
    ui->treeView->addAction(act);

    connect(&_viewModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(itemChanged(QStandardItem*)), Qt::DirectConnection);

    _possibility = nullptr;
    _itemChangedSlotIsEnabled = true;
    m_readOnly = false;

    constructHeader();
}

CTG_InModelPossibilitiesChoice::~CTG_InModelPossibilitiesChoice()
{
    delete ui;
}

bool CTG_InModelPossibilitiesChoice::isReadOnly() const
{
    return m_readOnly;
}

void CTG_InModelPossibilitiesChoice::setInResultModelPossibility(const CT_InStdResultModelPossibility *possibility)
{
    _possibility = const_cast<CT_InStdResultModelPossibility*>(possibility);

    constructModel();
}

void CTG_InModelPossibilitiesChoice::setReadOnly(bool enabled)
{
    m_readOnly = enabled;
}

void CTG_InModelPossibilitiesChoice::clearModel()
{
    _viewModel.invisibleRootItem()->removeRows(0, _viewModel.invisibleRootItem()->rowCount());
}

void CTG_InModelPossibilitiesChoice::constructModel()
{
    m_firstLoad = true;
    clearModel();
    constructHeader();

    if(_possibility != nullptr)
    {
        // on récupère le résultat modèle de sortie de cette possibilité
        CT_OutAbstractResultModelGroup* outResModel = dynamic_cast<CT_OutAbstractResultModelGroup*>(_possibility->outModel());
        CT_InResultModelGroup* inResModel = dynamic_cast<CT_InResultModelGroup*>(_possibility->inResultModel());


        if((outResModel != nullptr)
                && (inResModel != nullptr)
                && (inResModel->rootGroup() != nullptr))
        {
            // on construit l'arbre de ses modèles
            QList<QStandardItem*> items = createItemsForOutResultModel(outResModel);

            if(!items.isEmpty())
            {
                _itemChangedSlotIsEnabled = false;

                _viewModel.invisibleRootItem()->appendRow(items);

                // on va parcourir l'arbre pour définir quelle modele (et possibilité) est utilise par tel checkbox
                constructCheckBox();

                // on parcours pour supprimer les éléments non sélectionnable
                removeItemsThatCanNotBeChoosed();

                _itemChangedSlotIsEnabled = true;
            }

            ui->treeView->expandAll();
        }
    }

    m_firstLoad = false;
    setMinimumWidth(ui->treeView->header()->length());
}

void CTG_InModelPossibilitiesChoice::constructHeader()
{
//    QString stepName;

//    if(_possibility != nullptr)
//    {
//        stepName = _possibility->outModel()->step()->getStepCustomName();
//    }

    QStringList header;
    //header << (tr("Output") + (!stepName.isEmpty() ? (tr(" de ") + stepName) : ""));
    header << tr("Available data");
    header << tr("Sel.");
    header << tr("Searched Data");

    _viewModel.setHorizontalHeaderLabels(header);

#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
    ui->treeView->header()->setResizeMode(0, QHeaderView::ResizeToContents);
    ui->treeView->header()->setResizeMode(1, QHeaderView::ResizeToContents);
    ui->treeView->header()->setResizeMode(2, QHeaderView::ResizeToContents);
#else
    ui->treeView->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->treeView->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->treeView->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
#endif
}

QList<QStandardItem*> CTG_InModelPossibilitiesChoice::createItemsForOutResultModel(CT_OutAbstractResultModelGroup* resModel)
{
    QList<QStandardItem*> retList;

    // Nom du résultat modèle d'entrée
    QStandardItem *rootName = new QStandardItem(resModel->displayableName());
    rootName->setEditable(false);

    retList.append(rootName);

    // un item vide
    CTG_InModelCheckBox *emptyItem = new CTG_InModelCheckBox("");
    emptyItem->setEditable(false);
    emptyItem->setCheckable(false);
    retList.append(emptyItem);

    // un item vide
    CTG_InModelComboBox *emptyItem2 = new CTG_InModelComboBox(*emptyItem, "");
    emptyItem2->setEditable(false);
    retList.append(emptyItem2);

    recursiveCreateItemsForGroupModel(rootName,
                                      resModel->rootGroup());

    return retList;
}

void CTG_InModelPossibilitiesChoice::recursiveCreateItemsForGroupModel(QStandardItem* root,
                                                                       const DEF_CT_AbstractGroupModelOut* group)
{
    if(group == nullptr)
        return;

    const bool readOnly = m_readOnly;

    QList<QStandardItem*> list;

    // le nom du modèle de sortie
    QStandardItem *rootItem = new QStandardItem(group->displayableName());
    rootItem->setEditable(false);

    list.append(rootItem);

    // un item pour la sélection
    CTG_InModelCheckBox *selectableRootItem = new CTG_InModelCheckBox("");
    selectableRootItem->setEditable(false);
    selectableRootItem->setEnabled(!readOnly);
    selectableRootItem->_outModel = const_cast<DEF_CT_AbstractGroupModelOut*>(group);
    list.append(selectableRootItem);

    // un item pour la sélection du modèle d'entrée si il y en a plusieurs (combobox)
    CTG_InModelComboBox *comboBoxRootItem = new CTG_InModelComboBox(*selectableRootItem, "");
    comboBoxRootItem->setEditable(false);
    comboBoxRootItem->setEnabled(!readOnly);
    list.append(comboBoxRootItem);

    group->visitGroups([this, &rootItem](const DEF_CT_AbstractGroupModelOut* g) -> bool {
        this->recursiveCreateItemsForGroupModel(rootItem, g);
        return true;
    });

    group->visitItems([&readOnly, &rootItem](const CT_OutAbstractSingularItemModel* item) -> bool {

        QList<QStandardItem*> list2;

        IItemDrawableForModel* itemDrawable = item->itemDrawable();
        const QString itemType = (itemDrawable != nullptr) ? " [" + itemDrawable->itemToolForModel()->displayableName() + "]" : QString();

        // le nom du modèle de sortie
        QStandardItem* itemItem = new QStandardItem(QString("%1%2").arg(item->displayableName()).arg(itemType));
        itemItem->setEditable(false);
        list2.append(itemItem);

        // un item pour la sélection (case à cocher)
        CTG_InModelCheckBox* checkableItemItem = new CTG_InModelCheckBox("");
        checkableItemItem->setEditable(false);
        checkableItemItem->setEnabled(!readOnly);
        checkableItemItem->_outModel = const_cast<CT_OutAbstractSingularItemModel*>(item);
        list2.append(checkableItemItem);

        // un item pour la sélection du modèle d'entrée si il y en a plusieurs (combobox)
        CTG_InModelComboBox* comboBoxItemItem = new CTG_InModelComboBox(*checkableItemItem, "");
        comboBoxItemItem->setEditable(false);
        comboBoxItemItem->setEnabled(!readOnly);
        list2.append(comboBoxItemItem);

        item->visitAttributes([&readOnly, &itemItem](const CT_OutAbstractItemAttributeModel* itemAtt) -> bool {
            QList<QStandardItem*> list3;

            // le nom du modèle de sortie
            QStandardItem* itemItemAtt = new QStandardItem(itemAtt->displayableName());
            itemItemAtt->setEditable(false);
            list3.append(itemItemAtt);

            // un item pour la sélection (case à cocher)
            CTG_InModelCheckBox* checkableItemItemAtt = new CTG_InModelCheckBox("");
            checkableItemItemAtt->setEditable(false);
            checkableItemItemAtt->setEnabled(!readOnly);
            checkableItemItemAtt->_outModel = const_cast<CT_OutAbstractItemAttributeModel*>(itemAtt);
            list3.append(checkableItemItemAtt);

            // un item pour la sélection du modèle d'entrée si il y en a plusieurs (combobox)
            CTG_InModelComboBox* comboBoxItemItemAtt = new CTG_InModelComboBox(*checkableItemItemAtt, "");
            comboBoxItemItemAtt->setEditable(false);
            comboBoxItemItemAtt->setEnabled(!readOnly);
            list3.append(comboBoxItemItemAtt);

            itemItem->appendRow(list3);

            return true;
        });

        rootItem->appendRow(list2);

        return true;
    });

    root->appendRow(list);
}

void CTG_InModelPossibilitiesChoice::constructCheckBox()
{
    _itemChangedSlotIsEnabled = false;

    QStandardItem *parentOfItemsToTest = _viewModel.invisibleRootItem();

    recursiveConstructCheckBox(parentOfItemsToTest);

    int size = parentOfItemsToTest->rowCount();

    for(int i=0; i<size; ++i)
    {
        QStandardItem *child = parentOfItemsToTest->child(i, 0);

        int childSize = child->rowCount();

        // only for children of the result item
        for(int j=0; j<childSize; ++j)
        {
            QStandardItem *childJ = child->child(j, 0);

            // get the checkbox and combobox item to set in the combobox the selected model or the first model of the checkbox item
            CTG_InModelCheckBox *checkableItem = staticToCheckBox(childJ);
            CTG_InModelComboBox *comboItem = staticToComboBox(childJ);

            Q_ASSERT(checkableItem != nullptr);
            if (checkableItem == nullptr) {qDebug() << "CTG_InModelPossibilitiesChoice::constructCheckBox" << ", " <<  "checkableItem == nullptr"; return;}

            Q_ASSERT(comboItem != nullptr);
            if (comboItem == nullptr) {qDebug() << "CTG_InModelPossibilitiesChoice::constructCheckBox" << ", " <<  "comboItem == nullptr"; return;}

            CT_InAbstractModel *model = checkableItem->checkedOrFirstInModelActiveOrFirstDontChooseInModel();

            // set it the selected or first model
            comboItem->setCurrentInModel(model);
        }
    }

    // update (recursively) all checkbox and combobox now
    updateCheckBoxAndComboBox();

    _itemChangedSlotIsEnabled = true;
}

void CTG_InModelPossibilitiesChoice::recursiveConstructCheckBox(QStandardItem *parentOfItemsToTest)
{
    int size = parentOfItemsToTest->rowCount();

    for(int i=0; i<size; ++i)
    {
        QStandardItem *child = parentOfItemsToTest->child(i, 0);

        // go recursively in childrens of the model and search model that can be added in this item, set it if can
        recursiveSearchModelThatMatchWithItemAndSetIt(_possibility->inResultModel(), parentOfItemsToTest, child);

        // go recursively in childrens of the tree and do the same process
        recursiveConstructCheckBox(child);
    }
}

void CTG_InModelPossibilitiesChoice::recursiveSearchModelThatMatchWithItemAndSetIt(const CT_InAbstractModel *inModel,
                                                                                   QStandardItem *parentOfItemToTest,
                                                                                   QStandardItem *itemToTest)
{
    /*qDebug() << inModel->displayableName() << " possibilities : ";
    QList<CT_InStdModelPossibility*> pp = inModel->getPossibilitiesSaved();
    foreach (CT_InStdModelPossibility *p, pp) {
        qDebug() << p->outModel()->displayableName();
    }*/
    // get the checkable item if exist in this row
    CTG_InModelCheckBox *checkableItem = staticToCheckBox(itemToTest);

    Q_ASSERT(checkableItem != nullptr);
    if (checkableItem == nullptr) {qDebug() << "CTG_InModelPossibilitiesChoice::recursiveSearchModelThatMatchWithItemAndSetIt" << ", " <<  "checkableItem == nullptr"; return;}

    // if this checkable item match with a possibility of this in model, the model was added to "inModels" or
    // "dontChooseInModels" automatically
    checkableItem->addInModel(inModel);

    // go recursively to next childrens in the tree
    inModel->visitInChildrens([this, &parentOfItemToTest, &itemToTest](const CT_InAbstractModel* child) -> bool {
        this->recursiveSearchModelThatMatchWithItemAndSetIt(child, parentOfItemToTest, itemToTest);
        return true;
    });
}

void CTG_InModelPossibilitiesChoice::updateCheckBoxAndComboBox()
{
    _itemChangedSlotIsEnabled = false;

    QStandardItem *parentOfItemsToTest = _viewModel.invisibleRootItem();

    int size = parentOfItemsToTest->rowCount();

    for(int i=0; i<size; ++i)
        recursiveUpdateCheckBoxAndComboBox(parentOfItemsToTest->child(i, 0));

    _itemChangedSlotIsEnabled = true;
}

void CTG_InModelPossibilitiesChoice::recursiveUpdateCheckBoxAndComboBox(QStandardItem *parentOfItemsToTest)
{
    int size = parentOfItemsToTest->rowCount();

    for(int i=0; i<size; ++i)
    {
        QStandardItem *child = parentOfItemsToTest->child(i, 0);

        CTG_InModelCheckBox *checkableItem = staticToCheckBox(child);
        CTG_InModelComboBox *comboItem = staticToComboBox(child);

        Q_ASSERT(checkableItem != nullptr);
        if (checkableItem == nullptr) {qDebug() << "CTG_InModelPossibilitiesChoice::recursiveUpdateCheckBoxAndComboBox" << ", " <<  "checkableItem == nullptr"; return;}

        Q_ASSERT(comboItem != nullptr);
        if (comboItem == nullptr) {qDebug() << "CTG_InModelPossibilitiesChoice::recursiveUpdateCheckBoxAndComboBox" << ", " <<  "comboItem == nullptr"; return;}

        // get the inModel of the parent combobox
        CT_InAbstractModel *parentSelectedModel = nullptr;
        CTG_InModelComboBox *parentComboBoxItem = staticToComboBox(parentOfItemsToTest);

        if(parentComboBoxItem != nullptr)
            parentSelectedModel = parentComboBoxItem->currentInModel();

        CTG_InModelCheckBox::ModelContainer inModels = checkableItem->getAllInModels();
        CTG_InModelCheckBox::ModelContainerIterator it(inModels);

        while(it.hasNext()) {
            CT_InAbstractModel *model = it.next();

            bool active = (parentSelectedModel == model->parentModel());

            if(!active)
                active = (dynamic_cast<CT_InAbstractGroupModel*>(model) != nullptr) && (dynamic_cast<CT_InZeroOrMoreGroupModel*>(model->parentModel()) != nullptr);

            // set the model "active" (displayed and selectable by the user) only if it has no selected parent model or if the parent model is its parent
            // otherwise we set it not active
            checkableItem->setModelActive(model, active);

            // if we desactivated it and the combobox selected model is this model
            if(!active && (comboItem->currentInModel() == model)) {
                // we uncheck it
                comboItem->setPossibilityOfCurrentInModelChecked(false);

                // and remove the selected model of the combobox
                comboItem->setCurrentInModel(nullptr);
            }
        }

        // if the combobox has no selected model, we set it the selected "active" model or the first "active" model or the first "don't choose" model
        if(comboItem->currentInModel() == nullptr)
            comboItem->setCurrentInModel(checkableItem->checkedOrFirstInModelActiveOrFirstDontChooseInModel());

        // checkbox can be check if it has at least one model
        checkableItem->setCheckable(checkableItem->hasAtLeastOneInModel());

        // checkbox is enable if we are not in read only mode and if it has at least one model "active"
        checkableItem->setEnabled(!m_readOnly && checkableItem->hasInModelsActive());

        // combobox is editable if checkbox is enabled and the user must choose between multiple element
        comboItem->setEditable(checkableItem->isEnabled() && checkableItem->getInModelsActive().size() > 1);

        // update the widget display data
        comboItem->updateDisplayData();

        // get the possibility selected of the current model
        CT_InStdModelPossibility *poss = comboItem->getPossibilityOfCurrentInModel();

        // set checkbox checked if a possibility is selected
        if(poss != nullptr)
            checkableItem->setChecked(poss->isSelected());
        else
            checkableItem->setChecked(false);

        recursiveUpdateCheckBoxAndComboBox(child);
    }
}

void CTG_InModelPossibilitiesChoice::removeItemsThatCanNotBeChoosed()
{
    recursiveRemoveItemsThatCanNotBeChoosed(_viewModel.invisibleRootItem());
}

void CTG_InModelPossibilitiesChoice::recursiveRemoveItemsThatCanNotBeChoosed(QStandardItem *itemToTest)
{
    int size = itemToTest->rowCount();

    for(int i=0; i<size; ++i)
    {
        QStandardItem *child = itemToTest->child(i, 0);

        if(child->rowCount() > 0)
            recursiveRemoveItemsThatCanNotBeChoosed(itemToTest->child(i, 0));

        CTG_InModelCheckBox *checkableItem = static_cast<CTG_InModelCheckBox*>(itemToTest->child(i, COLUMN_CHECK));

        if(!checkableItem->isCheckable() && (child->rowCount() == 0))
        {
            itemToTest->removeRow(i);
            --i;
            --size;
        }
    }
}

QStandardItem* CTG_InModelPossibilitiesChoice::staticToFirstColumn(QStandardItem *itemToConvert)
{
    QStandardItem *root = itemToConvert->model()->invisibleRootItem();
    Q_ASSERT(itemToConvert != root);
    if (itemToConvert == root) {qDebug() << "CTG_InModelPossibilitiesChoice::staticToFirstColumn" << ", " <<  "itemToConvert == root"; return nullptr;}

    QStandardItem *parent = itemToConvert->parent();

    if(parent == nullptr)
        parent = root;

    return parent->child(itemToConvert->row(), 0);
}

CTG_InModelCheckBox *CTG_InModelPossibilitiesChoice::staticToCheckBox(QStandardItem *itemToConvert)
{
    QStandardItem *root = itemToConvert->model()->invisibleRootItem();
    Q_ASSERT(itemToConvert != root);
    if (itemToConvert == root) {qDebug() << "CTG_InModelPossibilitiesChoice::staticToCheckBox" << ", " <<  "itemToConvert == root"; return nullptr;}

    QStandardItem *parent = itemToConvert->parent();

    if(parent == nullptr)
        parent = root;

    return static_cast<CTG_InModelCheckBox*>(parent->child(itemToConvert->row(), COLUMN_CHECK));
}

CTG_InModelComboBox *CTG_InModelPossibilitiesChoice::staticToComboBox(QStandardItem *itemToConvert)
{
    QStandardItem *root = itemToConvert->model()->invisibleRootItem();
    Q_ASSERT(itemToConvert != root);
    if (itemToConvert == root) {qDebug() << "CTG_InModelPossibilitiesChoice::staticToComboBox" << ", " <<  "itemToConvert == root"; return nullptr;}

    QStandardItem *parent = itemToConvert->parent();

    if(parent == nullptr)
        parent = root;

    return static_cast<CTG_InModelComboBox*>(parent->child(itemToConvert->row(), COLUMN_COMBO));
}

bool CTG_InModelPossibilitiesChoice::staticIsRecursiveCurrentInModelnullptr(CTG_InModelComboBox *parentToTest)
{
    if(parentToTest == nullptr)
        return true;

    if(parentToTest != nullptr) {
        if(parentToTest->currentInModel() != nullptr)
            return false;
    }

    QStandardItem *first = staticToFirstColumn(parentToTest);

    if((first->parent() != nullptr) && (first->parent() != parentToTest->model()->invisibleRootItem()))
        return staticIsRecursiveCurrentInModelnullptr(staticToComboBox(first->parent()));

    return true;
}

void CTG_InModelPossibilitiesChoice::itemChanged(QStandardItem *item)
{
    if(_itemChangedSlotIsEnabled)
    {
        // if a checkbox was modified
        if(item->column() == COLUMN_CHECK) {
            CTG_InModelCheckBox *checkableItem = static_cast<CTG_InModelCheckBox*>(item);

            // we get his current state
            bool checked = checkableItem->isChecked();

            // we get the combobox to check/uncheck the selected model
            CTG_InModelComboBox *comboItem = staticToComboBox(item);

            // if the selected model state has changed
            if(comboItem->setPossibilityOfCurrentInModelChecked(checked))
            {
                // if we check it
                if(checked)
                {
                    if((comboItem->currentInModel() == nullptr) || (dynamic_cast<CT_InZeroOrMoreGroupModel*>(comboItem->currentInModel()->parentModel()) == nullptr)) {
                        // we check as its parent
                        checkableItem = staticToCheckBox(item->parent());

                        if(checkableItem != nullptr)
                            checkableItem->setChecked(true);
                    }
                }
                else
                {
                    // else we uncheck his childrens (recursively)
                    QStandardItem *itemFirstColumn = staticToFirstColumn(item);

                    int size = itemFirstColumn->rowCount();

                    for(int i=0; i<size; ++i) {
                        if((comboItem->currentInModel() == nullptr) || (dynamic_cast<CT_InZeroOrMoreGroupModel*>(comboItem->currentInModel()->parentModel()) == nullptr))
                            static_cast<CTG_InModelCheckBox*>(itemFirstColumn->child(i, COLUMN_CHECK))->setChecked(false);
                    }
                }
            }
        } else if(item->column() == COLUMN_COMBO) {
            // else if a combobox was modified, we check what we must change in other combobox, etc...
            updateCheckBoxAndComboBox();
        }
    }
}

void CTG_InModelPossibilitiesChoice::checkAllItemSelected()
{
    QModelIndexList indexes = ui->treeView->selectionModel()->selectedIndexes();

    foreach (const QModelIndex& index, indexes) {
        if(index.column() == COLUMN_CHECK) {
            CTG_InModelCheckBox* item = static_cast<CTG_InModelCheckBox*>(_viewModel.itemFromIndex(index));
            item->setChecked(true);
        }
    }
}

void CTG_InModelPossibilitiesChoice::uncheckAllItemSelected()
{
    QModelIndexList indexes = ui->treeView->selectionModel()->selectedIndexes();

    foreach (const QModelIndex& index, indexes) {
        if(index.column() == COLUMN_CHECK) {
            CTG_InModelCheckBox* item = static_cast<CTG_InModelCheckBox*>(_viewModel.itemFromIndex(index));
            item->setChecked(false);
        }
    }
}
