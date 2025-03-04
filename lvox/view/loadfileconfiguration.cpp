#include "loadfileconfiguration.h"
#include "ui_loadfileconfiguration.h"

//+test libs
#include <QDebug>
//-test libs

#include <QFileDialog>
#include <QMessageBox>

#include "ct_reader/abstract/ct_abstractreader.h"
#include "ct_reader/extensions/ct_readerpointsfilteringextension.h"
//#include "ct_view/ct_stepconfigurabledialog.h"

#include "tools/lvox3_scannerutils.h"

#include "loadfileconfigutil.h"

LoadFileConfiguration::LoadFileConfiguration(QWidget *parent) :
    CT_AbstractConfigurableWidget(parent),
    ui(new Ui::LoadFileConfiguration)
{
    ui->setupUi(this);
    ui->listWidgetFiles->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(ui->pushButtonModifyFile, SIGNAL(clicked(bool)), this, SLOT(modifySelectedFile()));
    connect(ui->pushButtonImportFile, SIGNAL(clicked(bool)), this, SLOT(importFile()));
    connect(ui->pushButtonExportFiles, SIGNAL(clicked(bool)), this, SLOT(exportToInFile()));
    connect(ui->pushButtonDeleteFile, SIGNAL(clicked(bool)), this, SLOT(removeSelectedFile()));
    connect(ui->pushButtonDeleteAllFile, SIGNAL(clicked(bool)), this, SLOT(removeAllFile()));
    connect(ui->pushButtonAddFile, SIGNAL(clicked(bool)), this, SLOT(addFile()));

    /* load scanner types */
    for (const ScannerDefinitionStruct& scanDef: LVOX3_ScannerUtils::getScannerDefinitions()) {
        ui->comboBoxScannerType->addItem(scanDef.name, scanDef.id);
    }

    on_listWidgetFiles_currentRowChanged(-1);
}

LoadFileConfiguration::~LoadFileConfiguration()
{
    delete ui;
}

void LoadFileConfiguration::setReaders(const QList<CT_AbstractReader *>& readers)
{
    m_availableReaders = readers;

    initReaderComboBox();
}

void LoadFileConfiguration::setCurrentReaderByClassName(const QString &readerClassName)
{
    int i = 0;

    foreach (CT_AbstractReader* reader, m_availableReaders) {
        if(reader->uniqueName() == readerClassName) {
            ui->comboBoxReaderType->setCurrentIndex(i);
            return;
        }

        ++i;
    }
}

void LoadFileConfiguration::setConfiguration(const QList<LoadFileConfiguration::Configuration> &configs)
{
    ui->listWidgetFiles->clear();
    m_filesScannerConfiguration.clear();

    foreach (const Configuration& c, configs) {
        QListWidgetItem* item = new QListWidgetItem(c.filepath);

        m_filesScannerConfiguration.insert(item, c);
        ui->listWidgetFiles->addItem(item);
        ui->comboBoxScannerType->setCurrentIndex(ui->comboBoxScannerType->findData(c.scannerId));
    }

    if(!configs.isEmpty()) {
        int last = ui->listWidgetFiles->count() - 1;
        QListWidgetItem* item = ui->listWidgetFiles->item(last);
        ui->listWidgetFiles->setCurrentItem(item);
        editItem(item);
    }
}

void LoadFileConfiguration::setFilterPointsOrigin(bool enable)
{
    ui->checkBoxFilterPointsOrigin->setChecked(enable);
}

bool LoadFileConfiguration::isFilterPointsOrigin() const
{
    return ui->checkBoxFilterPointsOrigin->isChecked();
}

void LoadFileConfiguration::setRestrictScene(bool enable)
{
    ui->checkBoxRestrictScene->setChecked(enable);
}

bool LoadFileConfiguration::isRestrictScene() const
{
    return ui->checkBoxRestrictScene->isChecked();
}

/**
 * @brief sets the value of restrict scene spinbox
 */
void LoadFileConfiguration::setRestrictRadius(int radius)
{
    ui->spinBoxRestrictRadius->setValue(radius);
}

/**
 * @brief returns if the user requested restrict radius
 */
int LoadFileConfiguration::getRestrictRadius() const
{
    return ui->spinBoxRestrictRadius->value();
}

//The get configuration reorders the items in the QMap based on how they were inserted in the widget for consistency and for new functions
QList<LoadFileConfiguration::Configuration> LoadFileConfiguration::getConfiguration() const
{
    QList<Configuration> configList;
    QMap<QListWidgetItem*, Configuration> tempConfigList = m_filesScannerConfiguration;
    int count = ui->listWidgetFiles->count();
     if(ui->listWidgetFiles->count() != 0){
        for(int i = 0; i < count; i++)
        {
            QListWidgetItem* itemKey = ui->listWidgetFiles->item(i);
            QMap<QListWidgetItem*, Configuration>::iterator j = tempConfigList.find(itemKey);
            int x = 0;
            while (j != tempConfigList.end() && j.key() == itemKey) {
                configList.append(j.value());
                j++;
                x++;
            }
        }
        return configList;
     }
     return configList;
}

CT_AbstractReader* LoadFileConfiguration::getReaderToUse() const
{
    return m_availableReaders.at(ui->comboBoxReaderType->currentIndex());
}

bool LoadFileConfiguration::canAcceptIt(QString *err) const
{
    if(m_filesScannerConfiguration.isEmpty())
    {
        if(err != NULL)
            err->append(tr("No files selected!"));

        return false;
    }

    return true;
}

bool LoadFileConfiguration::isSettingsModified() const
{
    return false;
}

bool LoadFileConfiguration::updateElement(QString *err)
{
    Q_UNUSED(err)
    if(m_filesScannerConfiguration.isEmpty())
        return false;

    QListWidgetItem* item = getSelectedItem();

    if((item != NULL) && !item->text().isEmpty())
        updateConfiguration(item);

    return true;
}

void LoadFileConfiguration::initReaderComboBox()
{
    ui->comboBoxReaderType->clear();

    foreach (CT_AbstractReader* reader, m_availableReaders) {
        ui->comboBoxReaderType->addItem(reader->uniqueName());
    }

    if(!m_availableReaders.isEmpty())
        ui->comboBoxReaderType->setCurrentIndex(0);
}

void LoadFileConfiguration::editItem(QListWidgetItem *item)
{
    Configuration c = m_filesScannerConfiguration.value(item, Configuration());

    ui->comboBoxScannerType->setCurrentIndex(ui->comboBoxScannerType->findData(c.scannerId));
    ui->checkBoxScannerClockwise->setChecked(c.clockWise);
    ui->checkBoxAngleInRadians->setChecked(c.radians);

    ui->labelFilepath->setText(tr("<i>%1</i>").arg(((item == NULL) ? tr("No file selected") : QFileInfo(c.filepath).baseName())));

    ui->doubleSpinBoxXPos->setValue(c.scannerPosition.x());
    ui->doubleSpinBoxYPos->setValue(c.scannerPosition.y());
    ui->doubleSpinBoxZPos->setValue(c.scannerPosition.z());

    ui->doubleSpinBoxHRes->setValue(c.scannerResolution.x());
    ui->doubleSpinBoxVRes->setValue(c.scannerResolution.y());

    ui->doubleSpinBoxStartPhi->setValue(c.scannerPhiRange.x());
    ui->doubleSpinBoxEndPhi->setValue(c.scannerPhiRange.y());

    ui->doubleSpinBoxStartTheta->setValue(c.scannerThetaRange.x());
    ui->doubleSpinBoxEndTheta->setValue(c.scannerThetaRange.y());

    ui->doubleSpinBoxXDir->setValue(c.scannerDirection.x());
    ui->doubleSpinBoxYDir->setValue(c.scannerDirection.y());
    ui->doubleSpinBoxZDir->setValue(c.scannerDirection.z());

    setCurrentScannerType(c.scannerId);
}

void LoadFileConfiguration::updateConfiguration(QListWidgetItem *item)
{
    if(!m_filesScannerConfiguration.contains(item))
        return;

    Configuration c = m_filesScannerConfiguration.value(item);

    c.scannerId = static_cast<ScannerTypeEnum>(ui->comboBoxScannerType->currentData().toInt());
    c.clockWise = ui->checkBoxScannerClockwise->isChecked();
    c.radians = ui->checkBoxAngleInRadians->isChecked();

    c.scannerPosition.x() = ui->doubleSpinBoxXPos->value();
    c.scannerPosition.y() = ui->doubleSpinBoxYPos->value();
    c.scannerPosition.z() = ui->doubleSpinBoxZPos->value();

    c.scannerResolution.x() = ui->doubleSpinBoxHRes->value();
    c.scannerResolution.y() = ui->doubleSpinBoxVRes->value();

    c.scannerPhiRange.x() = ui->doubleSpinBoxStartPhi->value();
    c.scannerPhiRange.y() = ui->doubleSpinBoxEndPhi->value();

    c.scannerThetaRange.x() = ui->doubleSpinBoxStartTheta->value();
    c.scannerThetaRange.y() = ui->doubleSpinBoxEndTheta->value();

    c.scannerDirection.x() = ui->doubleSpinBoxXDir->value();
    c.scannerDirection.y() = ui->doubleSpinBoxYDir->value();
    c.scannerDirection.z() = ui->doubleSpinBoxZDir->value();

    m_filesScannerConfiguration.insert(item, c);
}

CT_AbstractReader* LoadFileConfiguration::getSelectedReader() const
{
    if((ui->comboBoxReaderType->count() == 0)
            || (ui->comboBoxReaderType->currentIndex() >= m_availableReaders.size()))
        return NULL;

    return m_availableReaders.at(ui->comboBoxReaderType->currentIndex());
}

QListWidgetItem* LoadFileConfiguration::getSelectedItem() const
{
    return ui->listWidgetFiles->currentItem();
}

void LoadFileConfiguration::addFile()
{
    QStringList list;
    CT_AbstractReader* reader = getSelectedReader();


    if(chooseFiles(list, true)) {
        foreach (QString filepath, list) {
            QListWidgetItem* item = new QListWidgetItem(filepath);
            Eigen::Vector3d itemScannerPosition;
            Configuration c;
            //+Parsing file for scanner coordinates, if it's a .xyb file or .ptx file
            if(reader->uniqueName() == "CT_Reader_XYB"){

                itemScannerPosition = LoadFileConfigUtil::loadFileScannerCoordinatesXYB(filepath);
                c.scannerPosition = itemScannerPosition;
            }else if(reader->uniqueName() == "CT_Reader_PTX"){
                itemScannerPosition = LoadFileConfigUtil::loadFileScannerCoordinatesPTX(filepath);
                c.scannerPosition = itemScannerPosition;
            }
            //-Parsing file for scanner coordinates
            c.filepath = filepath;

            m_filesScannerConfiguration.insert(item, c);

            ui->listWidgetFiles->addItem(item);
        }

        int last = ui->listWidgetFiles->count()-1;
        QListWidgetItem* item = ui->listWidgetFiles->item(last);
        ui->listWidgetFiles->setCurrentItem(item);
        editItem(item);
    }
}

void LoadFileConfiguration::importFile()
{
    // FIXME: set sensible default directory
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("Scene (*.in)"));
    dialog.setViewMode(QFileDialog::Detail);

    if (dialog.exec() != QDialog::Accepted)
        return;

    QStringList fileNames = dialog.selectedFiles();
    QList<ConfItem> conf;
    try {
        LoadFileConfigUtil::loadInFile(fileNames.at(0), conf);
    } catch (LvoxConfigError err) {
        QString msg(tr("<b>Failed to load file %1</b></br><p>%2<p>"));
        msg = msg.arg(fileNames.at(0)).arg(err.m_msg);
        QMessageBox::critical(this, tr("Import error"), msg);
        return;
    }

    QStringList notValid;
    for (const ConfItem& item: conf) {
        QFileInfo info(item.filepath);
        if (!info.exists() || !info.isFile()) {
            notValid.append(item.filepath);
        }
    }
    if (!notValid.isEmpty()) {
        QString msg(tr("<b>%n Files(s) not found:</b></br><p>%1<p>", "", notValid.count()));
        QString notValidList = notValid.join("</br>");
        msg = msg.arg(notValidList);
        QMessageBox::critical(this, tr("Import error"), msg);
        return;
    }

    setConfiguration(conf);
}

//Exports all loaded files to a .in file
void LoadFileConfiguration::exportToInFile()
{
    QString dir = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                        "/home/untitled.in",
                                                        tr("In File (*.in)"));
    QList<ConfItem> conf = getConfiguration(); //Get every item

    QFile file(dir);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;

    //For every item in the configuration
    for (const ConfItem& item: conf) {
        QStringList foldernameToSplit =  item.filepath.split("/");
        QString foldername = foldernameToSplit.value(foldernameToSplit.length() - 1);

        QTextStream out(&file);
        out << foldername << " FARO " << item.scannerPosition.x()<<" "<<item.scannerPosition.y()<<" "<<item.scannerPosition.z()<<" ";
        out << item.scannerResolution.x()<<" "<<item.scannerThetaRange.x()<<" "<<item.scannerThetaRange.y()<<" ";
        out << item.scannerPhiRange.x()<<" "<<item.scannerPhiRange.y()<<"\n";
    }

    file.close();
}

void LoadFileConfiguration::modifySelectedFile()
{
    QListWidgetItem* item = getSelectedItem();
    CT_AbstractReader* reader = getSelectedReader();
    if(item == NULL)
        return;

    QStringList list;

    if(chooseFiles(list, false)) {
        item->setText(list.first());
        Eigen::Vector3d itemScannerPosition;
        Configuration c = m_filesScannerConfiguration.value(item);
        //+Parsing file for scanner coordinates, if it's a .xyb file
        if(reader->uniqueName() == "CT_Reader_XYB"){
            itemScannerPosition = LoadFileConfigUtil::loadFileScannerCoordinatesXYB(list.first());
            c.scannerPosition = itemScannerPosition;
        }else if(reader->uniqueName() == "CT_Reader_PTX"){
            itemScannerPosition = LoadFileConfigUtil::loadFileScannerCoordinatesPTX(list.first());
            c.scannerPosition = itemScannerPosition;
        }
        //-Parsing file for scanner coordinates, if it's a .xyb file
        c.filepath = list.first();

        m_filesScannerConfiguration.insert(item, c);
        editItem(item);
    }
}

bool LoadFileConfiguration::chooseFiles(QStringList& filepath, bool multiple)
{
    CT_AbstractReader* reader = getSelectedReader();

    if(reader == NULL) {
        QMessageBox::critical(this, tr("Error"), tr("No file reader available!"));
        return false;
    }

    QString formats;

    QListIterator<FileFormat> it(reader->readableFormats());

    while(it.hasNext()) {
        QListIterator<QString> itS(it.next().suffixes());

        while(itS.hasNext())
            formats += "*." + itS.next() + " ";
    }

    formats = formats.trimmed();

    if(multiple)
        filepath.append(QFileDialog::getOpenFileNames(this, tr("Choose a file"), "", tr("Compatible files (%1)").arg(formats)));
    else {
        QString fp = QFileDialog::getOpenFileName(this, tr("Choose a file"), "", tr("Compatible file (%1)").arg(formats));

        if(!fp.isEmpty())
            filepath.append(fp);
    }

    return !filepath.isEmpty();
}

void LoadFileConfiguration::removeSelectedFile()
{
    QListWidgetItem* item = getSelectedItem();

    if(item == NULL)
        return;

    m_filesScannerConfiguration.remove(item);

    delete item;
}

void LoadFileConfiguration::removeAllFile()
{
    m_filesScannerConfiguration.clear();
    ui->listWidgetFiles->clear();
}

void LoadFileConfiguration::on_pushButtonApplyConfigurationToNextFile_clicked()
{
    QListWidgetItem* item = getSelectedItem();

    if(item == NULL)
        return;

    updateConfiguration(item);

    int nextRow = ui->listWidgetFiles->currentRow()+1;

    if(nextRow < ui->listWidgetFiles->count()) {
        QListWidgetItem* nextItem = ui->listWidgetFiles->item(nextRow);
        Configuration cNextItem = m_filesScannerConfiguration.value(nextItem);
        Configuration c = m_filesScannerConfiguration.value(item);
        //Keeps original filepath and scanner positions
        c.filepath = cNextItem.filepath;
        c.scannerPosition = cNextItem.scannerPosition;
        m_filesScannerConfiguration.insert(nextItem, c);
        ui->listWidgetFiles->setCurrentRow(nextRow);
        editItem(nextItem);
    }
}

void LoadFileConfiguration::on_pushButtonApplyConfigurationToAll_clicked()
{
    QListWidgetItem* item = getSelectedItem();

    if(item == NULL)
        return;

    updateConfiguration(item);

    Configuration c = m_filesScannerConfiguration.value(item);

    QMutableMapIterator<QListWidgetItem*, Configuration> it(m_filesScannerConfiguration);

    while(it.hasNext()) {
        it.next();
        Configuration cNextItem = it.value();
        //Keeps original filepath and scanner positions
        c.filepath = cNextItem.filepath;
        c.scannerPosition = cNextItem.scannerPosition;
        it.setValue(c);
    }
}

void LoadFileConfiguration::on_listWidgetFiles_currentRowChanged(int currentRow)
{
    ScannerTypeEnum idx = static_cast<ScannerTypeEnum>(ui->comboBoxScannerType->currentData().toInt());

    bool hasRows = currentRow != -1;
    bool hasRowsAndCustom = hasRows && LVOX3_ScannerUtils::isCustomScannerConfiguration(idx);

    ui->pushButtonDeleteFile->setEnabled(hasRows);
    ui->pushButtonModifyFile->setEnabled(hasRows);

    ui->pushButtonApplyConfigurationToAll->setEnabled(hasRowsAndCustom);
    ui->pushButtonApplyConfigurationToNextFile->setEnabled(hasRowsAndCustom);

    setCurrentScannerType(idx);
}

void LoadFileConfiguration::on_listWidgetFiles_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if((previous != NULL) && !previous->text().isEmpty())
        updateConfiguration(previous);

    editItem(current);
}

void LoadFileConfiguration::on_listWidgetFiles_itemDoubleClicked(QListWidgetItem *item)
{
    if(item->text().isEmpty())
        addFile();
    else
        modifySelectedFile();
}

void LoadFileConfiguration::on_comboBoxReaderType_currentIndexChanged(int index)
{
    ui->comboBoxReaderType->setCurrentIndex(index);

    if(index >= 0 && index < m_availableReaders.size())
        ui->checkBoxFilterPointsOrigin->setEnabled(dynamic_cast<CT_ReaderPointsFilteringExtension*>(m_availableReaders.at(index)));
}

void LoadFileConfiguration::setCurrentScannerType(ScannerTypeEnum scannerId)
{
    bool angles = false, direction = false, position = true;

    ui->doubleSpinBoxXPos->setEnabled(true);
    ui->doubleSpinBoxYPos->setEnabled(true);

    switch(scannerId) {
    case ScannerSphericPointCloud:
        position = true;
        break;
    case ScannerPlanePointCloud:
        direction = true;
        break;
    case ScannerSphericTheoreticScanner:
        position = false;
        angles = true;
        break;
    case ScannerSphericTheoreticCustom:
        angles = true;
        break;
    default:
        break;
    }

    ui->groupBoxPosition->setEnabled(position);
    ui->groupBoxAngles->setEnabled(angles);
    ui->groupBoxDirection->setEnabled(direction);

    //The scanner moves with the plane, changing these two is pointless
    if(scannerId == ScannerPlanePointCloud){
        ui->doubleSpinBoxXPos->setEnabled(false);
        ui->doubleSpinBoxYPos->setEnabled(false);
    }
    ui->comboBoxScannerType->setToolTip(LVOX3_ScannerUtils::getScannerDefinition(scannerId).description);
}

void LoadFileConfiguration::on_comboBoxScannerType_currentIndexChanged(int index)
{
    setCurrentScannerType(static_cast<ScannerTypeEnum>(ui->comboBoxScannerType->itemData(index).toInt()));
}

void LoadFileConfiguration::on_checkBoxRestrictScene_toggled(bool checked)
{
    ui->label_7->setEnabled(checked);
    ui->spinBoxRestrictRadius->setEnabled(checked);
}
