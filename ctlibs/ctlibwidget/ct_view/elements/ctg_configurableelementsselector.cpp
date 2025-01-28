#include "ctg_configurableelementsselector.h"
#include "ui_ctg_configurableelementsselector.h"

#include <QMessageBox>

CTG_ConfigurableElementsSelector::CTG_ConfigurableElementsSelector(QWidget *parent, bool readOnly) :
    QDialog(parent),
    ui(new Ui::CTG_ConfigurableElementsSelector)
{
    ui->setupUi(this);

    m_selectedContainerToModify = nullptr;
    m_readOnly = readOnly;

    ui->pushButtonAddAll->setDisabled(m_readOnly);
    ui->pushButtonRemove->setDisabled(m_readOnly);
    ui->pushButtonRemoveAll->setDisabled(m_readOnly);

    if(!m_readOnly) {
        connect(ui->listWidgetAvailable, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(transferFromAvailableToSelected(QListWidgetItem*)));
        connect(ui->listWidgetSelected, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(transferFromSelectedToAvailable(QListWidgetItem*)));
    }
}

CTG_ConfigurableElementsSelector::~CTG_ConfigurableElementsSelector()
{
    clearSelectedListWithoutSave();
    clearAvailable();

    delete ui;
}

void CTG_ConfigurableElementsSelector::setElementsAvailable(const QList<CT_AbstractConfigurableElement *> &elements)
{
    // remove all already selected elements
    clearSelectedListWithoutSave();

    // remove all available elements
    clearAvailable();

    addAvailableElementsToUI(elements);
}

bool CTG_ConfigurableElementsSelector::setElementsSelected(QList<CT_AbstractConfigurableElement *> *elements)
{
    // remove all already selected elements
    clearSelectedListWithoutSave();

    // clean available item of element that was in m_selectedContainerToModify
    cleanAvailableListOfElementsFromContainerToModify();

    m_selectedContainerToModify = elements;

    if(elements != nullptr) {
        QList<QListWidgetItem*> transfered;

        if(!addSelectedElementsToUI(*elements, transfered)) {

            while(!transfered.isEmpty()) {
                QListWidgetItem *item = transfered.takeFirst();
                cleanItem(item);
                transferFromSelectedToAvailable(item);
            }

            m_selectedContainerToModify = nullptr;
            return false;
        }
    }

    return true;
}

QList<CT_AbstractConfigurableElement *> CTG_ConfigurableElementsSelector::takeElementsSelectedAndConfigured()
{
    QList<CT_AbstractConfigurableElement *> l;

    if(m_selectedContainerToModify != nullptr)
        return l;

    if(!saveConfigurationOfAllElementsSelected())
        return l;

    while(ui->listWidgetSelected->count() > 0) {
        QListWidgetItem *item = ui->listWidgetSelected->item(0);
        CT_AbstractConfigurableElement *el = configurableElementFromItem(item);

        l.append(el);

        delete configurableWidgetFromItem(item);

        el = el->copy();

        setConfigurableElementToItem(el, item);
        setConfigurableWidgetToItem(el->createConfigurationWidget(), item);

        transferFromSelectedToAvailable(item);
    }

    return l;
}

bool CTG_ConfigurableElementsSelector::readOnly() const
{
    return m_readOnly;
}

void CTG_ConfigurableElementsSelector::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e);
    if(ui->scrollAreaConfiguration->widget() != nullptr)
    {
        ui->scrollAreaConfiguration->widget()->setFixedSize(ui->scrollAreaConfiguration->width(), ui->scrollAreaConfiguration->height());
    }
}

void CTG_ConfigurableElementsSelector::addAvailableElementsToUI(const QList<CT_AbstractConfigurableElement *> &elements)
{
    QListIterator<CT_AbstractConfigurableElement*> it(elements);

    while(it.hasNext())
        ui->listWidgetAvailable->addItem(createItem(it.next()));
}

bool CTG_ConfigurableElementsSelector::addSelectedElementsToUI(const QList<CT_AbstractConfigurableElement *> &elements, QList<QListWidgetItem*> &transfered)
{
    QListIterator<CT_AbstractConfigurableElement*> it(elements);

    while(it.hasNext()) {
        CT_AbstractConfigurableElement *el = it.next();

        QListWidgetItem *item = findItemInAvailableElementByUniqueName(el->getUniqueName());

        // item not found or item already added
        if(item == nullptr)
            return false;

        QListWidgetItem *cpy = copyItem(item);

        // we delete previous element and previous widget
        delete configurableElementFromItem(cpy);
        delete configurableWidgetFromItem(cpy);

        // set the element to this item
        setConfigurableElementToItem(el, cpy);
        setConfigurableWidgetToItem(el->createConfigurationWidget(), cpy); // and a new configuration widget

        // finally add it to selected
        ui->listWidgetSelected->addItem(cpy);

        // add it to list that contains transfered items
        transfered.append(cpy);
    }

    return true;
}

CT_AbstractConfigurableElement* CTG_ConfigurableElementsSelector::configurableElementFromItem(const QListWidgetItem *item) const
{
    return (CT_AbstractConfigurableElement*)item->data(Qt::UserRole).value<void*>();
}

CT_AbstractConfigurableWidget *CTG_ConfigurableElementsSelector::configurableWidgetFromItem(const QListWidgetItem *item) const
{
    return (CT_AbstractConfigurableWidget*)item->data(Qt::UserRole+2).value<void*>();
}

void CTG_ConfigurableElementsSelector::setConfigurableElementToItem(CT_AbstractConfigurableElement *ce, QListWidgetItem *item) const
{
    item->setData(Qt::UserRole, QVariant::fromValue((void*)ce));
}

void CTG_ConfigurableElementsSelector::setConfigurableWidgetToItem(CT_AbstractConfigurableWidget *cw, QListWidgetItem *item) const
{
    item->setData(Qt::UserRole+2, QVariant::fromValue((void*)cw));

    if(cw != nullptr)
        cw->setDisabled(readOnly());
}

QListWidgetItem *CTG_ConfigurableElementsSelector::createItem(CT_AbstractConfigurableElement *ce) const
{
    CT_AbstractConfigurableElement *el = ce->copy();

    QListWidgetItem *item = new QListWidgetItem(el->getShortDisplayableName());
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    item->setToolTip(el->getShortDescription());
    setConfigurableElementToItem(el, item);
    setConfigurableWidgetToItem(el->createConfigurationWidget(), item);

    return item;
}

QListWidgetItem *CTG_ConfigurableElementsSelector::copyItem(QListWidgetItem *item) const
{
    CT_AbstractConfigurableElement *el = configurableElementFromItem(item)->copy();

    QListWidgetItem *copy = new QListWidgetItem(el->getShortDisplayableName());
    copy->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    copy->setToolTip(el->getShortDescription());
    setConfigurableElementToItem(el, copy);
    setConfigurableWidgetToItem(el->createConfigurationWidget(), copy);

    return copy;
}

bool CTG_ConfigurableElementsSelector::saveConfigurationOfAllElementsSelected()
{
    int count = ui->listWidgetSelected->count();

    for(int i=0; i<count; ++i) {
        QListWidgetItem *item = ui->listWidgetSelected->item(i);

        CT_AbstractConfigurableWidget *wid = configurableWidgetFromItem(item);

        if(wid != nullptr) {
            QString err;
            if(!wid->canAcceptIt(&err) || !wid->updateElement(&err)) {
                QMessageBox::critical(this, tr("Erreur"), err);
                return false;
            }
        }
    }

    return true;
}

bool CTG_ConfigurableElementsSelector::updateSelectedElementContainerToModify()
{
    if(m_selectedContainerToModify == nullptr)
        return true;

    QList<CT_AbstractConfigurableElement*> newElements;

    int count = ui->listWidgetSelected->count();

    for(int i=0; i<count; ++i)
        newElements.append( configurableElementFromItem( ui->listWidgetSelected->item(i) ) );

    (*m_selectedContainerToModify) = newElements;

    return true;
}

bool CTG_ConfigurableElementsSelector::selectedContainerIsSetAndContains(CT_AbstractConfigurableElement *el) const
{
    if(m_selectedContainerToModify != nullptr)
        return m_selectedContainerToModify->contains(el);

    return false;
}

QListWidgetItem *CTG_ConfigurableElementsSelector::findItemInAvailableElementByUniqueName(const QString &uniqueName) const
{
    int count = ui->listWidgetAvailable->count();

    for(int i=0; i<count; ++i) {
        QListWidgetItem *bItem = ui->listWidgetAvailable->item(i);

        if(configurableElementFromItem(bItem)->getUniqueName() == uniqueName)
            return bItem;
    }

    return nullptr;
}

void CTG_ConfigurableElementsSelector::clearSelectedListWithoutSave()
{
    while(ui->listWidgetSelected->count() > 0) {
        QListWidgetItem *item = ui->listWidgetSelected->item(0);

        cleanItem(item);

        transferFromSelectedToAvailable(item);
    }
}

void CTG_ConfigurableElementsSelector::cleanAvailableListOfElementsFromContainerToModify()
{
    int count = ui->listWidgetAvailable->count();

    for(int i=0; i<count; ++i)
        cleanItem(ui->listWidgetAvailable->item(i));
}

void CTG_ConfigurableElementsSelector::cleanItem(QListWidgetItem *item)
{
    CT_AbstractConfigurableElement *el = configurableElementFromItem(item);

    if(selectedContainerIsSetAndContains(el)) {
        delete configurableWidgetFromItem(item);

        CT_AbstractConfigurableElement *elCpy = el->copy();
        setConfigurableElementToItem(elCpy, item);
        setConfigurableWidgetToItem(elCpy->createConfigurationWidget(), item);
    }
}

void CTG_ConfigurableElementsSelector::clearItem(QListWidgetItem *item)
{
    if(!selectedContainerIsSetAndContains(configurableElementFromItem(item))) {
        delete configurableElementFromItem(item);
        setConfigurableElementToItem(nullptr, item);
    }

    delete configurableWidgetFromItem(item);
    setConfigurableWidgetToItem(nullptr, item);
}

void CTG_ConfigurableElementsSelector::clearAvailable()
{
    int count = ui->listWidgetAvailable->count();

    for(int i=0; i<count; ++i)
        clearItem(ui->listWidgetAvailable->item(i));

    ui->listWidgetAvailable->clear();
}

void CTG_ConfigurableElementsSelector::transferFromAvailableToSelected(QListWidgetItem *item)
{
    if(item == nullptr)
        return;

    QListWidgetItem *cpy = copyItem(item);
    ui->listWidgetSelected->addItem(cpy);
    ui->listWidgetSelected->setCurrentItem(cpy);
}

void CTG_ConfigurableElementsSelector::transferFromSelectedToAvailable(QListWidgetItem *item)
{
    if(item == nullptr)
        return;

    clearItem(item);
    delete item;
}

void CTG_ConfigurableElementsSelector::on_pushButtonAddAll_clicked()
{
    int n = ui->listWidgetAvailable->count();

    for(int i=0; i<n; ++i)
    {
        transferFromAvailableToSelected(ui->listWidgetAvailable->item(i));

        if(n > ui->listWidgetAvailable->count())
        {
            n = ui->listWidgetAvailable->count();
            --i;
        }
    }

}

void CTG_ConfigurableElementsSelector::on_pushButtonRemoveAll_clicked()
{
    while(ui->listWidgetSelected->count() > 0)
        transferFromSelectedToAvailable(ui->listWidgetSelected->item(0));
}

void CTG_ConfigurableElementsSelector::on_pushButtonRemove_clicked()
{
    QList<QListWidgetItem*> items = ui->listWidgetSelected->selectedItems();

    while(!items.isEmpty())
        transferFromSelectedToAvailable(items.takeFirst());
}

void CTG_ConfigurableElementsSelector::on_listWidgetSelected_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous)

    if(!readOnly())
        ui->pushButtonRemove->setEnabled(current != nullptr);

    ui->scrollAreaConfiguration->takeWidget();

    if(current != nullptr) {
        ui->scrollAreaConfiguration->setWidget( configurableWidgetFromItem(current) );

        if(ui->scrollAreaConfiguration->widget() != nullptr)
        {
            ui->scrollAreaConfiguration->setMinimumWidth(ui->scrollAreaConfiguration->widget()->width());
            ui->scrollAreaConfiguration->widget()->setFixedSize(ui->scrollAreaConfiguration->width(), ui->scrollAreaConfiguration->height());

        }
    }
}

void CTG_ConfigurableElementsSelector::accept()
{
    if(readOnly() || (saveConfigurationOfAllElementsSelected() && updateSelectedElementContainerToModify()))
        QDialog::accept();
}

void CTG_ConfigurableElementsSelector::on_buttonBox_helpRequested()
{
    emit showHelp();
}

