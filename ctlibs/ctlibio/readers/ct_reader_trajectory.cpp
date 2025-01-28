#include "ct_reader_trajectory.h"

#include "ct_view/tools/ct_textfileconfigurationdialog.h"
#include "ct_coordinates/tools/ct_coordinatesystemmanager.h"

#include "ct_log/ct_logmanager.h"

#define CHK_ERR(argFunc, argErrStr) if(!error && !argFunc) { error = true; PS_LOG->addErrorMessage(LogInterface::reader, argErrStr); }

#define X_COLUMN "Easting"
#define Y_COLUMN "Northing"
#define Z_COLUMN "Height"
#define GPS_TIME_COLUMN "GPStime"

CT_Reader_Trajectory::CT_Reader_Trajectory(int subMenuLevel) : SuperClass(subMenuLevel)
{
    m_firstConfiguration = true;
    m_columnXIndex = -1;
    m_columnYIndex = -1;
    m_columnZIndex = -1;
    m_columnGPSTimeIndex = -1;
    m_nLinesToSkip = 0;
    m_hasHeader = false;
    m_separator = ".";
    m_localeName = "";

    addNewReadableFormat(FileFormat(QStringList() << "xyz" << "asc" << "txt" << "csv" << "ptx", tr("Fichiers ascii")));

    setToolTip(tr("Chargement d'un fichier de trajectoire au format ASCII.<br>"
                  "L'import est configurable, le fichier devant contenir les champs suivants :<br>"
                  "- X          : Coordonnée X du points<br>"
                  "- Y          : Coordonnée Y du point<br>"
                  "- Z          : Coordonnée Y du point<br><br>"
                  "- GPSTime    : Temps GPS du point<br>"));
}

CT_Reader_Trajectory::CT_Reader_Trajectory(const CT_Reader_Trajectory &other) : SuperClass(other)
{
    m_firstConfiguration = other.m_firstConfiguration;
    m_columnXIndex = other.m_columnXIndex;
    m_columnYIndex = other.m_columnYIndex;
    m_columnZIndex = other.m_columnZIndex;
    m_columnGPSTimeIndex = other.m_columnGPSTimeIndex;
    m_nLinesToSkip = other.m_nLinesToSkip;
    m_hasHeader = other.m_hasHeader;
    m_separator = other.m_separator;
    m_localeName = other.m_localeName;

}

QString CT_Reader_Trajectory::displayableName() const
{
    return tr("Trajectoire, Fichiers ASCII");
}

bool CT_Reader_Trajectory::configure()
{
    // Columns that can be found in the ascii file
    QList<CT_TextFileConfigurationFields> fieldList;
    fieldList.append(CT_TextFileConfigurationFields(X_COLUMN, QRegExp(" *[xX] *| *[eE][aA][sS][tT] *")));            // x
    fieldList.append(CT_TextFileConfigurationFields(Y_COLUMN, QRegExp(" *[yY] *| *[nN][oO][rR][tT][hH] *")));            // y
    fieldList.append(CT_TextFileConfigurationFields(Z_COLUMN, QRegExp(" *[zZ] *| *[hH][eE][iI][gG][hH][tT] *| *[aA][lL][tT][iI] *")));            // z
    fieldList.append(CT_TextFileConfigurationFields(GPS_TIME_COLUMN, QRegExp("[gG][pP][sS] *| *[tT][iI][mM][eE] *")));      // GPS Time


    // a configurable dialog that help the user to select the right column and auto-detect some columns
    CT_TextFileConfigurationDialog dialog(fieldList, nullptr, filepath());
    dialog.setFileExtensionAccepted(readableFormats());
    dialog.setFilePathCanBeModified(filePathCanBeModified());

    if (QFileInfo(filepath()).suffix() == "ptx")
    {
        m_hasHeader = false;
        m_nLinesToSkip = 6;
        m_separator = " ";

        dialog.setHeader(m_hasHeader);
        dialog.setNLinesToSkip(m_nLinesToSkip);
        dialog.setSeparator(m_separator);
    }

    // table that link a sought column to a column in the ascii file
    QMap<QString, int> corresp;

    // if it is not the first configuration
    if(!m_firstConfiguration)
    {
        // we will set by default columns detected in last configuration
        corresp.insert(X_COLUMN, m_columnXIndex);
        corresp.insert(Y_COLUMN, m_columnYIndex);
        corresp.insert(Z_COLUMN, m_columnZIndex);
        corresp.insert(GPS_TIME_COLUMN, m_columnGPSTimeIndex);

        // and other elements like header, number of lines to skip, etc...
        dialog.setHeader(m_hasHeader);
        dialog.setNLinesToSkip(m_nLinesToSkip);
        dialog.setSeparator(m_separator);
        dialog.setQLocale(m_localeName);
        dialog.setFieldColumnsSelected(corresp);
    }              

    if(dialog.exec() != QDialog::Accepted)
        return false;

    // get the link between sought columns and column in the ascii file selected by the user
    corresp = dialog.getNeededFieldColumns();

    if((corresp.value(X_COLUMN, -1) < 0)
            || (corresp.value(Y_COLUMN, -1)  < 0)
            || (corresp.value(Z_COLUMN, -1)  < 0))
        return false;

    m_firstConfiguration = false;

    int columnXIndex = corresp.value(X_COLUMN);
    int columnYIndex = corresp.value(Y_COLUMN);
    int columnZIndex = corresp.value(Z_COLUMN);
    int columnGPSTimeIndex = corresp.value(GPS_TIME_COLUMN, -1);

    m_columnXIndex = columnXIndex;
    m_columnYIndex = columnYIndex;
    m_columnZIndex = columnZIndex;
    m_columnGPSTimeIndex = columnGPSTimeIndex;

    m_nLinesToSkip = dialog.getNlinesToSkip();
    m_hasHeader = dialog.hasHeader();
    m_separator = dialog.getSeparator();
    m_localeName = dialog.getQLocaleName();

    if(!filePathCanBeModified() && !filepath().isEmpty())
        return true;

    return setFilePath(dialog.getFileNameWithPath());
}

void CT_Reader_Trajectory::saveSettings(SettingsWriterInterface &writer) const
{
    SuperClass::saveSettings(writer);

    writer.addParameter(this, "HasHeader", m_hasHeader);
    writer.addParameter(this, "NLinesToSkip", m_nLinesToSkip);
    writer.addParameter(this, "Separator", m_separator);
    writer.addParameter(this, "LocaleName", m_localeName);
    writer.addParameter(this, "ColumnXIndex", m_columnXIndex);
    writer.addParameter(this, "ColumnYIndex", m_columnYIndex);
    writer.addParameter(this, "ColumnZIndex", m_columnZIndex);
    writer.addParameter(this, "ColumnGPSTimeIndex", m_columnGPSTimeIndex);
}

bool CT_Reader_Trajectory::restoreSettings(SettingsReaderInterface &reader)
{
    if(!SuperClass::restoreSettings(reader))
        return false;

    QVariant value;

    if(reader.parameter(this, "HasHeader", value))
        m_hasHeader = value.toBool();

    if(reader.parameter(this, "NLinesToSkip", value))
        m_nLinesToSkip = value.toInt();

    if(reader.parameter(this, "Separator", value))
        m_separator = value.toString();

    if(reader.parameter(this, "LocaleName", value))
        m_localeName = value.toString();

    if(reader.parameter(this, "ColumnXIndex", value))
        m_columnXIndex = value.toInt();

    if(reader.parameter(this, "ColumnYIndex", value))
        m_columnYIndex = value.toInt();

    if(reader.parameter(this, "ColumnZIndex", value))
        m_columnZIndex = value.toInt();

    if(reader.parameter(this, "ColumnGPSTimeIndex", value))
        m_columnGPSTimeIndex = value.toInt();

    return true;
}

void CT_Reader_Trajectory::setXColumnIndex(int index)
{
    m_columnXIndex = index;
}

void CT_Reader_Trajectory::setYColumnIndex(int index)
{
    m_columnYIndex = index;
}

void CT_Reader_Trajectory::setZColumnIndex(int index)
{
    m_columnZIndex = index;
}

void CT_Reader_Trajectory::setGPSTimeColumnIndex(int index)
{
    m_columnGPSTimeIndex = index;
}

void CT_Reader_Trajectory::setFirstConfiguration(bool first)
{
    m_firstConfiguration = first;
}

void CT_Reader_Trajectory::setLinesToSkip(int skip)
{
    m_nLinesToSkip = skip;
}

void CT_Reader_Trajectory::setHasHeader(bool hasHeader)
{
    m_hasHeader = hasHeader;
}

void CT_Reader_Trajectory::setValueSeparator(QString sep)
{
    m_separator = sep;
}

void CT_Reader_Trajectory::setLocaleName(QString locale)
{
    m_localeName = locale;
}

int CT_Reader_Trajectory::xColumnIndex() const
{
    return m_columnXIndex;
}

int CT_Reader_Trajectory::yColumnIndex() const
{
    return m_columnYIndex;
}

int CT_Reader_Trajectory::zColumnIndex() const
{
    return m_columnZIndex;
}

int CT_Reader_Trajectory::gpsTimeColumnIndex() const
{
    return m_columnGPSTimeIndex;
}

bool CT_Reader_Trajectory::canLoadPoints() const
{
    return (m_columnXIndex >= 0) && (m_columnYIndex >= 0) && (m_columnZIndex >= 0) && (m_columnGPSTimeIndex >= 0);
}


void CT_Reader_Trajectory::internalDeclareOutputModels(CT_ReaderOutModelStructureManager& manager)
{
    manager.addItem(m_hOutScanPath, tr("ScanPath"));
}

bool CT_Reader_Trajectory::internalReadFile(CT_StandardItemGroup* group)
{
    if(!canLoadPoints())
        return false;

    if(QFile::exists(filepath()))
    {
        QFile f(filepath());

        if(f.open(QIODevice::ReadOnly))
        {
            QTextStream stream(&f);
            QString     currentLine;
            QStringList wordsOfLine;
            CT_Point point;

            int nLine = 0;
            int maxIndex = maxColumnIndex();

            // Getting the file size to set progress
            qint64 fileSize = f.size();
            qint64 currentSizeRead = 0;

            skipLines(stream, currentSizeRead);

            QLocale locale(m_localeName);
            bool error = false;

            QFileInfo info(filepath());
            CT_ScanPath* scanPath = new CT_ScanPath(info.baseName());
            // While we did not reached the end of file
            while(!stream.atEnd() && !isStopped() && !error) {
                // Read the currentLine
                ++nLine;
                currentLine = stream.readLine();
                currentSizeRead += currentLine.size();
                setProgress(int((float(currentSizeRead)*100.0f) / float(fileSize)));

                if(currentLine.isEmpty())
                    continue;

                // Read each word separately
                wordsOfLine = currentLine.split(m_separator, QT_SKIP_EMPTY_PARTS);

                // Checking for a valid line
                if(maxIndex >= wordsOfLine.size()) {
                    PS_LOG->addErrorMessage(LogInterface::reader, tr("Error loading at line %1: missing columns.").arg(nLine));
                    error = true;
                }

                Eigen::Vector3d pt;
                double gpsTime = 0;

                CHK_ERR(readPoint(wordsOfLine, locale, pt, gpsTime), tr("Error loading point at line %1").arg(nLine));

                scanPath->addPathPoint(gpsTime, pt);
            }

            f.close();

            group->addSingularItem(m_hOutScanPath, scanPath);

            return !error;
        }
    }

    return false;
}

int CT_Reader_Trajectory::maxColumnIndex() const
{
    int index = -1;
    index = qMax(index, m_columnXIndex);
    index = qMax(index, m_columnYIndex);
    index = qMax(index, m_columnZIndex);
    index = qMax(index, m_columnGPSTimeIndex);

    return index;
}


void CT_Reader_Trajectory::skipLines(QTextStream &stream, qint64 &currentSizeRead) const
{
    for(int i = 0 ; i < m_nLinesToSkip; ++i)
        currentSizeRead += stream.readLine().size();

    if (m_hasHeader)
        currentSizeRead += stream.readLine().size();
}

bool CT_Reader_Trajectory::readPoint(const QStringList &wordsOfLine, const QLocale &locale, Eigen::Vector3d &point, double &gpsTime) const
{
    bool ok;

    point(0) = locale.toDouble(wordsOfLine.at(m_columnXIndex), &ok);

    if(!ok)
        return false;

    point(1) = locale.toDouble(wordsOfLine.at(m_columnYIndex), &ok);

    if(!ok)
        return false;

    point(2) = locale.toDouble(wordsOfLine.at(m_columnZIndex), &ok);

    if(!ok)
        return false;

    gpsTime = locale.toDouble(wordsOfLine.at(m_columnGPSTimeIndex), &ok);

    if(!ok)
        return false;


    return true;
}


