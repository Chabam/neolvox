#include "predefinedmapperconfiguration.h"
#include "ui_predefinedmapperconfiguration.h"

#define INPUT_COLUMN        0
#define PREDEFINED_COLUMN   1

PredefinedMapperConfiguration::PredefinedMapperConfiguration(QWidget *parent) :
    CT_AbstractConfigurableWidget(parent),
    ui(new Ui::PredefinedMapperConfiguration)
{
    ui->setupUi(this);

    ui->tableWidgetVar->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidgetNameAndType->horizontalHeader()->setStretchLastSection(true);

    ui->tableWidgetVar->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableWidgetVar->verticalHeader()->setDefaultSectionSize(24);

    ui->tableWidgetNameAndType->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableWidgetNameAndType->verticalHeader()->setDefaultSectionSize(24);

    ui->groupBoxMapperVar->setVisible(false);
}

PredefinedMapperConfiguration::~PredefinedMapperConfiguration()
{
    delete ui;
}

bool PredefinedMapperConfiguration::setNecessaryConfiguration(const QList<lvox::InGridConfiguration> &input, lvox::PredefinedConfiguration predefined)
{
    if(!predefined.checkValidity())
        return false;

    m_inputs = input;
    m_predefined = predefined;

    resetAndIniUI();

    return true;
}

lvox::PredefinedConfiguration PredefinedMapperConfiguration::getPredefinedConfigurationMapped() const
{
    return m_predefined;
}

bool PredefinedMapperConfiguration::canAcceptIt(QString *err) const
{
    QSet<QString> uniqueGridsLetter;

    int n = ui->tableWidgetNameAndType->rowCount();

    for(int i=0; i<n; ++i) {
        QComboBox* cb = qobject_cast<QComboBox*>(ui->tableWidgetNameAndType->cellWidget(i, PREDEFINED_COLUMN));
        QString gridLetter = cb->currentData().toString();

        if(!gridLetter.isEmpty()) {
            if(uniqueGridsLetter.contains(gridLetter)) {
                if(err != NULL) {
                    *err = tr("The grid \"%1\" has been chosen twice!").arg(cb->currentText());
                    return false;
                }
            }

            uniqueGridsLetter.insert(gridLetter);
        }
    }

    if(uniqueGridsLetter.size() < m_predefined.input.size())
    {
        if(err != NULL) {
            *err = tr("All required grids aren't filled in");
            return false;
        }
    }

    return true;
}

bool PredefinedMapperConfiguration::isSettingsModified() const
{
    return true;
}

bool PredefinedMapperConfiguration::updateElement(QString *err)
{
    Q_UNUSED(err)

    QMap<char, char> gridsLettersMap;

    if(!constructMapper(gridsLettersMap))
        return false;

    lvox::PredefinedConfiguration pre = m_predefined;

    int i = 0;

    foreach(lvox::OutGridConfiguration out, pre.output) {

        int j = 0;

        foreach(lvox::CheckConfiguration check, out.checks) {
            QString formula = check.getFormulaInQStringFormat();

            mapFormula(formula, gridsLettersMap);

            check.setFormula(formula);

            out.checks.replace(j++, check);
        }

        QString formula = out.getFormulaInQStringFormat();

        mapFormula(formula, gridsLettersMap);

        out.setFormula(formula);

        pre.output.replace(i++, out);
    }

    m_predefined = pre;

    return true;
}

void PredefinedMapperConfiguration::resetAndIniUI()
{
    ui->tableWidgetVar->clearContents();
    ui->tableWidgetNameAndType->clearContents();

    int nRow = m_inputs.size();

    ui->tableWidgetVar->setRowCount(nRow);
    ui->tableWidgetNameAndType->setRowCount(nRow);

    int row = 0;

    foreach(const lvox::InGridConfiguration& in, m_inputs) {
        QTableWidgetItem* item = new QTableWidgetItem(lvox::InGridConfiguration::gridLetterToString(in.gridLetter));
        item->setData(Qt::UserRole+1, QString(in.gridLetter));
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->tableWidgetVar->setItem(row, INPUT_COLUMN, item);

        QComboBox* cbv = createNewVarComboBox();
        ui->tableWidgetVar->setCellWidget(row, PREDEFINED_COLUMN, cbv);

        item = new QTableWidgetItem(in.gridDisplayableName);
        item->setData(Qt::UserRole+1, QString(in.gridLetter));
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->tableWidgetNameAndType->setItem(row, INPUT_COLUMN, item);

        QComboBox* cbn = createNewNameAndTypeComboBox();

        ui->tableWidgetNameAndType->setCellWidget(row, PREDEFINED_COLUMN, cbn);

        connect(cbv, SIGNAL(currentIndexChanged(int)), cbn, SLOT(setCurrentIndex(int)));
        connect(cbn, SIGNAL(currentIndexChanged(int)), cbv, SLOT(setCurrentIndex(int)));

        int findIndex = cbn->findText(in.gridDisplayableName);

        if(findIndex != -1)
            cbn->setCurrentIndex(findIndex);
        else
            cbn->setCurrentIndex(cbn->count()-1);

        ++row;
    }

    ui->tableWidgetVar->resizeColumnToContents(0);
    ui->tableWidgetNameAndType->resizeColumnToContents(0);
}

QComboBox* PredefinedMapperConfiguration::createNewVarComboBox() const
{
    QComboBox* cb = new QComboBox();

    foreach (const lvox::InGridConfiguration& in, m_predefined.input) {
        cb->addItem(lvox::InGridConfiguration::gridLetterToString(in.gridLetter), QString(in.gridLetter));
    }

    if(m_inputs.size() > m_predefined.input.size())
        cb->addItem("");

    return cb;
}

QComboBox* PredefinedMapperConfiguration::createNewNameAndTypeComboBox() const
{
    QComboBox* cb = new QComboBox();

    foreach (const lvox::InGridConfiguration& in, m_predefined.input) {
        cb->addItem(in.gridDisplayableName, QString(in.gridLetter));
    }

    if(m_inputs.size() > m_predefined.input.size())
        cb->addItem("", "");

    return cb;
}

bool PredefinedMapperConfiguration::constructMapper(QMap<char, char>& gridsLettersMap) const
{
    int n = ui->tableWidgetNameAndType->rowCount();

    for(int i=0; i<n; ++i) {
        QComboBox* cb = qobject_cast<QComboBox*>(ui->tableWidgetNameAndType->cellWidget(i, PREDEFINED_COLUMN));
        QString gridLetter = cb->currentData().toString();

        if(!gridLetter.isEmpty()) {
            char c = gridLetter.at(0).toLatin1();

            if(gridsLettersMap.contains(c))
                return false;

            gridsLettersMap.insert(c, ui->tableWidgetNameAndType->item(i, INPUT_COLUMN)->data(Qt::UserRole+1).toString().at(0).toLatin1());
        }
    }

    return true;
}

void PredefinedMapperConfiguration::mapFormula(QString &formula, const QMap<char, char>& gridsLettersMap)
{
    QMapIterator<char, char> it(gridsLettersMap);
    int i = 0;

    while(it.hasNext()) {
        it.next();
        char gridLetterToReplace = it.key();

        formula.replace(QString(gridLetterToReplace), QString("TMP%1").arg(i++));
    }

    it.toFront();
    i = 0;

    while(it.hasNext()) {
        it.next();
        char newGridLetter = it.value();

        formula.replace(QString("TMP%1").arg(i++), QString(newGridLetter));
    }
}
