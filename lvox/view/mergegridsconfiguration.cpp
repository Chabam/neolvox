#include "mergegridsconfiguration.h"
#include "ui_mergegridsconfiguration.h"
#include "tools/lvox3_mergegrids.h"

MergeGridsConfiguration::MergeGridsConfiguration(QWidget *parent) :
    CT_AbstractConfigurableWidget(parent),
    ui(new Ui::MergeGridsConfiguration)
{
    ui->setupUi(this);
}

MergeGridsConfiguration::~MergeGridsConfiguration()
{
    delete ui;
}

bool MergeGridsConfiguration::isSettingsModified() const
{
    return false;
}
bool MergeGridsConfiguration::updateElement(QString *err)
{
    Q_UNUSED(err);
    return true;
}

VoxelReducerOptions MergeGridsConfiguration::getOptions()
{
    VoxelReducerOptions opts;
    const QVector<VoxelReducerDefinitionStruct> &reducers = LVOX3_MergeGrids::getReducerList();
    uint idx = ui->comboBoxMergeMethod->currentIndex();
    opts.reducerLabel = reducers[idx].label;
    opts.ignoreVoxelZeroDensity = ui->checkBoxIgnoreZero->isChecked();
    opts.effectiveRaysThreshold = ui->spinBoxEffectiveRaysThreshold->value();
    opts.isRelativeThreshold = ui->checkIsRelativeThreshold->isChecked();
    return opts;
}

void MergeGridsConfiguration::setOptions(VoxelReducerOptions &opts)
{
    ui->checkBoxIgnoreZero->setChecked(opts.ignoreVoxelZeroDensity);
    ui->checkIsRelativeThreshold->setChecked(opts.isRelativeThreshold);
    ui->spinBoxEffectiveRaysThreshold->setValue(opts.effectiveRaysThreshold);

    ui->comboBoxMergeMethod->clear();
    const QVector<VoxelReducerDefinitionStruct> &reducers = LVOX3_MergeGrids::getReducerList();
    for (int i = 0; i < reducers.size(); i++) {
        const VoxelReducerDefinitionStruct &def = reducers[i];
        QString item = def.name + " " + def.desc;
        ui->comboBoxMergeMethod->addItem(item);
        if (def.label == opts.reducerLabel) {
            ui->comboBoxMergeMethod->setCurrentIndex(i);
        }
    }

    updateState();
}

void MergeGridsConfiguration::updateState()
{
    int idx = ui->comboBoxMergeMethod->currentIndex();
    const QVector<VoxelReducerDefinitionStruct> reducers = LVOX3_MergeGrids::getReducerList();
    const VoxelReducerDefinitionStruct &def = reducers[idx];
    ui->checkBoxIgnoreZero->setEnabled(def.label == LVOX_REDUCER_TRUST_RATIO);
    ui->spinBoxEffectiveRaysThreshold->setEnabled(def.label == LVOX_REDUCER_SUM_RATIO || def.label == LVOX_REDUCER_WEIGHTED_RDI || def.label == LVOX_REDUCER_WEIGHTED_RDI_ALT);
}

void MergeGridsConfiguration::on_comboBoxMergeMethod_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    updateState();
}
