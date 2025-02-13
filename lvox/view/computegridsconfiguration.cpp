#include "computegridsconfiguration.h"
#include "qmessagebox.h"
#include <QDebug>

ComputeGridsConfiguration::ComputeGridsConfiguration(QWidget *parent) :
    CT_AbstractConfigurableWidget(parent),
    _ui(new Ui::ComputeGridsConfiguration)

{
    // UI
    _ui->setupUi(this);

    _vegetationTypeCollection.insert(VegetationType::NeedleFromDimension, tr("Needle"));
    _vegetationTypeCollection.insert(VegetationType::BroadLeafFromDimension, tr("BroadLeaf"));
    _vegetationTypeCollection.insert(VegetationType::NeedleFromFlatArea, tr("Needle Area"));
    _vegetationTypeCollection.insert(VegetationType::BroadLeafFromFlatArea, tr("BroadLead Area"));

    _ui->estimationMethods->addItem(_vegetationTypeCollection[VegetationType::NeedleFromDimension]);
    _ui->estimationMethods->addItem(_vegetationTypeCollection[VegetationType::BroadLeafFromDimension]);
    _ui->estimationMethods->addItem(_vegetationTypeCollection[VegetationType::NeedleFromFlatArea]);
    _ui->estimationMethods->addItem(_vegetationTypeCollection[VegetationType::BroadLeafFromFlatArea]);

    _currentVegetationType = VegetationType::NeedleFromDimension;

    _gridModesCollection.insert(GridMode::BoundingBoxOfTheScene, tr("Scene's bounding box"));
    _gridModesCollection.insert(GridMode::RelativeToCoordinates, tr("Relative to coordinates"));
    _gridModesCollection.insert(GridMode::RelativeToCoordinatesAndCustomDimensions, tr("Relative to following coordinates with custom dimensions"));
    _gridModesCollection.insert(GridMode::CenteredOnCoordinatesAndCustomDimensions, tr("Centered on following coordinates with custom dimensions"));
    _gridModesCollection.insert(GridMode::FromOtherGrid, tr("From other grid"));

    _ui->referenceMethods->addItem(_gridModesCollection[GridMode::BoundingBoxOfTheScene]);
    _ui->referenceMethods->addItem(_gridModesCollection[GridMode::RelativeToCoordinates]);
    _ui->referenceMethods->addItem(_gridModesCollection[GridMode::RelativeToCoordinatesAndCustomDimensions]);
    _ui->referenceMethods->addItem(_gridModesCollection[GridMode::CenteredOnCoordinatesAndCustomDimensions]);
    _ui->referenceMethods->addItem(_gridModesCollection[GridMode::FromOtherGrid]);

    _currentGridMode = GridMode::BoundingBoxOfTheScene; // Depends on the order modes are inserted

    connect(_ui->estimationMethods, &QComboBox::currentTextChanged, this, &ComputeGridsConfiguration::estimationMethodHasChanged);
    connect(_ui->referenceMethods, &QComboBox::currentTextChanged, this, &ComputeGridsConfiguration::referenceMethodHasChanged);

    updateInterface();

}

ComputeGridsConfiguration::~ComputeGridsConfiguration()
{
    delete _ui;
}

bool ComputeGridsConfiguration::isSettingsModified() const
{
    return true;
}

bool ComputeGridsConfiguration::canAcceptIt(QString *err) const
{
    Q_UNUSED(err)
    if(!(_ui->doComputeDistances->isChecked()))
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Not computing distance will give invalid results for PAD computation"));
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);

        msgBox.exec();
    }

    return true;
}

void ComputeGridsConfiguration::setCurrentProjectedAreaMethod(VegetationType vegetationType)
{
    _ui->estimationMethods->setCurrentText(_vegetationTypeCollection[vegetationType]);
    _currentVegetationType = vegetationType;
}

void ComputeGridsConfiguration::setCurrentGridMode(GridMode gridMode)
{
    _ui->referenceMethods->setCurrentText(_gridModesCollection[gridMode]);
    _currentGridMode = gridMode;
}

void ComputeGridsConfiguration::setCoordinates(Eigen::Vector3d coordinates)
{
    _ui->coordinatesX->setValue(coordinates.x());
    _ui->coordinatesY->setValue(coordinates.y());
    _ui->coordinatesZ->setValue(coordinates.z());
}

void ComputeGridsConfiguration::setDimensions(Eigen::Vector3d dimensions)
{
    _ui->dimensionX->setValue(dimensions.x());
    _ui->dimensionY->setValue(dimensions.y());
    _ui->dimensionZ->setValue(dimensions.z());
}

void ComputeGridsConfiguration::setComputeDistances(bool value)
{
    _ui->doComputeDistances->setChecked(value);
}

void ComputeGridsConfiguration::setResolution(double resolution)
{
    // TODO: make it one dimmension (FC)
    _ui->gridResX->setValue(resolution);
    _ui->gridResY->setValue(resolution);
    _ui->gridResZ->setValue(resolution);
}

void ComputeGridsConfiguration::setFlatArea(double area)
{
    _ui->projectedAreaSpinBox->setValue(area);
}

void ComputeGridsConfiguration::setLengthOrExtension1(double value)
{
    _ui->extension1SpinBox->setValue(value);
    _ui->lengthSpinBox->setValue(value);
}

void ComputeGridsConfiguration::setWidthOrExtension2(double value)
{
    _ui->extension2SpinBox->setValue(value);
    _ui->widthSpinBox->setValue(value);
}


double ComputeGridsConfiguration::getResolution() const
{
    // TODO: make it one dimmension (FC)
    return _ui->gridResX->value();
}

Eigen::Vector3d ComputeGridsConfiguration::getCoordinates() const
{
    return Eigen::Vector3d {
        _ui->coordinatesX->value(),
        _ui->coordinatesY->value(),
        _ui->coordinatesZ->value()
    };
}

Eigen::Vector3d ComputeGridsConfiguration::getDimensiosn() const
{
    return Eigen::Vector3d {
        _ui->dimensionX->value(),
        _ui->dimensionY->value(),
        _ui->dimensionZ->value()
    };
}

double ComputeGridsConfiguration::getFlatArea() const
{
    return _ui->projectedAreaSpinBox->value();
}

ComputeGridsConfiguration::VegetationType ComputeGridsConfiguration::getElementProjectedAreaMethods() const
{
    return _currentVegetationType;
}

ComputeGridsConfiguration::GridMode ComputeGridsConfiguration::getGridMode() const
{
    return _currentGridMode;
}

bool ComputeGridsConfiguration::getComputeDistances() const
{
    return _ui->doComputeDistances->isChecked();
}

double ComputeGridsConfiguration::getLengthOrExtension1() const
{
    if(_currentVegetationType == NeedleFromDimension)
        return _ui->extension1SpinBox->value();
    else
        return _ui->lengthSpinBox->value();
}

double ComputeGridsConfiguration::getWidthOrExtension2() const
{
    if(_currentVegetationType == NeedleFromDimension)
        return _ui->extension2SpinBox->value();
    else
        return _ui->widthSpinBox->value();
}



///////////////////////////////////////////////////////////
// SLOTS
///////////////////////////////////////////////////////////

bool ComputeGridsConfiguration::updateElement(QString *err)
{
    Q_UNUSED(err);
    return true;
}

void ComputeGridsConfiguration::estimationMethodHasChanged(const QString s)
{
    if(_vegetationTypeCollection[VegetationType::NeedleFromDimension] == s)
        _currentVegetationType = VegetationType::NeedleFromDimension;

    if(_vegetationTypeCollection[VegetationType::BroadLeafFromDimension] == s)
        _currentVegetationType = VegetationType::BroadLeafFromDimension;

    if(_vegetationTypeCollection[VegetationType::NeedleFromFlatArea] == s)
        _currentVegetationType = VegetationType::NeedleFromFlatArea;

    if(_vegetationTypeCollection[VegetationType::BroadLeafFromFlatArea] == s)
        _currentVegetationType = VegetationType::BroadLeafFromFlatArea;

    updateInterface();
}

void ComputeGridsConfiguration::referenceMethodHasChanged(const QString s)
{
    if(_gridModesCollection[GridMode::BoundingBoxOfTheScene] == s)
        _currentGridMode = GridMode::BoundingBoxOfTheScene;

    if(_gridModesCollection[GridMode::RelativeToCoordinates] == s)
        _currentGridMode = GridMode::RelativeToCoordinates;

    if(_gridModesCollection[GridMode::RelativeToCoordinatesAndCustomDimensions] == s)
        _currentGridMode = GridMode::RelativeToCoordinatesAndCustomDimensions;

    if(_gridModesCollection[GridMode::CenteredOnCoordinatesAndCustomDimensions] == s)
        _currentGridMode = GridMode::CenteredOnCoordinatesAndCustomDimensions;

    if(_gridModesCollection[GridMode::FromGridFileParameters] == s)
        _currentGridMode = GridMode::FromGridFileParameters;

    updateInterface();
}

void ComputeGridsConfiguration::updateInterface() const
{
    switch (_currentVegetationType) {
    case NeedleFromDimension:
        _ui->needleFrame->show();
        _ui->broadleafFrame->hide();
        _ui->areaFrame->hide();
        break;
    case BroadLeafFromDimension:
        _ui->needleFrame->hide();
        _ui->broadleafFrame->show();
        _ui->areaFrame->hide();
        break;
    case NeedleFromFlatArea:
    case BroadLeafFromFlatArea:
        _ui->needleFrame->hide();
        _ui->broadleafFrame->hide();
        _ui->areaFrame->show();
        break;
    }

    switch (_currentGridMode) {
    case GridMode::RelativeToCoordinates:
        _ui->coordinatesFrame->show();
        _ui->dimensionFrame->hide();
        break;
    case GridMode::RelativeToCoordinatesAndCustomDimensions:
    case GridMode::CenteredOnCoordinatesAndCustomDimensions:
        _ui->coordinatesFrame->show();
        _ui->dimensionFrame->show();
        break;
    case GridMode::FromGridFileParameters:
    case GridMode::BoundingBoxOfTheScene:
    case GridMode::FromOtherGrid:
        _ui->coordinatesFrame->hide();
        _ui->dimensionFrame->hide();
        break;
    }
}
