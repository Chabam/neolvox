#ifndef MERGEGRIDSCONFIGURATION_H
#define MERGEGRIDSCONFIGURATION_H

#include <QWidget>
#include "ct_view/ct_abstractconfigurablewidget.h"
#include "tools/lvox3_mergegrids.h"

namespace Ui {
class MergeGridsConfiguration;
}

class MergeGridsConfiguration : public CT_AbstractConfigurableWidget
{
    Q_OBJECT

public:
    explicit MergeGridsConfiguration(QWidget *parent = 0);
    ~MergeGridsConfiguration();
    bool isSettingsModified() const;
    bool updateElement(QString *err);

    VoxelReducerOptions getOptions();
    void setOptions(VoxelReducerOptions &opts);
    void updateState();

private slots:
    void on_comboBoxMergeMethod_currentIndexChanged(int index);

private:
    Ui::MergeGridsConfiguration *ui;
};

#endif // MERGEGRIDSCONFIGURATION_H
