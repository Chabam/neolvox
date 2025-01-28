#include "gplycolorconfiguration.h"
#include "ui_gplycolorconfiguration.h"

#include "readers/tools/ply/views/tools/plyviewtools.h"

GPlyColorConfiguration::GPlyColorConfiguration(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GPlyColorConfiguration)
{
    ui->setupUi(this);
}

GPlyColorConfiguration::~GPlyColorConfiguration()
{
    delete ui;
}

bool GPlyColorConfiguration::isValid() const
{
    if((ui->comboBoxRed->count() == 0)
            || (ui->comboBoxRed->currentData().toInt() == -1))
        return false;

    if((ui->comboBoxGreen->count() == 0)
            || (ui->comboBoxGreen->currentData().toInt() == -1))
        return false;

    if((ui->comboBoxBlue->count() == 0)
        || (ui->comboBoxBlue->currentData().toInt() == -1))
        return false;

    return true;
}

void GPlyColorConfiguration::setConfiguration(const PlyColorConfiguration &config)
{
    setPlyElement(config.elementIndex);

    ui->comboBoxRed->setCurrentIndex(ui->comboBoxRed->findData(config.rPropertyIndex));
    ui->comboBoxGreen->setCurrentIndex(ui->comboBoxGreen->findData(config.gPropertyIndex));
    ui->comboBoxBlue->setCurrentIndex(ui->comboBoxBlue->findData(config.bPropertyIndex));
    ui->comboBoxAlpha->setCurrentIndex(ui->comboBoxAlpha->findData(config.aPropertyIndex));
}

PlyColorConfiguration GPlyColorConfiguration::getConfiguration() const
{
    PlyColorConfiguration config;
    config.elementIndex = getPlyElement();
    config.rPropertyIndex = ui->comboBoxRed->currentData().toInt();
    config.gPropertyIndex = ui->comboBoxGreen->currentData().toInt();
    config.bPropertyIndex = ui->comboBoxBlue->currentData().toInt();
    config.aPropertyIndex = ui->comboBoxAlpha->currentData().toInt();

    return config;
}

void GPlyColorConfiguration::resetUI()
{
    ui->comboBoxRed->clear();
    ui->comboBoxGreen->clear();
    ui->comboBoxBlue->clear();
    ui->comboBoxAlpha->clear();

    QList<PlyElement> elements = m_header.getElements();

    if((m_elementIndex >=0)
            && (m_elementIndex < elements.size()))
    {
        const PlyElement& element = elements.at(m_elementIndex);

        ui->comboBoxRed->addItem("", -1);
        ui->comboBoxGreen->addItem("", -1);
        ui->comboBoxBlue->addItem("", -1);
        ui->comboBoxAlpha->addItem("", -1);

        PlyViewTools::staticFillPropertiesInComboBox(ui->comboBoxRed, element);
        PlyViewTools::staticFillPropertiesInComboBox(ui->comboBoxGreen, element);
        PlyViewTools::staticFillPropertiesInComboBox(ui->comboBoxBlue, element);
        PlyViewTools::staticFillPropertiesInComboBox(ui->comboBoxAlpha, element);

        PlyViewTools::staticFindPropertyAndSetCurrentIndexInComboBox(ui->comboBoxRed, element, [](const PlyProperty &property) -> int { return staticFindColorProperty(property, QStringList() << "red" << "r"); });
        PlyViewTools::staticFindPropertyAndSetCurrentIndexInComboBox(ui->comboBoxGreen, element, [](const PlyProperty &property) -> int { return staticFindColorProperty(property, QStringList() << "green" << "g"); });
        PlyViewTools::staticFindPropertyAndSetCurrentIndexInComboBox(ui->comboBoxBlue, element, [](const PlyProperty &property) -> int { return staticFindColorProperty(property, QStringList() << "blue" << "b"); });
        PlyViewTools::staticFindPropertyAndSetCurrentIndexInComboBox(ui->comboBoxAlpha, element, [](const PlyProperty &property) -> int { return staticFindColorProperty(property, QStringList() << "alpha" << "a"); });
    }
}

int GPlyColorConfiguration::staticFindColorProperty(const PlyProperty &property, const QStringList &wordsToFindByBestScore)
{
    int score = wordsToFindByBestScore.size();

    foreach (const QString& wordToFind, wordsToFindByBestScore) {

        if(property.getDisplayableName().indexOf(wordToFind) != -1)
            return score;

        --score;
    }

    return 0;
}
