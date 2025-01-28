#include "ct_textfileconfigurationdialog.h"
#include "ui_ct_textfileconfigurationdialog.h"
#include "qfiledialog.h"

#include "qdebug.h"


CT_TextFileConfigurationDialog::CT_TextFileConfigurationDialog(QStringList neededFields,
                                                               QWidget *parent,
                                                               QString fileName,
                                                               bool autoDetect) :
    QDialog(parent),
    ui(new Ui::CT_TextFileConfigurationDialog)
{
    QListIterator<QString> it(neededFields);

    while(it.hasNext())
        _neededFields.append(CT_TextFileConfigurationFields(it.next()));

    initConstructor(fileName, autoDetect);
}

CT_TextFileConfigurationDialog::CT_TextFileConfigurationDialog(QList<CT_TextFileConfigurationFields> neededFields,
                                                               QWidget *parent,
                                                               QString fileName,
                                                               bool autoDetect) :
    QDialog(parent),
    ui(new Ui::CT_TextFileConfigurationDialog)
{    
    _neededFields.append(neededFields);

    initConstructor(fileName, autoDetect);
}

void CT_TextFileConfigurationDialog::init()
{
    if (!_filename.isEmpty())
    {
        // Lecture du fichier (entete)
        delete _file;
        _file = new QFile(_filename);
        if (_file->exists() && _file->open(QIODevice::ReadOnly | QIODevice::Text))
        {
            _stream = new QTextStream(_file);
            _file->close();

            if (_autoDetect) {pb_detect_clicked();}

            ui->filePath->setText(_filename);
            nbLines_valueChanged(ui->nbLines->value());
        } else {
            ui->filePath->setText(tr("Aucun fichier valide choisi."));
            ui->fileExtract->setText("");
        }
    }
}

CT_TextFileConfigurationDialog::~CT_TextFileConfigurationDialog()
{
    delete _stream;
    delete _file;
    delete ui;
}

void CT_TextFileConfigurationDialog::fileChoose_clicked()
{
    QString filter = tr("Fichier ascii") + " (";

    QListIterator<FileFormat> it(_extensions);

    QString tmp;

    while(it.hasNext()) {
        const FileFormat &ff = it.next();

        QStringListIterator itS(ff.suffixes());

        if(itS.hasNext() && !tmp.isEmpty())
            filter += " ";

        while(itS.hasNext())
        {
            tmp += "*." + itS.next();

            if(itS.hasNext())
                tmp += " ";
        }
    }

    if(tmp.isEmpty())
        filter += "*.*";
    else
        filter += tmp;

    filter += ")";

    // choix du fichier
    _filename = QFileDialog::getOpenFileName(this, tr("Choix du fichier ascii"), "", filter);

    // nettoyage
    clearFieldCombos();

    init();
    extractFieldsNames();
}

void CT_TextFileConfigurationDialog::separator_currentIndexChanged(int index)
{
    _separator = ui->separator->itemData(index).toString();

    nbLines_valueChanged(ui->nbLines->value());
    extractFieldsNames();
}

void CT_TextFileConfigurationDialog::nbLines_valueChanged(int arg1)
{
    QString result = "";
    if (_file!=nullptr && _file->exists() && _file->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        _stream->seek(0);

        for (int i = 0 ; i < arg1 ; i++)
        {
            QString line = _stream->readLine();
            if (ui->cb_showCols->isChecked() && _separator != "\t") {line.replace(_separator, QString("%1\t").arg(_separator));}
            line.append("\n");
            result.append(line);
        }
        _file->close();
    }

    ui->fileExtract->setText(result);
}

void CT_TextFileConfigurationDialog::clearFieldCombos()
{
    // nettoyage de la zone des champs
    QMapIterator<QLabel*, QComboBox*> it(_combos);
    while (it.hasNext())
    {
        it.next();
        _layout->removeWidget(it.key());
        _layout->removeWidget(it.value());
        delete it.key();
        delete it.value();
    }
    _combos.clear();
}

void CT_TextFileConfigurationDialog::extractFieldsNames()
{
    _headers.clear();
    _headersNames.clear();

    int headerNNumericValues = 0;
    bool headerIsNumericValues = false;

    if (_file!=nullptr && _file->exists() && _file->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        _stream->seek(0);
        for (int i = 0 ; i < ui->skipLines->value() ; i++)
        {
            _stream->readLine();
        }
        QRegExp regNumeric("\\d+");

        QString headerLine = _stream->readLine();

        QStringList headers = headerLine.split(_separator);
        int size = headers.size();

        for (int i = 0 ; i < size ; i++)
        {
            QString value = headers.at(i);

            if(regNumeric.indexIn(value) != -1)
                ++headerNNumericValues;

            if (hasHeader() && !value.isEmpty())
            {     
                int cpt = 2;
                QString valueBase = value;
                while (_headers.contains(value))
                {
                    value = QString("%1_%2").arg(valueBase, cpt++);
                }
            } else {
                value = QString("COL_%1").arg(i+1);
            }

            _headers.insert(value, i);
            _headersNames.append(value);
        }

        _headers.insert("NODATA", -1);
        _headersNames.append("NODATA");

        _file->close();

        headerIsNumericValues = headerNNumericValues >= (((float)size)/2.0);
    }

    //_headersNames.append("");
    clearFieldCombos();

    int sizeNeededFields = _neededFields.size();
    for (int i = 0 ; i < sizeNeededFields ; i++)
    {
        CT_TextFileConfigurationFields neededField = _neededFields.at(i);

        QLabel *label = new QLabel(QString("%1").arg(neededField.m_nameOfField));
        QComboBox *combo = new QComboBox();
        combo->addItems(_headersNames);

        int matchIndex = -1;
        int bestBeginIndex = 10000;
        int bestMatchedLength = 0;

        for(int j=0; j<_headersNames.size(); ++j)
        {
            int beginIndex = neededField.m_fieldInFileChooser.indexIn(_headersNames.at(j));
            int matchedLength = neededField.m_fieldInFileChooser.matchedLength();

            if(beginIndex >= 0)
            {
                if (matchIndex < 0 || beginIndex < bestBeginIndex || matchedLength > bestMatchedLength)
                {
                    if ((beginIndex <  bestBeginIndex) ||
                       ((beginIndex == bestBeginIndex) && (matchedLength > bestMatchedLength)))
                    {
                        matchIndex = j;
                        bestMatchedLength = neededField.m_fieldInFileChooser.matchedLength();
                        bestBeginIndex = beginIndex;
                    }
                }
            }
        }

        if((matchIndex >= 0) && (matchIndex < _headersNames.count()))
            combo->setCurrentIndex(matchIndex);
        else if((neededField.m_chooseNoDataIfNotMatch || (i >= _headersNames.count())) && !headerIsNumericValues)
            combo->setCurrentIndex(_headersNames.count()-1);
        else if(i >= _headersNames.count())
            combo->setCurrentIndex(_headersNames.count()-1);
        else
            combo->setCurrentIndex(i);

        _layout->addWidget(label, i, 0);
        _layout->addWidget(combo, i, 1);
        _combos.insert(label, combo);
    }
}

void CT_TextFileConfigurationDialog::buttonBox_accepted()
{
    _neededFieldsColumns.clear();
    QMapIterator<QLabel*, QComboBox*> it(_combos);
    while (it.hasNext())
    {
        it.next();
        QString needed = it.key()->text();
        QString header = it.value()->currentText();

        if (_neededFields.contains(CT_TextFileConfigurationFields(needed)) && _headers.contains(header))
        {
            _neededFieldsColumns.insert(needed, _headers.value(header));
        }
    }
}

int CT_TextFileConfigurationDialog::getNlinesToSkip()
{
    return ui->skipLines->value();
}

bool CT_TextFileConfigurationDialog::hasHeader()
{
    return !ui->cb_noheader->isChecked();
}

QString CT_TextFileConfigurationDialog::getQLocaleName()
{
    if (ui->rb_point->isChecked())
    {
        return QLocale(QLocale::English, QLocale::UnitedKingdom).name();
    } else {
        return QLocale(QLocale::French, QLocale::France).name();
    }
}

void CT_TextFileConfigurationDialog::setFileNameWithPath(const QString &path)
{
    _filename = path;
    ui->filePath->setText(_filename);
    init();
    extractFieldsNames();
}

void CT_TextFileConfigurationDialog::setNLinesToSkip(const int &n)
{
    ui->skipLines->setValue(n);
    extractFieldsNames();
}

void CT_TextFileConfigurationDialog::setFieldColumnsSelected(const QMap<QString, int> &map)
{
    extractFieldsNames();

    QMapIterator<QLabel*, QComboBox*> it(_combos);
    while (it.hasNext())
    {
        it.next();

        int headerSelected = map.value(it.key()->text(), 0);

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
        it.value()->setCurrentIndex(it.value()->findText(_headers.key(headerSelected)));
#else
        it.value()->setCurrentText(_headers.key(headerSelected));
#endif
    }
}

QMap<QString, int> CT_TextFileConfigurationDialog::setFieldColumnsSelectedFromString(QString mapAsString)
{
    QMap<QString, int> map;

    QStringList list = mapAsString.split("\n");
    for (int i = 0 ; i < list.size() ; i++)
    {
        QStringList list2 = list.at(i).split("\t");
        if (list2.size() >= 2)
        {
            bool ok;
            int n = list2.at(1).toInt(&ok);
            if (ok)
            {
                map.insert(list2.at(0), n);
            }
        }
    }
    setFieldColumnsSelected(map);

    return map;
}

QString CT_TextFileConfigurationDialog::getFieldColumnsSelectedAsString(const QMap<QString, int> &map)
{
    QString result = "";
    QMapIterator<QString, int> it(map);
    while (it.hasNext())
    {
        it.next();
        result.append(it.key());
        result.append("\t");
        result.append(QString("%1").arg(it.value()));
        if (it.hasNext()) {result.append("\n");}
    }
    return result;
}

void CT_TextFileConfigurationDialog::setFilePathCanBeModified(bool enable)
{
    ui->fileChoose->setEnabled(enable);
}

void CT_TextFileConfigurationDialog::setSeparator(const QString &separator)
{
    _separator = separator;
    int size = ui->separator->count();

    for(int i=0; i<size; ++i)
    {
        if(ui->separator->itemData(i).toString() == separator)
        {
            ui->separator->setCurrentIndex(i);
            return;
        }
    }
}

QStringList CT_TextFileConfigurationDialog::separatorsAvailable()
{
    QStringList l;
    l << "\t";
    l << ",";
    l << ";";
    l << " ";

    return l;
}

void CT_TextFileConfigurationDialog::setFileExtensionAccepted(const QList<FileFormat> &extensions)
{
    _extensions = extensions;
}

void CT_TextFileConfigurationDialog::setHeader(bool header)
{
    ui->cb_noheader->setChecked(!header);
    extractFieldsNames();
}

void CT_TextFileConfigurationDialog::setQLocale(QString locale)
{
    if (QLocale(locale).decimalPoint() == '.')
    {
        ui->rb_point->setChecked(true);
    } else {
        ui->rb_comma->setChecked(true);
    }
}

void CT_TextFileConfigurationDialog::setDecimal(QString decimal)
{
    if (decimal == ".")
    {
        ui->rb_point->setChecked(true);
    } else {
        ui->rb_comma->setChecked(true);
    }
}

void CT_TextFileConfigurationDialog::cb_showCols_clicked()
{
    nbLines_valueChanged(ui->nbLines->value());
}

void CT_TextFileConfigurationDialog::sb_tabSize_valueChanged(int arg1)
{
    ui->fileExtract->setTabStopDistance(arg1);
    nbLines_valueChanged(ui->nbLines->value());
}

void CT_TextFileConfigurationDialog::skipLines_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    extractFieldsNames();
}

void CT_TextFileConfigurationDialog::cb_noheader_clicked()
{
    extractFieldsNames();
}

void CT_TextFileConfigurationDialog::pb_detect_clicked()
{

    // Detect fieds separator automatically
    if (_file!=nullptr && _file->exists() && _file->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        _stream->seek(0);

        for (int i = 0 ; i < getNlinesToSkip() && !_stream->atEnd(); i++) {_stream->readLine();}

        if (hasHeader() && !_stream->atEnd()) {_stream->readLine();}

        if (!_stream->atEnd())
        {
            QString line = _stream->readLine();
            _file->close();

            int nbTab = line.split("\t").size();
            int nbComa = line.split(",").size();
            int nbSemicolon = line.split(";").size();
            int nbSpace = line.split(" ").size();

            if (nbTab >= nbComa && nbTab >= nbSemicolon && nbTab >= nbSpace) {
                setSeparator("\t");
            } else if (nbSemicolon >= nbComa && nbSemicolon >= nbTab && nbSemicolon >= nbSpace) {
                setSeparator(";");
            } else if (nbComa >= nbTab && nbComa >= nbSemicolon && nbComa >= nbSpace) {
                setSeparator(",");
            } else if (nbSpace >= nbComa && nbSpace >= nbTab && nbSpace >= nbSemicolon) {
                setSeparator(" ");
            }
        }
    }
}

void CT_TextFileConfigurationDialog::initConstructor(QString fileName, bool autoDetect)
{
    ui->setupUi(this);

    // Modif michael : problème si on met a nullptr ! si on ne clique pas sur le bouton filechoose
    _stream = nullptr;
    _file = nullptr;
    _autoDetect = autoDetect;

    if (_autoDetect) {ui->pb_detect->setVisible(false);}

    delete ui->fieldWidget->layout();
    _layout = new QGridLayout(ui->fieldWidget);

    // paramétrage de la combo-box des séparateurs
    ui->separator->addItem(tr("Tabulation"), "\t");
    ui->separator->addItem(tr("Virgule"), ",");
    ui->separator->addItem(tr("Point-Virgule"), ";");
    ui->separator->addItem(tr("Espace"), " ");
    _separator = "\t";
    ui->nbLines->setValue(10);
    ui->cb_noheader->setChecked(false);

    _filename = fileName;

    init();
    extractFieldsNames();

    connect(ui->fileChoose, SIGNAL(clicked()), this, SLOT(fileChoose_clicked()));
    connect(ui->separator, SIGNAL(currentIndexChanged(int)), this, SLOT(separator_currentIndexChanged(int)));
    connect(ui->nbLines, SIGNAL(valueChanged(int)), this, SLOT(nbLines_valueChanged(int)));
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(buttonBox_accepted()));
    connect(ui->cb_showCols, SIGNAL(clicked()), this, SLOT(cb_showCols_clicked()));
    connect(ui->sb_tabSize, SIGNAL(valueChanged(int)), this, SLOT(sb_tabSize_valueChanged(int)));
    connect(ui->skipLines, SIGNAL(valueChanged(int)), this, SLOT(skipLines_valueChanged(int)));
    connect(ui->cb_noheader, SIGNAL(clicked()), this, SLOT(cb_noheader_clicked()));
    connect(ui->pb_detect, SIGNAL(clicked()), this, SLOT(pb_detect_clicked()));
}
