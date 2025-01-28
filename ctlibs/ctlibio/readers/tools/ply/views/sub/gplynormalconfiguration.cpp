#include "gplynormalconfiguration.h"
#include "ui_gplynormalconfiguration.h"

#include "readers/tools/ply/views/tools/plyviewtools.h"

GPlyNormalConfiguration::GPlyNormalConfiguration(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GPlyNormalConfiguration)
{
    ui->setupUi(this);
}

GPlyNormalConfiguration::~GPlyNormalConfiguration()
{
    delete ui;
}

bool GPlyNormalConfiguration::isValid() const
{
    if((ui->comboBoxNx->count() == 0)
            || (ui->comboBoxNx->currentData().toInt() == -1))
        return false;

    if((ui->comboBoxNy->count() == 0)
            || (ui->comboBoxNy->currentData().toInt() == -1))
        return false;

    if((ui->comboBoxNz->count() == 0)
        || (ui->comboBoxNz->currentData().toInt() == -1))
        return false;

    return true;
}

void GPlyNormalConfiguration::setConfiguration(const PlyNormalConfiguration &config)
{
    setPlyElement(config.elementIndex);

    ui->comboBoxNx->setCurrentIndex(ui->comboBoxNx->findData(config.nxPropertyIndex));
    ui->comboBoxNy->setCurrentIndex(ui->comboBoxNy->findData(config.nyPropertyIndex));
    ui->comboBoxNz->setCurrentIndex(ui->comboBoxNz->findData(config.nzPropertyIndex));
    ui->comboBoxCurvature->setCurrentIndex(ui->comboBoxCurvature->findData(config.ncPropertyIndex));
}

PlyNormalConfiguration GPlyNormalConfiguration::getConfiguration() const
{
    PlyNormalConfiguration config;
    config.elementIndex = getPlyElement();
    config.nxPropertyIndex = ui->comboBoxNx->currentData().toInt();
    config.nyPropertyIndex = ui->comboBoxNy->currentData().toInt();
    config.nzPropertyIndex = ui->comboBoxNz->currentData().toInt();
    config.ncPropertyIndex = ui->comboBoxCurvature->currentData().toInt();

    return config;
}

void GPlyNormalConfiguration::resetUI()
{
    ui->comboBoxNx->clear();
    ui->comboBoxNy->clear();
    ui->comboBoxNz->clear();
    ui->comboBoxCurvature->clear();

    QList<PlyElement> elements = m_header.getElements();

    if((m_elementIndex >=0)
            && (m_elementIndex < elements.size()))
    {
        const PlyElement& element = elements.at(m_elementIndex);

        ui->comboBoxNx->addItem("", -1);
        ui->comboBoxNy->addItem("", -1);
        ui->comboBoxNz->addItem("", -1);
        ui->comboBoxCurvature->addItem("", -1);

        PlyViewTools::staticFillPropertiesInComboBox(ui->comboBoxNx, element);
        PlyViewTools::staticFillPropertiesInComboBox(ui->comboBoxNy, element);
        PlyViewTools::staticFillPropertiesInComboBox(ui->comboBoxNz, element);
        PlyViewTools::staticFillPropertiesInComboBox(ui->comboBoxCurvature, element);

        PlyViewTools::staticFindPropertyAndSetCurrentIndexInComboBox(ui->comboBoxNx, element, [](const PlyProperty &property) -> int { return staticFindNormalProperty(property, "x"); });
        PlyViewTools::staticFindPropertyAndSetCurrentIndexInComboBox(ui->comboBoxNy, element, [](const PlyProperty &property) -> int { return staticFindNormalProperty(property, "y"); });
        PlyViewTools::staticFindPropertyAndSetCurrentIndexInComboBox(ui->comboBoxNz, element, [](const PlyProperty &property) -> int { return staticFindNormalProperty(property, "z"); });
        PlyViewTools::staticFindPropertyAndSetCurrentIndexInComboBox(ui->comboBoxCurvature, element, [](const PlyProperty &property) -> int { return staticFindProperty(property, "curvature"); });
    }
}

int GPlyNormalConfiguration::staticFindNormalProperty(const PlyProperty &property, const QString &wordToFind)
{
    if(property.getDisplayableName().indexOf("normal") != -1) {
        if(property.getDisplayableName().indexOf(wordToFind) != -1)
            return 3;

        return 2;
    }

    if(property.getDisplayableName().indexOf("n" + wordToFind) != -1)
        return 3;

    if(property.getDisplayableName().indexOf(wordToFind) != -1)
        return 1;

    return 0;
}
