#include "gplyreadconfiguration.h"
#include "ui_gplyreadconfiguration.h"

#include "sub/gplyvertexconfiguration.h"
#include "sub/gplycolorconfiguration.h"
#include "sub/gplyscalarconfiguration.h"
#include "sub/gplynormalconfiguration.h"
#include "tools/plyviewtools.h"

#include <QMenu>
#include <QInputDialog>

#define VERTEX_DATA 1
#define FACE_DATA 2
#define EDGE_DATA 3

#define COLOR_DATA 10
#define SCALAR_DATA 20
#define NORMAL_DATA 30

#define COLOR_OBJECT_DATA 100
#define SCALAR_OBJECT_DATA 200
#define NORMAL_OBJECT_DATA 300

GPlyReadConfiguration::GPlyReadConfiguration(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GPlyReadConfiguration)
{
    ui->setupUi(this);

    m_currentItemConfiguration = nullptr;
    m_vertexItem = nullptr;
    m_vertexColorItem = nullptr;
    m_vertexNormalItem = nullptr;
    m_vertexScalarItem = nullptr;

    ui->treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->treeWidget->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(itemSelectionChanged()));
    connect(ui->treeWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));

    connect(ui->comboBoxElement, SIGNAL(currentIndexChanged(int)), this, SLOT(elementChanged(int)));
}

GPlyReadConfiguration::~GPlyReadConfiguration()
{
    m_currentItemConfiguration = nullptr;

    qDeleteAll(m_itemConfiguration.begin(), m_itemConfiguration.end());
    m_itemConfiguration.clear();

    delete ui;
}

void GPlyReadConfiguration::setHeader(const PlyHeader &header)
{
    m_header = header;

    ui->comboBoxElement->clear();
    PlyViewTools::staticFillElementsInComboBox(ui->comboBoxElement, m_header);

    resetUi();
}

bool GPlyReadConfiguration::isValid() const
{
    QHashIterator<QTreeWidgetItem*, ItemConfiguration*> it(m_itemConfiguration);

    while(it.hasNext()) {
        if(!it.next().value()->configurationWidget->isValid())
            return false;
    }

    return true;
}

void GPlyReadConfiguration::setConfiguration(const PlyReadConfiguration &config)
{
    resetUi();

    dynamic_cast<GPlyVertexConfiguration*>(createOrGetItemConfiguration<GPlyVertexConfiguration>(m_vertexItem)->configurationWidget)->setConfiguration(config.vertex);

    foreach (const PlyColorConfiguration& cc, config.colors) {
        QTreeWidgetItem* item = addNewObjectInItem(m_vertexColorItem, tr("Couleur %1").arg(m_vertexColorItem->childCount()+1), COLOR_OBJECT_DATA);
        dynamic_cast<GPlyColorConfiguration*>(createOrGetItemConfiguration<GPlyColorConfiguration>(item)->configurationWidget)->setConfiguration(cc);
    }

    foreach (const PlyScalarConfiguration& sc, config.scalars) {
        QTreeWidgetItem* item = addNewObjectInItem(m_vertexScalarItem, tr("Scalaire %1").arg(m_vertexScalarItem->childCount()+1), SCALAR_OBJECT_DATA);
        dynamic_cast<GPlyScalarConfiguration*>(createOrGetItemConfiguration<GPlyScalarConfiguration>(item)->configurationWidget)->setConfiguration(sc);
    }

    foreach (const PlyNormalConfiguration& nc, config.normals) {
        QTreeWidgetItem* item = addNewObjectInItem(m_vertexNormalItem, tr("Normale %1").arg(m_vertexNormalItem->childCount()+1), NORMAL_OBJECT_DATA);
        dynamic_cast<GPlyNormalConfiguration*>(createOrGetItemConfiguration<GPlyNormalConfiguration>(item)->configurationWidget)->setConfiguration(nc);
    }

    ui->treeWidget->clearSelection();
    m_vertexItem->setSelected(true);
}

PlyReadConfiguration GPlyReadConfiguration::getConfiguration() const
{
    PlyReadConfiguration config;
    config.vertex = dynamic_cast<GPlyVertexConfiguration*>(createOrGetItemConfiguration<GPlyVertexConfiguration>(m_vertexItem)->configurationWidget)->getConfiguration();

    for(int i=0; i<m_vertexColorItem->childCount(); ++i) {
        config.colors.append(dynamic_cast<GPlyColorConfiguration*>(createOrGetItemConfiguration<GPlyColorConfiguration>(m_vertexColorItem->child(i))->configurationWidget)->getConfiguration());
    }

    for(int i=0; i<m_vertexScalarItem->childCount(); ++i) {
        config.scalars.append(dynamic_cast<GPlyScalarConfiguration*>(createOrGetItemConfiguration<GPlyScalarConfiguration>(m_vertexScalarItem->child(i))->configurationWidget)->getConfiguration());
    }

    for(int i=0; i<m_vertexNormalItem->childCount(); ++i) {
        config.normals.append(dynamic_cast<GPlyNormalConfiguration*>(createOrGetItemConfiguration<GPlyNormalConfiguration>(m_vertexNormalItem->child(i))->configurationWidget)->getConfiguration());
    }

    return config;
}

void GPlyReadConfiguration::resetUi()
{
    m_currentItemConfiguration = nullptr;

    qDeleteAll(m_itemConfiguration.begin(), m_itemConfiguration.end());
    m_itemConfiguration.clear();

    ui->treeWidget->clear();
    ui->treeWidget->setHeaderLabel(tr("Type"));

    m_vertexItem = new QTreeWidgetItem(ui->treeWidget);
    m_vertexItem->setText(0, tr("Points"));
    m_vertexItem->setData(0, Qt::UserRole, VERTEX_DATA);

    m_vertexColorItem = new QTreeWidgetItem(m_vertexItem);
    m_vertexColorItem->setText(0, tr("Couleurs"));
    m_vertexColorItem->setData(0, Qt::UserRole, COLOR_DATA);

    m_vertexScalarItem = new QTreeWidgetItem(m_vertexItem);
    m_vertexScalarItem->setText(0, tr("Scalaires"));
    m_vertexScalarItem->setData(0, Qt::UserRole, SCALAR_DATA);

    m_vertexNormalItem = new QTreeWidgetItem(m_vertexItem);
    m_vertexNormalItem->setText(0, tr("Normales"));
    m_vertexNormalItem->setData(0, Qt::UserRole, NORMAL_DATA);

    ui->treeWidget->clearSelection();
    m_vertexItem->setSelected(true);

    ui->treeWidget->expandAll();
}

QTreeWidgetItem* GPlyReadConfiguration::addNewObjectInItem(QTreeWidgetItem *parentItem, const QString& name, int type)
{
    ui->treeWidget->clearSelection();

    QTreeWidgetItem* subItem = new QTreeWidgetItem(parentItem);
    subItem->setText(0, name);
    subItem->setData(0, Qt::UserRole, type);
    subItem->setSelected(true);

    ui->treeWidget->expandItem(parentItem);

    return subItem;
}

QTreeWidgetItem* GPlyReadConfiguration::addNewObjectFromItemType(QTreeWidgetItem* parentItem)
{
    if(parentItem == nullptr)
        return nullptr;

    const int type = parentItem->data(0, Qt::UserRole).toInt();

    if(type == COLOR_DATA)
        return addNewObjectInItem(parentItem, tr("Couleur %1").arg(parentItem->childCount()+1), COLOR_OBJECT_DATA);
    else if(type == SCALAR_DATA)
        return addNewObjectInItem(parentItem, tr("Scalaire %1").arg(parentItem->childCount()+1), SCALAR_OBJECT_DATA);
    else if(type == NORMAL_DATA)
        return addNewObjectInItem(parentItem, tr("Normale %1").arg(parentItem->childCount()+1), NORMAL_OBJECT_DATA);

    return nullptr;
}

int GPlyReadConfiguration::getComboBoxElementCurrentData() const
{
    return ui->comboBoxElement->currentData().toInt();
}

void GPlyReadConfiguration::showContextMenu(const QPoint &point)
{
    QTreeWidgetItem* item = ui->treeWidget->itemAt(point);

    if(item != nullptr) {
        const int type = item->data(0, Qt::UserRole).toInt();

        if((type == COLOR_DATA)
                || (type == SCALAR_DATA)
                || (type == NORMAL_DATA)) {

            QMenu menu;

            QAction* action = menu.addAction(tr("Nouveau"), this, SLOT(addNewObject()));
            action->setData(QVariant::fromValue(static_cast<void*>(item)));

            menu.exec(ui->treeWidget->mapToGlobal(point));
        } else if((type == COLOR_OBJECT_DATA)
                    || (type == SCALAR_OBJECT_DATA)
                    || (type == NORMAL_OBJECT_DATA)) {
            QMenu menu;

            QAction* action = menu.addAction(tr("Supprimer"), this, SLOT(deleteObject()));
            action->setData(QVariant::fromValue(static_cast<void*>(item)));

            menu.exec(ui->treeWidget->mapToGlobal(point));
        }
    }
}

void GPlyReadConfiguration::itemSelectionChanged()
{
    if(m_currentItemConfiguration != nullptr) {
        ui->verticalLayout->removeWidget(m_currentItemConfiguration->configurationWidget->toWidget());
        m_currentItemConfiguration->configurationWidget->toWidget()->setVisible(false);
    }

    m_currentItemConfiguration = nullptr;

    QList<QTreeWidgetItem*> items = ui->treeWidget->selectedItems();

    if(!items.isEmpty()) {
        QTreeWidgetItem* item = items.last();

        ItemConfiguration* newConfig = nullptr;

        if(item->data(0, Qt::UserRole).toInt() == VERTEX_DATA)
            newConfig = createOrGetItemConfiguration<GPlyVertexConfiguration>(item);
        else if(item->data(0, Qt::UserRole).toInt() == COLOR_OBJECT_DATA)
            newConfig = createOrGetItemConfiguration<GPlyColorConfiguration>(item);
        else if(item->data(0, Qt::UserRole).toInt() == SCALAR_OBJECT_DATA)
            newConfig = createOrGetItemConfiguration<GPlyScalarConfiguration>(item);
        else if(item->data(0, Qt::UserRole).toInt() == NORMAL_OBJECT_DATA)
            newConfig = createOrGetItemConfiguration<GPlyNormalConfiguration>(item);

        if(newConfig != nullptr) {
            if(newConfig->configurationWidget->getPlyElement() != -1)
                ui->comboBoxElement->setCurrentIndex(ui->comboBoxElement->findData(newConfig->configurationWidget->getPlyElement()));

            ui->verticalLayout->insertWidget(1, newConfig->configurationWidget->toWidget());
            newConfig->configurationWidget->toWidget()->setVisible(true);
            ui->pushButtonNewObject->setVisible(false);
        } else {
            ui->pushButtonNewObject->setVisible(true);
        }

        m_currentItemConfiguration = newConfig;
    }
}

void GPlyReadConfiguration::elementChanged(int index)
{
    Q_UNUSED(index)

    if(m_currentItemConfiguration != nullptr)
        m_currentItemConfiguration->configurationWidget->setPlyElement(ui->comboBoxElement->currentData().toInt());
}

void GPlyReadConfiguration::addNewObject()
{
    QAction* act = static_cast<QAction*>(sender());
    addNewObjectFromItemType(static_cast<QTreeWidgetItem*>(act->data().value<void*>()));
}

void GPlyReadConfiguration::on_pushButtonNewObject_clicked()
{
    QTreeWidgetItem* parentItem = nullptr;

    if(!ui->treeWidget->selectedItems().isEmpty())
        parentItem = ui->treeWidget->selectedItems().first();

    addNewObjectFromItemType(parentItem);
}

void GPlyReadConfiguration::deleteObject()
{
    QAction* act = static_cast<QAction*>(sender());

    QTreeWidgetItem* item = static_cast<QTreeWidgetItem*>(act->data().value<void*>());

    if(item != nullptr) {
        m_currentItemConfiguration = nullptr;
        delete m_itemConfiguration.take(item);
        delete item;
        ui->treeWidget->clearSelection();
        m_vertexItem->setSelected(true);
    }
}
