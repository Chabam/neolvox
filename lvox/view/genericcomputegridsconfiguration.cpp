#include "genericcomputegridsconfiguration.h"
#include "ui_genericcomputegridsconfiguration.h"

#include <QDoubleSpinBox>
#include <QMessageBox>

#include "predefinedmapperconfiguration.h"
#include "ct_view/tools/ct_configurablewidgettodialog.h"

#define IN_LETTER_COLUMN 0
#define IN_TYPE_COLUMN 1

#define OUT_LETTER_COLUMN 0
#define OUT_TYPE_COLUMN 1
#define OUT_NAME_COLUMN 2
#define OUT_NAVALUE_COLUMN 3
#define OUT_DEFAULTVALUE_COLUMN 4

#define OUT_CHECK_FORMULA_COLUMN 0
#define OUT_CHECK_ERROR_CODE_COLUMN 1

GenericComputeGridsConfiguration::GenericComputeGridsConfiguration(QWidget *parent) :
    CT_AbstractConfigurableWidget(parent),
    ui(new Ui::GenericComputeGridsConfiguration)
{
    ui->setupUi(this);

    ui->tableWidgetInGrid->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableWidgetOutGrid->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidgetChecks->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->tableWidgetOutGrid->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidgetChecks->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->tableWidgetInGrid->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableWidgetInGrid->verticalHeader()->setDefaultSectionSize(24);

    ui->tableWidgetOutGrid->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableWidgetOutGrid->verticalHeader()->setDefaultSectionSize(24);

    ui->tableWidgetChecks->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableWidgetChecks->verticalHeader()->setDefaultSectionSize(24);

    ui->comboBoxPredefinedCompute->setEnabled(false);
    ui->pushButtonApplyPredefinedCompute->setEnabled(false);

    m_reset = false;

    connect(ui->lineEditComputeFormula, SIGNAL(textChanged(QString)), this, SLOT(saveCurrentConfiguration()), Qt::DirectConnection);

    initTableHeaders();
}

GenericComputeGridsConfiguration::~GenericComputeGridsConfiguration()
{
    delete ui;
}

void GenericComputeGridsConfiguration::setInputGridConfiguration(const QList<lvox::InGridConfiguration> &confs)
{
    m_inputconfiguration = confs;

    resetAndUpdateUI();
}

void GenericComputeGridsConfiguration::setOutputGridConfiguration(const QList<lvox::OutGridConfiguration> &confs)
{
    m_outputconfiguration = confs;

    resetAndUpdateUI();
}

void GenericComputeGridsConfiguration::addOutputGridConfiguration(const QList<lvox::OutGridConfiguration> &confs)
{
    m_outputconfiguration.append(confs);

    resetAndUpdateUI();
}

bool GenericComputeGridsConfiguration::addPredefinedConfiguration(const lvox::PredefinedConfiguration &conf)
{
    if(!conf.checkValidity())
        return false;

    m_predefinedConfiguration.append(conf);

    ui->comboBoxPredefinedCompute->addItem(conf.displayableName);
    ui->comboBoxPredefinedCompute->setEnabled(true);
    ui->pushButtonApplyPredefinedCompute->setEnabled(true);

    return true;
}

QList<lvox::InGridConfiguration> GenericComputeGridsConfiguration::getInputGridConfiguration() const
{
    return m_inputconfiguration;
}

QList<lvox::OutGridConfiguration> GenericComputeGridsConfiguration::getOutputGridConfiguration() const
{
    return m_outputconfiguration;
}

bool GenericComputeGridsConfiguration::canAcceptIt(QString *err) const
{
    if(m_outputconfiguration.isEmpty())
        return false;

    QSet<char> uniqueOutGridLetters;

    foreach (const lvox::OutGridConfiguration& conf, m_outputconfiguration) {
        if((conf.checks.size() == 0) && conf.getFormula().empty()) {
            if(err != NULL)
                *err = tr("No condition or formula defined for output grid %1").arg(conf.getDisplayableString());

            return false;
        }

        foreach (const lvox::CheckConfiguration& check, conf.checks) {
            if(check.getFormula().empty()) {
                if(err != NULL)
                    *err = tr("One output grid's condition %1 contain no formula").arg(conf.getDisplayableString());

                return false;
            }
        }

        if(uniqueOutGridLetters.contains(conf.gridLetter)) {
            if(err != NULL)
                *err = tr("The output grid %1 has the same variable name as another").arg(conf.getDisplayableString());

            return false;
        }

        uniqueOutGridLetters.insert(conf.gridLetter);
    }

    return true;
}

bool GenericComputeGridsConfiguration::isSettingsModified() const
{
    return false;
}

bool GenericComputeGridsConfiguration::updateElement(QString *err)
{
    if(!canAcceptIt(err))
        return false;

    saveCurrentConfiguration();

    return true;
}

void GenericComputeGridsConfiguration::resetAndUpdateUI(int outCurrentRow)
{
    m_reset = true;

    ui->tableWidgetInGrid->clearContents();
    ui->tableWidgetOutGrid->clearContents();
    ui->tableWidgetChecks->clearContents();
    ui->comboBoxOutGridToConfigure->clear();
    ui->lineEditComputeFormula->clear();

    ui->tableWidgetInGrid->setRowCount(m_inputconfiguration.size());

    int row = 0;

    foreach (const lvox::InGridConfiguration& conf, m_inputconfiguration) {
        QTableWidgetItem* item = new QTableWidgetItem(lvox::InGridConfiguration::gridLetterToString(conf.gridLetter));
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->tableWidgetInGrid->setItem(row, IN_LETTER_COLUMN, item);

        item = new QTableWidgetItem(conf.gridDisplayableName);
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->tableWidgetInGrid->setItem(row, IN_TYPE_COLUMN, item);

        ++row;
    }

    ui->tableWidgetOutGrid->setRowCount(m_outputconfiguration.size());

    row = 0;

    foreach (const lvox::OutGridConfiguration& conf, m_outputconfiguration) {

        ui->comboBoxOutGridToConfigure->addItem(lvox::OutGridConfiguration::gridLetterToString(conf.gridLetter));

        QComboBox* cb = createLetterComboBox();
        cb->setCurrentIndex(cb->findData(QString(conf.gridLetter).toLower()));
        ui->tableWidgetOutGrid->setCellWidget(row, OUT_LETTER_COLUMN, cb);

        connect(cb, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxLetterChanged(int)), Qt::QueuedConnection);

        cb = createDataTypeComboBox();
        cb->setCurrentIndex(cb->findData((int)conf.gridDataType));
        ui->tableWidgetOutGrid->setCellWidget(row, OUT_TYPE_COLUMN, cb);

        connect(cb, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxDataTypeChanged(int)), Qt::QueuedConnection);

        QTableWidgetItem* item = new QTableWidgetItem(conf.gridDisplayableName);
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
        ui->tableWidgetOutGrid->setItem(row, OUT_NAME_COLUMN, item);

        QDoubleSpinBox* sp = new QDoubleSpinBox();
        sp->setMinimum(-99999);
        sp->setMaximum(99999);
        sp->setValue(conf.gridNAValue);
        ui->tableWidgetOutGrid->setCellWidget(row, OUT_NAVALUE_COLUMN, sp);

        connect(sp, SIGNAL(valueChanged(double)), this, SLOT(spinBoxNAValueChanged(double)), Qt::QueuedConnection);

        sp = new QDoubleSpinBox();
        sp->setMinimum(-99999);
        sp->setMaximum(99999);
        sp->setValue(conf.gridDefaultValue);
        ui->tableWidgetOutGrid->setCellWidget(row, OUT_DEFAULTVALUE_COLUMN, sp);

        connect(sp, SIGNAL(valueChanged(double)), this, SLOT(spinBoxDefaultValueChanged(double)), Qt::QueuedConnection);

        ++row;
    }

    adjustTableWidgetColumnsToContents();

    m_reset = false;

    if(row > 0) {
        ui->comboBoxOutGridToConfigure->setCurrentIndex((outCurrentRow >= m_outputconfiguration.size()) ? 0 : outCurrentRow);

        restoreConfiguration(outCurrentRow);
    }
}

QComboBox* GenericComputeGridsConfiguration::createLetterComboBox() const
{
    QComboBox* cb = new QComboBox();

    for (char l = 'a'; l < 'z'; ++l){
        cb->addItem(lvox::OutGridConfiguration::gridLetterToString(l), QString(l));
    }

    return cb;
}

QComboBox* GenericComputeGridsConfiguration::createDataTypeComboBox() const
{
    QComboBox* cb = new QComboBox();

    QList<QMetaType::Type> l = CT_Reader_AsciiGrid3D::getTypeThatCanBeUsed();

    foreach (const QMetaType::Type& type, l) {
        cb->addItem(QString(QMetaType::typeName(type)), (int)type);
    }

    return cb;
}

void GenericComputeGridsConfiguration::saveConfiguration(int row)
{
    if(m_reset || (row < 0) || (row >= m_outputconfiguration.size()))
        return;

    lvox::OutGridConfiguration c = m_outputconfiguration.at(row);

    c.gridLetter = qobject_cast<QComboBox*>(ui->tableWidgetOutGrid->cellWidget(row, OUT_LETTER_COLUMN))->currentData().toString().at(0).toLatin1();
    c.gridDataType = (QMetaType::Type)qobject_cast<QComboBox*>(ui->tableWidgetOutGrid->cellWidget(row, OUT_TYPE_COLUMN))->currentData().toInt();
    c.gridDisplayableName = ui->tableWidgetOutGrid->item(row, OUT_NAME_COLUMN)->text();
    c.gridNAValue = qobject_cast<QDoubleSpinBox*>(ui->tableWidgetOutGrid->cellWidget(row, OUT_NAVALUE_COLUMN))->value();
    c.gridDefaultValue = qobject_cast<QDoubleSpinBox*>(ui->tableWidgetOutGrid->cellWidget(row, OUT_DEFAULTVALUE_COLUMN))->value();
    c.setFormula(ui->lineEditComputeFormula->text());

    if(ui->comboBoxOutGridToConfigure->currentIndex() == row) {
        c.checks.clear();

        int n = ui->tableWidgetChecks->rowCount();

        for(int i=0; i<n; ++i) {
            lvox::CheckConfiguration check;

            check.setFormula(ui->tableWidgetChecks->item(i, OUT_CHECK_FORMULA_COLUMN)->text());
            check.setErrorFormula(ui->tableWidgetChecks->item(i, OUT_CHECK_ERROR_CODE_COLUMN)->text());

            c.checks.append(check);
        }
    }

    m_outputconfiguration.replace(row, c);

    updateLabelConfigureOutGrid();
}

void GenericComputeGridsConfiguration::restoreConfiguration(int row)
{
    if(m_reset)
        return;

    m_reset = true;

    if(row >= m_outputconfiguration.size())
        row = 0;

    ui->tableWidgetChecks->clearContents();
    ui->tableWidgetChecks->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidgetChecks->setEnabled(!m_outputconfiguration.isEmpty() && (row >= 0) && (row < m_outputconfiguration.size()));
    ui->pushButtonAddCheck->setEnabled(ui->tableWidgetChecks->isEnabled());
    ui->pushButtonDeleteCheck->setEnabled(ui->tableWidgetChecks->isEnabled());
    ui->lineEditComputeFormula->setEnabled(ui->tableWidgetChecks->isEnabled());

    updateLabelConfigureOutGrid();

    if(!ui->tableWidgetChecks->isEnabled()) {
        m_reset = false;
        return;
    }

    lvox::OutGridConfiguration c = m_outputconfiguration.at(row);

    ui->lineEditComputeFormula->setText(lvox::OutGridConfiguration::formulaToString(c.getFormula()));

    row = 0;

    ui->tableWidgetChecks->setRowCount(c.checks.size());

    foreach (const lvox::CheckConfiguration& check, c.checks) {
        QTableWidgetItem* item = new QTableWidgetItem(lvox::CheckConfiguration::formulaToString(check.getFormula()));
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
        ui->tableWidgetChecks->setItem(row, OUT_CHECK_FORMULA_COLUMN, item);

        item = new QTableWidgetItem(lvox::CheckConfiguration::formulaToString(check.getErrorFormula()));
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
        ui->tableWidgetChecks->setItem(row, OUT_CHECK_ERROR_CODE_COLUMN, item);

        ++row;
    }

    adjustTableWidgetColumnsToContents();

    m_reset = false;
}

void GenericComputeGridsConfiguration::initTableHeaders()
{
    ui->tableWidgetInGrid->setColumnCount(2);
    ui->tableWidgetOutGrid->setColumnCount(5);
    ui->tableWidgetChecks->setColumnCount(2);

    ui->tableWidgetInGrid->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidgetOutGrid->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidgetChecks->horizontalHeader()->setStretchLastSection(true);

    ui->tableWidgetInGrid->verticalHeader()->setVisible(false);
    ui->tableWidgetOutGrid->verticalHeader()->setVisible(false);
    ui->tableWidgetChecks->verticalHeader()->setVisible(false);

    ui->tableWidgetInGrid->setHorizontalHeaderLabels(QStringList() << tr("Variable") << tr("Name and Type"));
    ui->tableWidgetOutGrid->setHorizontalHeaderLabels(QStringList() << tr("Variable") << tr("Type") << tr("Name") << tr("NA") << tr("Default"));
    ui->tableWidgetChecks->setHorizontalHeaderLabels(QStringList() << tr("Si") << tr("Error's formula"));
}

void GenericComputeGridsConfiguration::adjustTableWidgetColumnsToContents()
{
    int n = ui->tableWidgetInGrid->columnCount()-1;

    for(int i=0; i<n; ++i)
        ui->tableWidgetInGrid->resizeColumnToContents(i);

    n = ui->tableWidgetOutGrid->columnCount()-1;

    for(int i=0; i<n; ++i)
        ui->tableWidgetOutGrid->resizeColumnToContents(i);

    n = ui->tableWidgetChecks->columnCount()-1;

    for(int i=0; i<n; ++i)
        ui->tableWidgetChecks->resizeColumnToContents(i);

    ui->tableWidgetInGrid->horizontalHeader()->stretchLastSection();
    ui->tableWidgetOutGrid->horizontalHeader()->stretchLastSection();
    ui->tableWidgetChecks->horizontalHeader()->stretchLastSection();
}

void GenericComputeGridsConfiguration::setOrAddCurrentPredefinedConfiguration(bool add)
{
    int ret = QMessageBox::Yes;

    if(!add)
        QMessageBox::warning(this, tr("Confirm"), tr("Warning! You current configuration will be lost, continue?"), QMessageBox::Yes | QMessageBox::No);

    if(ret == QMessageBox::Yes) {
        lvox::PredefinedConfiguration conf = m_predefinedConfiguration.at(ui->comboBoxPredefinedCompute->currentIndex());

        // check if input was present
        foreach (const lvox::InGridConfiguration& inConf, conf.input) {
            bool ok = false;

            foreach (const lvox::InGridConfiguration& myInConf, m_inputconfiguration) {
                if(myInConf == inConf)
                    ok = true;
            }

            if(!ok) {
                QString letters;

                foreach (const lvox::InGridConfiguration& inConf2, conf.input) {

                    if(!letters.isEmpty())
                        letters += ", ";

                    letters += inConf2.toString();
                }

                QMessageBox::critical(this, tr("Error"), tr("The predefined function can't be %4 because it needs input grid %1 not present."
                                                             "\r\n\r\n Grid(s) "
                                                             "mandatory : \r\n\r\n%3").arg(inConf.toString())
                                                                                       .arg(letters)
                                                                                       .arg(add ? tr("added") : tr("applied")),
                                                                                       QMessageBox::Ok);
                return;
            }
        }

        // all input required is present so we can set this predefined configuration
        // but the user must check and modify the variable name to correspond with is choice

        PredefinedMapperConfiguration mapper;
        mapper.setNecessaryConfiguration(m_inputconfiguration, conf);

        if(CT_ConfigurableWidgetToDialog::exec(&mapper) == QDialog::Accepted) {

            if(add)
                addOutputGridConfiguration(mapper.getPredefinedConfigurationMapped().output);
            else
                setOutputGridConfiguration(mapper.getPredefinedConfigurationMapped().output);
        }
    }
}

void GenericComputeGridsConfiguration::saveCurrentConfiguration()
{
    saveConfiguration(ui->comboBoxOutGridToConfigure->currentIndex());
}

void GenericComputeGridsConfiguration::on_tableWidgetOutGrid_cellChanged(int row, int column)
{
    Q_UNUSED(column)

    if(m_reset)
        return;

    saveConfiguration(row);

    adjustTableWidgetColumnsToContents();
}

void GenericComputeGridsConfiguration::on_tableWidgetChecks_cellChanged(int row, int column)
{
    if(m_reset)
        return;

    if((column == OUT_CHECK_FORMULA_COLUMN) || (column == OUT_CHECK_ERROR_CODE_COLUMN)) {
        int r = ui->comboBoxOutGridToConfigure->currentIndex();

        if(r >= 0 && r < m_outputconfiguration.size()) {
            lvox::OutGridConfiguration conf = m_outputconfiguration.at(r);

            if(row >= 0 && row < conf.checks.size()) {
                lvox::CheckConfiguration cConf = conf.checks.at(row);

                const QString formula = ui->tableWidgetChecks->item(row, column)->text();

                if(column == OUT_CHECK_FORMULA_COLUMN)
                    cConf.setFormula(formula);
                else if(column == OUT_CHECK_ERROR_CODE_COLUMN)
                    cConf.setErrorFormula(formula);

                conf.checks.replace(row, cConf);

                m_outputconfiguration.replace(r, conf);

                adjustTableWidgetColumnsToContents();
            }
        }
    }
}

void GenericComputeGridsConfiguration::on_pushButtonApplyPredefinedCompute_clicked()
{
    setOrAddCurrentPredefinedConfiguration(false);
}

void GenericComputeGridsConfiguration::on_pushButtonAddPredefinedCompute_clicked()
{
    setOrAddCurrentPredefinedConfiguration(true);
}

void GenericComputeGridsConfiguration::on_pushButtonAddOutGrid_clicked()
{
    lvox::OutGridConfiguration conf;
    conf.gridLetter = 'a';
    conf.gridDataType = QMetaType::Float;
    conf.gridDisplayableName = tr("Computed grid");

    if(!m_outputconfiguration.isEmpty()) {
        const lvox::OutGridConfiguration lastConf = m_outputconfiguration.last();

        if(lastConf.gridLetter != 'z')
            conf.gridLetter = lastConf.gridLetter + 1;
    }

    m_outputconfiguration.append(conf);

    resetAndUpdateUI(ui->tableWidgetOutGrid->rowCount());
}

void GenericComputeGridsConfiguration::on_pushButtonDeleteOutGrid_clicked()
{
    int r = ui->tableWidgetOutGrid->currentRow();

    if(r >= 0 && r < m_outputconfiguration.size()) {
        m_outputconfiguration.removeAt(r);
        resetAndUpdateUI(r > 0 ? r-1 : r+1);
    }
}

void GenericComputeGridsConfiguration::on_pushButtonAddCheck_clicked()
{
    int r = ui->comboBoxOutGridToConfigure->currentIndex();

    if(r >= 0 && r < m_outputconfiguration.size()) {
        saveConfiguration(r);

        lvox::CheckConfiguration c;

        lvox::OutGridConfiguration conf = m_outputconfiguration.at(r);
        conf.checks.append(c);

        m_outputconfiguration.replace(r, conf);

        restoreConfiguration(r);
    }
}

void GenericComputeGridsConfiguration::on_pushButtonDeleteCheck_clicked()
{
    int r = ui->comboBoxOutGridToConfigure->currentIndex();

    if(r >= 0 && r < m_outputconfiguration.size()) {

        int rCheck = ui->tableWidgetChecks->currentRow();

        saveConfiguration(r);

        lvox::OutGridConfiguration conf = m_outputconfiguration.at(r);

        if(rCheck >= 0 && rCheck < conf.checks.size()) {
            conf.checks.removeAt(rCheck);

            m_outputconfiguration.replace(r, conf);

            ui->tableWidgetChecks->removeRow(rCheck);
        }
    }
}

void GenericComputeGridsConfiguration::on_comboBoxPredefinedCompute_currentIndexChanged(int index)
{
    if(index >= 0 && index < m_predefinedConfiguration.size()) {
        const lvox::PredefinedConfiguration& conf = m_predefinedConfiguration.at(index);

        QString inGrids;

        foreach(const lvox::InGridConfiguration& in, conf.input) {
            inGrids += "<br></br>" + in.toString().toHtmlEscaped();
        }

        ui->comboBoxPredefinedCompute->setToolTip(tr("<b>%1</b><br></br><br></br><i>%2</i>"
                                                     "<br></br><br></br>Madatory input grid(s):"
                                                     "%3").arg(conf.displayableName.toHtmlEscaped())
                                                      .arg(conf.description.toHtmlEscaped())
                                                      .arg(inGrids));
        return;
    }

    ui->comboBoxPredefinedCompute->setToolTip(tr("No predefined computation available"));
}

void GenericComputeGridsConfiguration::on_comboBoxOutGridToConfigure_currentIndexChanged(int index)
{
    if(m_reset)
        return;

    restoreConfiguration(index);
}

void GenericComputeGridsConfiguration::comboBoxLetterChanged(int index)
{
    Q_UNUSED(index)

    if(m_reset)
        return;

    QComboBox* cb = qobject_cast<QComboBox*>(sender());

    int n = ui->tableWidgetOutGrid->rowCount();

    for(int i=0; i<n; ++i) {
        if(ui->tableWidgetOutGrid->cellWidget(i, OUT_LETTER_COLUMN) == cb) {
            lvox::OutGridConfiguration c = m_outputconfiguration.at(i);
            c.gridLetter = cb->currentData().toString().at(0).toLatin1();

            m_outputconfiguration.replace(i, c);

            ui->comboBoxOutGridToConfigure->setItemText(i, lvox::OutGridConfiguration::gridLetterToString(c.gridLetter));
            return;
        }
    }
}

void GenericComputeGridsConfiguration::comboBoxDataTypeChanged(int index)
{
    Q_UNUSED(index)

    if(m_reset)
        return;

    QComboBox* cb = qobject_cast<QComboBox*>(sender());

    int n = ui->tableWidgetOutGrid->rowCount();

    for(int i=0; i<n; ++i) {
        if(ui->tableWidgetOutGrid->cellWidget(i, OUT_TYPE_COLUMN) == cb) {
            lvox::OutGridConfiguration c = m_outputconfiguration.at(i);
            c.gridDataType = (QMetaType::Type)cb->currentData().toInt();

            m_outputconfiguration.replace(i, c);
            return;
        }
    }
}

void GenericComputeGridsConfiguration::spinBoxNAValueChanged(double value)
{
    if(m_reset)
        return;

    QObject* sp = sender();

    int n = ui->tableWidgetOutGrid->rowCount();

    for(int i=0; i<n; ++i) {
        if(ui->tableWidgetOutGrid->cellWidget(i, OUT_NAVALUE_COLUMN) == sp) {
            lvox::OutGridConfiguration c = m_outputconfiguration.at(i);
            c.gridNAValue = value;

            m_outputconfiguration.replace(i, c);
            return;
        }
    }
}

void GenericComputeGridsConfiguration::spinBoxDefaultValueChanged(double value)
{
    if(m_reset)
        return;

    QObject* sp = sender();

    int n = ui->tableWidgetOutGrid->rowCount();

    for(int i=0; i<n; ++i) {
        if(ui->tableWidgetOutGrid->cellWidget(i, OUT_DEFAULTVALUE_COLUMN) == sp) {
            lvox::OutGridConfiguration c = m_outputconfiguration.at(i);
            c.gridDefaultValue = value;

            m_outputconfiguration.replace(i, c);
            return;
        }
    }
}

void GenericComputeGridsConfiguration::updateLabelConfigureOutGrid()
{
    int row = ui->comboBoxOutGridToConfigure->currentIndex();

    QString text = tr("<b>Output grid configuration</b>");

    if(row < 0 || row >= m_outputconfiguration.size())
        ui->labelConfigureOutGrid->setText(text);
    else
        ui->labelConfigureOutGrid->setText(text + tr("<b>\"%1\"</b>").arg(m_outputconfiguration.at(row).gridDisplayableName.toHtmlEscaped()));
}

