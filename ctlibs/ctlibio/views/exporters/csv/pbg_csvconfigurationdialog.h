#ifndef PBG_CSVCONFIGURATIONDIALOG_H
#define PBG_CSVCONFIGURATIONDIALOG_H

#include <QDialog>

class PB_CSVExporterConfiguration;

namespace Ui {
class PBG_CSVConfigurationDialog;
}

class PBG_CSVConfigurationDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit PBG_CSVConfigurationDialog(PB_CSVExporterConfiguration &configuration,
                                        QWidget *parent = nullptr);
    ~PBG_CSVConfigurationDialog();

private:
    Ui::PBG_CSVConfigurationDialog *ui;

public slots:

    void accept();
};

#endif // PBG_CSVCONFIGURATIONDIALOG_H
