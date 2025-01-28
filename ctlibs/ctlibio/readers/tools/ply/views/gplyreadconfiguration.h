#ifndef GPLYREADCONFIGURATION_H
#define GPLYREADCONFIGURATION_H

#include <QWidget>
#include "readers/tools/ply/views/sub/iplygenericconfiguration.h"
#include "readers/tools/ply/plyreadconfiguration.h"

namespace Ui {
class GPlyReadConfiguration;
}

class QTreeWidgetItem;

class GPlyReadConfiguration : public QWidget
{
    Q_OBJECT

public:
    explicit GPlyReadConfiguration(QWidget *parent = nullptr);
    ~GPlyReadConfiguration();

    /**
     * @brief Set the header to use
     */
    void setHeader(const PlyHeader& header);

    /**
     * @brief Returns true if all objects has a defined property
     */
    bool isValid() const;

    /**
     * @brief Set a configuration
     */
    void setConfiguration(const PlyReadConfiguration& config);

    /**
     * @brief Returns the configuration
     */
    PlyReadConfiguration getConfiguration() const;

private:
    struct ItemConfiguration {
        ItemConfiguration() {
            configurationWidget = nullptr;
        }

        ~ItemConfiguration() {
            delete configurationWidget;
        }

        IPlyGenericConfiguration*   configurationWidget;
    };

    Ui::GPlyReadConfiguration                   *ui;
    PlyHeader                                   m_header;
    ItemConfiguration*                          m_currentItemConfiguration;
    QHash<QTreeWidgetItem*, ItemConfiguration*> m_itemConfiguration;

    QTreeWidgetItem*                            m_vertexItem;
    QTreeWidgetItem*                            m_vertexColorItem;
    QTreeWidgetItem*                            m_vertexScalarItem;
    QTreeWidgetItem*                            m_vertexNormalItem;

    /**
     * @brief Call to init/reset ui
     */
    void resetUi();

    /**
     * @brief Add a new object in item
     */
    QTreeWidgetItem* addNewObjectInItem(QTreeWidgetItem* parentItem, const QString& name, int type);

    /**
     * @brief Add a new object of type in item
     */
    QTreeWidgetItem* addNewObjectFromItemType(QTreeWidgetItem* parentItem);

    /**
     * @brief Create the configuration if not exist and return it. Is already exist just return it.
     */
    template<typename WidgetToCreate>
    ItemConfiguration* createOrGetItemConfiguration(QTreeWidgetItem* item) const {
        ItemConfiguration* c = m_itemConfiguration.value(item, nullptr);

        if(c == nullptr) {
            c = new ItemConfiguration();
            c->configurationWidget = new WidgetToCreate();
            c->configurationWidget->setHeader(m_header);
            c->configurationWidget->setPlyElement(this->getComboBoxElementCurrentData());
            c->configurationWidget->toWidget()->setVisible(false);

            const_cast<GPlyReadConfiguration*>(this)->m_itemConfiguration.insert(item, c);
        }

        return c;
    }

    /**
     * @brief Return the combobox current data
     */
    int getComboBoxElementCurrentData() const;

private slots:
    /**
     * @brief Called when the user right click on the tree widget
     */
    void showContextMenu(const QPoint& point);

    /**
     * @brief Called when a new item is selected
     */
    void itemSelectionChanged();

    /**
     * @brief Called when the element in the combobox changed
     */
    void elementChanged(int index);

    /**
     * @brief Called by the action in the context menu to add a colors, normals, or other object
     */
    void addNewObject();

    /**
     * @brief Called when the user click on the button "New object" to add a new color, normal or other object
     */
    void on_pushButtonNewObject_clicked();

    /**
     * @brief Called by the action in the context menu to delete a colors, normals, or other object
     */
    void deleteObject();
};

#endif // GPLYREADCONFIGURATION_H
