#include "pbg_csvconfigurationdialog.h"
#include "ui_pbg_csvconfigurationdialog.h"

#include "exporters/csv/pb_csvexporterconfiguration.h"

PBG_CSVConfigurationDialog::PBG_CSVConfigurationDialog(PB_CSVExporterConfiguration &configuration,
                                                       QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PBG_CSVConfigurationDialog)
{
    ui->setupUi(this);

    ui->widgetPreview->setConfiguration(&configuration);
    ui->widgetDataRefList->setList(configuration.list());

    connect(ui->widgetDataRefList, SIGNAL(attributeChoosed(QString)), ui->widgetPreview, SLOT(setItemAttribute(QString)));
}

PBG_CSVConfigurationDialog::~PBG_CSVConfigurationDialog()
{
    delete ui;
}

void PBG_CSVConfigurationDialog::accept()
{
    ui->widgetPreview->updateConfiguration();

    QDialog::accept();
}
