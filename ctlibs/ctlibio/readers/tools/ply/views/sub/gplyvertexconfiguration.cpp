#include "gplyvertexconfiguration.h"
#include "ui_gplyvertexconfiguration.h"

#include "readers/tools/ply/views/tools/plyviewtools.h"

GPlyVertexConfiguration::GPlyVertexConfiguration(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GPlyVertexConfiguration)
{
    ui->setupUi(this);
}

GPlyVertexConfiguration::~GPlyVertexConfiguration()
{
    delete ui;
}

bool GPlyVertexConfiguration::isValid() const
{
    if((ui->comboBoxX->count() == 0)
            || (ui->comboBoxX->currentData().toInt() == -1))
        return false;

    if((ui->comboBoxY->count() == 0)
            || (ui->comboBoxY->currentData().toInt() == -1))
        return false;

    if((ui->comboBoxZ->count() == 0)
        || (ui->comboBoxZ->currentData().toInt() == -1))
        return false;

    return true;
}

void GPlyVertexConfiguration::setConfiguration(const PlyVertexConfiguration &config)
{
    setPlyElement(config.elementIndex);

    ui->comboBoxX->setCurrentIndex(ui->comboBoxX->findData(config.xPropertyIndex));
    ui->comboBoxY->setCurrentIndex(ui->comboBoxY->findData(config.yPropertyIndex));
    ui->comboBoxZ->setCurrentIndex(ui->comboBoxZ->findData(config.zPropertyIndex));
}

PlyVertexConfiguration GPlyVertexConfiguration::getConfiguration() const
{
    PlyVertexConfiguration config;
    config.elementIndex = getPlyElement();
    config.xPropertyIndex = ui->comboBoxX->currentData().toInt();
    config.yPropertyIndex = ui->comboBoxY->currentData().toInt();
    config.zPropertyIndex = ui->comboBoxZ->currentData().toInt();

    return config;
}

void GPlyVertexConfiguration::resetUI()
{
    ui->comboBoxX->clear();
    ui->comboBoxY->clear();
    ui->comboBoxZ->clear();

    QList<PlyElement> elements = m_header.getElements();

    if((m_elementIndex >=0)
            && (m_elementIndex < elements.size()))
    {
        const PlyElement& element = elements.at(m_elementIndex);

        ui->comboBoxX->addItem("", -1);
        ui->comboBoxY->addItem("", -1);
        ui->comboBoxZ->addItem("", -1);

        PlyViewTools::staticFillPropertiesInComboBox(ui->comboBoxX, element);
        PlyViewTools::staticFillPropertiesInComboBox(ui->comboBoxY, element);
        PlyViewTools::staticFillPropertiesInComboBox(ui->comboBoxZ, element);

        PlyViewTools::staticFindPropertyAndSetCurrentIndexInComboBox(ui->comboBoxX, element, [](const PlyProperty &property) -> int { return staticFindProperty(property, "x"); });
        PlyViewTools::staticFindPropertyAndSetCurrentIndexInComboBox(ui->comboBoxY, element, [](const PlyProperty &property) -> int { return staticFindProperty(property, "y"); });
        PlyViewTools::staticFindPropertyAndSetCurrentIndexInComboBox(ui->comboBoxZ, element, [](const PlyProperty &property) -> int { return staticFindProperty(property, "z"); });
    }
}
