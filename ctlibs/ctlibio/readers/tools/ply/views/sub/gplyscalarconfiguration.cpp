#include "gplyscalarconfiguration.h"
#include "ui_gplyscalarconfiguration.h"

#include "readers/tools/ply/views/tools/plyviewtools.h"

GPlyScalarConfiguration::GPlyScalarConfiguration(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GPlyScalarConfiguration)
{
    ui->setupUi(this);
}

GPlyScalarConfiguration::~GPlyScalarConfiguration()
{
    delete ui;
}

bool GPlyScalarConfiguration::isValid() const
{
    if((ui->comboBoxScalar->count() == 0)
            || (ui->comboBoxScalar->currentData().toInt() == -1))
        return false;

    return true;
}

void GPlyScalarConfiguration::setConfiguration(const PlyScalarConfiguration &config)
{
    setPlyElement(config.elementIndex);

    ui->comboBoxScalar->setCurrentIndex(ui->comboBoxScalar->findData(config.sPropertyIndex));
}

PlyScalarConfiguration GPlyScalarConfiguration::getConfiguration() const
{
    PlyScalarConfiguration config;
    config.elementIndex = getPlyElement();
    config.sPropertyIndex = ui->comboBoxScalar->currentData().toInt();

    return config;
}

void GPlyScalarConfiguration::resetUI()
{
    ui->comboBoxScalar->clear();

    QList<PlyElement> elements = m_header.getElements();

    if((m_elementIndex >=0)
            && (m_elementIndex < elements.size()))
    {
        const PlyElement& element = elements.at(m_elementIndex);

        ui->comboBoxScalar->addItem("", -1);

        PlyViewTools::staticFillPropertiesInComboBox(ui->comboBoxScalar, element);

        PlyViewTools::staticFindPropertyAndSetCurrentIndexInComboBox(ui->comboBoxScalar, element, [](const PlyProperty &property) -> int { return staticFindProperty(property, "scalar"); });
    }
}
