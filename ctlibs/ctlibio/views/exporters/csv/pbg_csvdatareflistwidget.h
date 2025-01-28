#ifndef PBG_CSVDATAREFLISTWIDGET_H
#define PBG_CSVDATAREFLISTWIDGET_H

#include <QWidget>
#include <QStandardItemModel>

class CT_OutAbstractSingularItemModel;
class CT_OutAbstractItemAttributeModel;

namespace Ui {
class PBG_CSVDataRefListWidget;
}

/*!
 *  Classe héritant de QStandardItemModel afin de retourner une QMimeData
 *  utilisable pour le drag d'un QStandardItem
 */
class PBG_CSVDataRefListWidgetDragModel : public QStandardItemModel
{
public:
    QMimeData* mimeData(const QModelIndexList &indexes) const;
};

class PBG_CSVDataRefListWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit PBG_CSVDataRefListWidget(QWidget *parent = nullptr);
    ~PBG_CSVDataRefListWidget();
    
    void setList(const QList<const CT_OutAbstractSingularItemModel*> &list);

private:
    Ui::PBG_CSVDataRefListWidget *ui;

    QList<const CT_OutAbstractSingularItemModel*> _list;
    PBG_CSVDataRefListWidgetDragModel       _model;

    void createHeader();
    void initView();

    QList<QStandardItem*> createItemsForSingularItem(const CT_OutAbstractSingularItemModel *sModel) const;
    QList<QStandardItem*> createItemsForItemAttribute(const int& itemModelIndex, const CT_OutAbstractItemAttributeModel *iaModel, const int& iaModelIndex) const;

signals:
    void attributeChoosed(const QString &data);

private slots:
    void on_treeView_doubleClicked(const QModelIndex &index);
    void on_pushButtonAddAll_clicked();
};

#endif // PBG_CSVDATAREFLISTWIDGET_H
