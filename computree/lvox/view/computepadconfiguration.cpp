#include "computepadconfiguration.h"

///////////////////////////////////////////////////////////
/// PUBLIC METHODS
///////////////////////////////////////////////////////////

ComputePadConfiguration::ComputePadConfiguration(QWidget *parent) :
    CT_AbstractConfigurableWidget(parent),
    ui(new Ui::ComputePadConfiguration),
    _c()
{
    ui->setupUi(this);        
}

ComputePadConfiguration::~ComputePadConfiguration()
{
    delete ui;
}

void ComputePadConfiguration::setConfiguration(const ComputePadConfiguration::Configuration& c)
{
    _c.NThreshold = c.NThreshold;
    _c.isRelativeThreshold = c.isRelativeThreshold;
    _c.grilleSup = c.grilleSup;
    _c.methodsStatus = c.methodsStatus;
}

ComputePadConfiguration::Configuration ComputePadConfiguration::getConfiguration() const
{
    return _c;
}

bool ComputePadConfiguration::canAcceptIt(QString *err) const
{
    if(ui->comboBox->getSelectedIds().isEmpty())
    {
        if(err != NULL)
            err->append(tr("No files selected!"));

        return false;
    }

    return true;
}


bool ComputePadConfiguration::isSettingsModified() const
{
    return false;
}

bool ComputePadConfiguration::updateElement(QString *err)
{
    Q_UNUSED(err)
    if(ui->comboBox->getSelectedIds().isEmpty())
        return false;

    if(ui->estimatorN->value() == 0)
        return false;

    return true;
}

bool ComputePadConfiguration::isMethodSelected(QString methodName)
{
    return ui->comboBox->isTextInWidgets(methodName);
}

bool ComputePadConfiguration::isRelativeThreshold()
{
    return ui->isRelativeThreshold->isChecked();
}

bool ComputePadConfiguration::createSummationsGrids()
{
    return ui->createSummations->isChecked();
}

uint32_t ComputePadConfiguration::getThreshold()
{
    return ui->estimatorN->value();
}

void ComputePadConfiguration::setNThreshold(uint32_t value)
{
    ui->estimatorN->setValue(value);
}

void ComputePadConfiguration::setUseRelativeThreshold(bool value)
{
    ui->isRelativeThreshold->setChecked(value);
}

void ComputePadConfiguration::setCreateSummationGrids(bool value)
{
    ui->createSummations->setChecked(value);
}

void ComputePadConfiguration::setPADComputationMethod(QString displayableName, QString description, bool value)
{
    ui->comboBox->addItem(displayableName, value);
    ui->helpSelect->setToolTip(ui->helpSelect->toolTip() + "- " + displayableName + " : " + description + "\n");
}
