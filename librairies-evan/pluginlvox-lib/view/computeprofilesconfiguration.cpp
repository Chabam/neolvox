#include "computeprofilesconfiguration.h"
#include "ui_computeprofilesconfiguration.h"
#include <QDebug>
//Check if the value entered is not a percentage
#define CHECKPOURCENT(VALUE, ERRSTR)    if(VALUE < 0) { \
                                            if(err != NULL) \
                                                *err = ComputeProfilesConfiguration::tr("Valeur %1 en pourcentage invalide : %2 >= 0").arg(ERRSTR).arg(VALUE); \
                                            return false; \
                                        }

ComputeProfilesConfiguration::ComputeProfilesConfiguration(QWidget *parent) :
    CT_AbstractConfigurableWidget(parent),
    ui(new Ui::ComputeProfilesConfiguration)
{
    ui->setupUi(this);

    //Values of the voxel
    m_defaultConfiguration.minValue = 0.00;
    m_defaultConfiguration.maxValue = 999999.00;

    m_defaultConfiguration.ordonneeAxis = "Z";
    m_defaultConfiguration.genAxis = "X";
    //Unit types can be voxels or coordinates(meters)
    m_defaultConfiguration.unitType = "Coordonnées";
    m_defaultConfiguration.gridCustomized = false;
    m_defaultConfiguration.genValuesInPourcent = false;
    m_defaultConfiguration.minGen = -100.00;
    m_defaultConfiguration.maxGen = 100.00;
    m_defaultConfiguration.stepGen = 999999.00;
    m_defaultConfiguration.terminalCustomized = false;
    m_defaultConfiguration.abscisseOrdonneeValuesInPourcent = false;
    m_defaultConfiguration.averaged = true;
    m_defaultConfiguration.minAbscisse = -100.00;
    m_defaultConfiguration.maxAbscisse = 100.00;
    m_defaultConfiguration.minOrdonnee = -100.00;
    m_defaultConfiguration.maxOrdonnee = 100.00;

    for(int i=0; i<ui->comboBoxOrdonnee->count(); ++i)
        ui->comboBoxOrdonnee->setItemData(i, ui->comboBoxOrdonnee->itemText(i).toLower());

    for(int i=0; i<ui->comboBoxUnitType->count(); ++i)
        ui->comboBoxUnitType->setItemData(i, ui->comboBoxUnitType->itemText(i).toLower());

    on_comboBoxOrdonnee_currentIndexChanged(ui->comboBoxOrdonnee->currentIndex());
}

ComputeProfilesConfiguration::~ComputeProfilesConfiguration()
{
    delete ui;
}

void ComputeProfilesConfiguration::setConfiguration(const ComputeProfilesConfiguration::Configuration &c)
{
    QString toolTipMinMax = "Valeur du profil:\n\nUne intervalle positive calcule la valeur IDR pour chaque tranche verticale."
                            "\n\nUne intervalle négative calcule des codes d'erreur:\nPour une grille sans l'étape de personnalisation"
                            "\nle seul code d'erreur est -9, le NODATA\n\nPour une grille avec l'étape de personnalisation des grilles "
                            "les codes d'erreurs sont les suivants:\n\n"
                            "Considérant la formule du IDR a/(c-b)\n"
                            "-4: a>(c-b)\n"
                            "-3: c=b"
                            "-2: c<b\n"
                            "-1: (c-b)<10\n";
    QString toolTipStep = "Le pas est calculé en mètres si l'unité de calcul est Coordonnées, sinon il est calculé en voxels.";

    ui->doubleSpinBoxMinValue->setValue(c.minValue);
    ui->doubleSpinBoxMaxValue->setValue(c.maxValue);
    ui->doubleSpinBoxMinValue->setToolTip(toolTipMinMax);
    ui->doubleSpinBoxMaxValue->setToolTip(toolTipMinMax);
    ui->comboBoxOrdonnee->setCurrentIndex(ui->comboBoxOrdonnee->findData(c.ordonneeAxis.toLower()));
    on_comboBoxOrdonnee_currentIndexChanged(ui->comboBoxOrdonnee->currentIndex());
    ui->comboBoxGen->setCurrentIndex(ui->comboBoxGen->findData(c.genAxis.toLower()));
    ui->comboBoxUnitType->setCurrentIndex(ui->comboBoxUnitType->findData(c.unitType.toLower()));

    ui->radioButtonPersonalized->setChecked(c.gridCustomized);
    ui->radioButtonAllGrid->setChecked(!c.gridCustomized);

    ui->checkBoxZValuesInPourcent->setChecked(c.genValuesInPourcent);
    ui->doubleSpinBoxMinZ->setValue(c.minGen);
    ui->doubleSpinBoxMaxZ->setValue(c.maxGen);
    ui->doubleSpinBoxGenStep->setValue(c.stepGen);
    ui->doubleSpinBoxGenStep->setToolTip(toolTipStep);

    ui->checkBoxPersonalized->setChecked(c.terminalCustomized);
    ui->checkBoxXYValuesInPourcent->setChecked(c.abscisseOrdonneeValuesInPourcent);
    ui->checkBoxAveraged->setChecked(c.averaged);
    ui->doubleSpinBoxMinX->setValue(c.minAbscisse);
    ui->doubleSpinBoxMaxX->setValue(c.maxAbscisse);
    ui->doubleSpinBoxMinY->setValue(c.minOrdonnee);
    ui->doubleSpinBoxMaxY->setValue(c.maxOrdonnee);
}

ComputeProfilesConfiguration::Configuration ComputeProfilesConfiguration::getConfiguration() const
{
    Configuration conf = m_defaultConfiguration;

    conf.minValue = ui->doubleSpinBoxMinValue->value();
    conf.maxValue = ui->doubleSpinBoxMaxValue->value();
    conf.ordonneeAxis = ui->comboBoxOrdonnee->currentData().toString();
    conf.genAxis = ui->comboBoxGen->currentData().toString();
    conf.unitType = ui->comboBoxUnitType->currentData().toString();
    conf.gridCustomized = ui->radioButtonPersonalized->isChecked();
    conf.averaged = ui->checkBoxAveraged->isChecked();

    if(conf.gridCustomized) {
        conf.genValuesInPourcent = ui->checkBoxZValuesInPourcent->isChecked();
        conf.minGen = ui->doubleSpinBoxMinZ->value();
        conf.maxGen = ui->doubleSpinBoxMaxZ->value();
        conf.stepGen = ui->doubleSpinBoxGenStep->value();
        conf.terminalCustomized = ui->checkBoxPersonalized->isChecked();

        if(conf.maxGen < conf.minGen)
            qSwap(conf.maxGen, conf.minGen);

        if(conf.terminalCustomized) {
            conf.abscisseOrdonneeValuesInPourcent = ui->checkBoxXYValuesInPourcent->isChecked();
            conf.minAbscisse = ui->doubleSpinBoxMinX->value();
            conf.maxAbscisse = ui->doubleSpinBoxMaxX->value();
            conf.minOrdonnee = ui->doubleSpinBoxMinY->value();
            conf.maxOrdonnee = ui->doubleSpinBoxMaxY->value();

            if(conf.maxAbscisse < conf.minAbscisse)
                qSwap(conf.maxAbscisse, conf.minAbscisse);

            if(conf.maxOrdonnee < conf.minOrdonnee)
                qSwap(conf.maxOrdonnee, conf.minOrdonnee);
        }
    }

    return conf;
}

bool ComputeProfilesConfiguration::canAcceptIt(QString *err) const
{
    if(ui->radioButtonPersonalized->isChecked()) {
        if(ui->checkBoxZValuesInPourcent->isChecked()) {
            CHECKPOURCENT(ui->doubleSpinBoxMinZ->value(), "borne z minimum");
        }

        if(ui->checkBoxPersonalized->isChecked()) {
            if(ui->checkBoxXYValuesInPourcent->isChecked()) {
                CHECKPOURCENT(ui->doubleSpinBoxMinX->value(), "borne x minimum");
                CHECKPOURCENT(ui->doubleSpinBoxMinY->value(), "borne y minimum");
            }
        }
    }
    //This is a test for error code value input
    if(ui->doubleSpinBoxMinValue->value() < 0 || ui->doubleSpinBoxMaxValue->value() < 0 ) {
        if(ui->doubleSpinBoxMinValue->value() >= 0 || ui->doubleSpinBoxMaxValue->value() >= 0)
        if(err != NULL){
            *err = tr("Les valeurs minimum et maximum doivent être négatives lors du calcul de codes d'erreurs.");
            return false;
        }
    }

    return true;
}

bool ComputeProfilesConfiguration::isSettingsModified() const
{
    return false;
}

bool ComputeProfilesConfiguration::updateElement(QString *err)
{
    return canAcceptIt(err);
}

void ComputeProfilesConfiguration::on_pushButtonDefaultConfiguration_clicked() {
    setConfiguration(m_defaultConfiguration);
}

void ComputeProfilesConfiguration::on_comboBoxOrdonnee_currentIndexChanged(int index)
{
    ui->comboBoxGen->clear();

    if(index == 0) {
        ui->comboBoxGen->addItem("Y", "y");
        ui->comboBoxGen->addItem("Z", "z");
    } else if(index == 1) {
        ui->comboBoxGen->addItem("X", "x");
        ui->comboBoxGen->addItem("Z", "z");
    } else {
        ui->comboBoxGen->addItem("X", "x");
        ui->comboBoxGen->addItem("Y", "y");
    }
}

void ComputeProfilesConfiguration::on_radioButtonAllGrid_toggled(bool checked)
{
    ui->doubleSpinBoxMinZ->setEnabled(!checked);
    ui->doubleSpinBoxMaxZ->setEnabled(!checked);
    ui->doubleSpinBoxGenStep->setEnabled(!checked);
    ui->checkBoxPersonalized->setEnabled(!checked);
    ui->checkBoxZValuesInPourcent->setEnabled(!checked);
    ui->comboBoxGen->setEnabled(!checked);

    if(checked)
        ui->checkBoxPersonalized->setChecked(false);
}

void ComputeProfilesConfiguration::on_checkBoxPersonalized_toggled(bool checked)
{
    ui->checkBoxXYValuesInPourcent->setEnabled(checked);
    ui->doubleSpinBoxMinX->setEnabled(checked);
    ui->doubleSpinBoxMaxX->setEnabled(checked);
    ui->doubleSpinBoxMinY->setEnabled(checked);
    ui->doubleSpinBoxMaxY->setEnabled(checked);
}
