#include "ct_reader_points_ascii.h"

#include "ct_view/tools/ct_textfileconfigurationdialog.h"
#include "ct_colorcloud/ct_colorcloudstdvector.h"
#include "ct_normalcloud/ct_normalcloudstdvector.h"
#include "ct_point.h"
#include "ct_normal.h"
#include "ct_coordinates/tools/ct_coordinatesystemmanager.h"

#include "ct_log/ct_logmanager.h"

#define CHK_ERR(argFunc, argErrStr) if(!error && !argFunc) { error = true; PS_LOG->addErrorMessage(LogInterface::reader, argErrStr); }

QString intensity_column_str = QObject::tr("Intensité");
QString red_column_str = QObject::tr("Rouge");
QString green_column_str = QObject::tr("Vert");
QString blue_column_str = QObject::tr("Bleu");
QString nx_column_str = QObject::tr("Normale X");
QString ny_column_str = QObject::tr("Normale Y");
QString nz_column_str = QObject::tr("Normale Z");
QString nw_column_str = QObject::tr("Normale Curvature");

#define X_COLUMN "X"
#define Y_COLUMN "Y"
#define Z_COLUMN "Z"
#define INTENSITY_COLUMN intensity_column_str
#define RED_COLUMN red_column_str
#define GREEN_COLUMN green_column_str
#define BLUE_COLUMN blue_column_str
#define NX_COLUMN nx_column_str
#define NY_COLUMN ny_column_str
#define NZ_COLUMN nz_column_str
#define NW_COLUMN nw_column_str


CT_Reader_Points_ASCII::CT_Reader_Points_ASCII(int subMenuLevel) : SuperClass(subMenuLevel), CT_ReaderPointsFilteringExtension()
{
    m_firstConfiguration = true;
    m_columnXIndex = -1;
    m_columnYIndex = -1;
    m_columnZIndex = -1;
    m_columnIntensityIndex = -1;
    m_columnRedIndex = -1;
    m_columnGreenIndex = -1;
    m_columnBlueIndex = -1;
    m_columnNxIndex = -1;
    m_columnNyIndex = -1;
    m_columnNzIndex = -1;
    m_columnNCurvatureIndex = -1;
    m_nLinesToSkip = 0;
    m_hasHeader = false;
    m_separator = ".";
    m_localeName = "";

    addNewReadableFormat(FileFormat(QStringList() << "xyz" << "asc" << "txt" << "csv" << "ptx", tr("Fichiers ascii")));

    setToolTip(tr("Chargement d'un fichier de points au format ASCII.<br>"
                  "L'import est configurable, le fichier devant contenir les champs suivants :<br>"
                  "- X      : Coordonnée X du points<br>"
                  "- Y      : Coordonnée Y du point<br>"
                  "- Z      : Coordonnée Y du point<br><br>"
                  "De plus les champs suivants peuvent être fournis mais sont optionnels :<br>"
                  "- Intensité      : Intensité du point<br>"
                  "- Rouge          : Composante rouge du point<br>"
                  "- Vert           : Composante verte du point<br>"
                  "- Bleu           : Composante bleue du point<br>"
                  "- Normale X      : Coordonnée Y de la normale au point<br>"
                  "- Normale Y      : Coordonnée Y de la normale au point<br>"
                  "- Normale Z      : Coordonnée Y de la normale au point<br>"));

}

CT_Reader_Points_ASCII::CT_Reader_Points_ASCII(const CT_Reader_Points_ASCII &other) : SuperClass(other), CT_ReaderPointsFilteringExtension()
{
    m_firstConfiguration = other.m_firstConfiguration;
    m_columnXIndex = other.m_columnXIndex;
    m_columnYIndex = other.m_columnYIndex;
    m_columnZIndex = other.m_columnZIndex;
    m_columnIntensityIndex = other.m_columnIntensityIndex;
    m_columnRedIndex = other.m_columnRedIndex;
    m_columnGreenIndex = other.m_columnGreenIndex;
    m_columnBlueIndex = other.m_columnBlueIndex;
    m_columnNxIndex = other.m_columnNxIndex;
    m_columnNyIndex = other.m_columnNyIndex;
    m_columnNzIndex = other.m_columnNzIndex;
    m_columnNCurvatureIndex = other.m_columnNCurvatureIndex;
    m_nLinesToSkip = other.m_nLinesToSkip;
    m_hasHeader = other.m_hasHeader;
    m_separator = other.m_separator;
    m_localeName = other.m_localeName;
}

QString CT_Reader_Points_ASCII::displayableName() const
{
    return tr("Nuage de points, Fichiers ASCII (paramétrable)");
}

bool CT_Reader_Points_ASCII::configure()
{
    // Columns that can be found in the ascii file
    QList<CT_TextFileConfigurationFields> fieldList;
    fieldList.append(CT_TextFileConfigurationFields(X_COLUMN, QRegExp(" *[xX] *")));            // x
    fieldList.append(CT_TextFileConfigurationFields(Y_COLUMN, QRegExp(" *[yY] *")));            // y
    fieldList.append(CT_TextFileConfigurationFields(Z_COLUMN, QRegExp(" *[zZ] *")));            // z
    fieldList.append(CT_TextFileConfigurationFields(INTENSITY_COLUMN, QRegExp("[iI].*")));      // intensité - intensity - intensidad - intensität - i
    fieldList.append(CT_TextFileConfigurationFields(RED_COLUMN, QRegExp("[rR].*")));            // rouge - red - rojo - rot - r
    fieldList.append(CT_TextFileConfigurationFields(GREEN_COLUMN, QRegExp("[vVgG].*")));        // vert - green - verde - grün - v - g
    fieldList.append(CT_TextFileConfigurationFields(BLUE_COLUMN, QRegExp("[bBaA].*")));         // bleu - blue - azul - blau - b - a
    fieldList.append(CT_TextFileConfigurationFields(NX_COLUMN, QRegExp("[nN].*[xX]")));         // normale x - normal x - normali x - normalen x - nx
    fieldList.append(CT_TextFileConfigurationFields(NY_COLUMN, QRegExp("[nN].*[yY]")));         // normale y - normal y - normali y - normalen y - ny
    fieldList.append(CT_TextFileConfigurationFields(NZ_COLUMN, QRegExp("[nN].*[zZ]")));         // normale z - normal z - normali z - normalen z - nz
    fieldList.append(CT_TextFileConfigurationFields(NW_COLUMN, QRegExp("[nN].*[wW]")));         // normale w - normal w - normali w - normalen w - nw


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
        corresp.insert(INTENSITY_COLUMN, m_columnIntensityIndex);
        corresp.insert(RED_COLUMN, m_columnRedIndex);
        corresp.insert(GREEN_COLUMN, m_columnGreenIndex);
        corresp.insert(BLUE_COLUMN, m_columnBlueIndex);
        corresp.insert(NX_COLUMN, m_columnNxIndex);
        corresp.insert(NY_COLUMN, m_columnNyIndex);
        corresp.insert(NZ_COLUMN, m_columnNzIndex);
        corresp.insert(NW_COLUMN, m_columnNCurvatureIndex);

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
    int columnIntensityIndex = corresp.value(INTENSITY_COLUMN, -1);
    int columnRedIndex = corresp.value(RED_COLUMN, -1);
    int columnGreenIndex = corresp.value(GREEN_COLUMN, -1);
    int columnBlueIndex = corresp.value(BLUE_COLUMN, -1);
    int columnNxIndex = corresp.value(NX_COLUMN, -1);
    int columnNyIndex = corresp.value(NY_COLUMN, -1);
    int columnNzIndex = corresp.value(NZ_COLUMN, -1);
    int columnNCurvatureIndex = corresp.value(NW_COLUMN, -1);

    m_columnXIndex = columnXIndex;
    m_columnYIndex = columnYIndex;
    m_columnZIndex = columnZIndex;
    m_columnIntensityIndex = columnIntensityIndex;
    m_columnRedIndex = columnRedIndex;
    m_columnGreenIndex = columnGreenIndex;
    m_columnBlueIndex = columnBlueIndex;
    m_columnNxIndex = columnNxIndex;
    m_columnNyIndex = columnNyIndex;
    m_columnNzIndex = columnNzIndex;
    m_columnNCurvatureIndex = columnNCurvatureIndex;

    m_nLinesToSkip = dialog.getNlinesToSkip();
    m_hasHeader = dialog.hasHeader();
    m_separator = dialog.getSeparator();
    m_localeName = dialog.getQLocaleName();

    if(!filePathCanBeModified() && !filepath().isEmpty())
        return true;

    return setFilePath(dialog.getFileNameWithPath());
}

void CT_Reader_Points_ASCII::saveSettings(SettingsWriterInterface &writer) const
{
    SuperClass::saveSettings(writer);

    writer.addParameter(this, "HasHeader", m_hasHeader);
    writer.addParameter(this, "NLinesToSkip", m_nLinesToSkip);
    writer.addParameter(this, "Separator", m_separator);
    writer.addParameter(this, "LocaleName", m_localeName);
    writer.addParameter(this, "ColumnXIndex", m_columnXIndex);
    writer.addParameter(this, "ColumnYIndex", m_columnYIndex);
    writer.addParameter(this, "ColumnZIndex", m_columnZIndex);
    writer.addParameter(this, "ColumnIntensityIndex", m_columnIntensityIndex);
    writer.addParameter(this, "ColumnRedIndex", m_columnRedIndex);
    writer.addParameter(this, "ColumnGreenIndex", m_columnGreenIndex);
    writer.addParameter(this, "ColumnBlueIndex", m_columnBlueIndex);
    writer.addParameter(this, "ColumnNxIndex", m_columnNxIndex);
    writer.addParameter(this, "ColumnNyIndex", m_columnNyIndex);
    writer.addParameter(this, "ColumnNzIndex", m_columnNzIndex);
    writer.addParameter(this, "ColumnNCurvatureIndex", m_columnNCurvatureIndex);
}

bool CT_Reader_Points_ASCII::restoreSettings(SettingsReaderInterface &reader)
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

    if(reader.parameter(this, "ColumnIntensityIndex", value))
        m_columnIntensityIndex = value.toInt();

    if(reader.parameter(this, "ColumnRedIndex", value))
        m_columnRedIndex = value.toInt();

    if(reader.parameter(this, "ColumnGreenIndex", value))
        m_columnGreenIndex = value.toInt();

    if(reader.parameter(this, "ColumnBlueIndex", value))
        m_columnBlueIndex = value.toInt();

    if(reader.parameter(this, "ColumnNxIndex", value))
        m_columnNxIndex = value.toInt();

    if(reader.parameter(this, "ColumnNyIndex", value))
        m_columnNyIndex = value.toInt();

    if(reader.parameter(this, "ColumnNzIndex", value))
        m_columnNzIndex = value.toInt();

    if(reader.parameter(this, "ColumnNCurvatureIndex", value))
        m_columnNCurvatureIndex = value.toInt();

    return true;
}

void CT_Reader_Points_ASCII::setXColumnIndex(int index)
{
    m_columnXIndex = index;
}

void CT_Reader_Points_ASCII::setYColumnIndex(int index)
{
    m_columnYIndex = index;
}

void CT_Reader_Points_ASCII::setZColumnIndex(int index)
{
    m_columnZIndex = index;
}

void CT_Reader_Points_ASCII::setIntensityColumnIndex(int index)
{
    m_columnIntensityIndex = index;
}

void CT_Reader_Points_ASCII::setRedColumnIndex(int index)
{
    m_columnRedIndex = index;
}

void CT_Reader_Points_ASCII::setGreenColumnIndex(int index)
{
    m_columnGreenIndex = index;
}

void CT_Reader_Points_ASCII::setBlueColumnIndex(int index)
{
    m_columnBlueIndex = index;
}

void CT_Reader_Points_ASCII::setNxColumnIndex(int index)
{
    m_columnNxIndex = index;
}

void CT_Reader_Points_ASCII::setNyColumnIndex(int index)
{
    m_columnNyIndex = index;
}

void CT_Reader_Points_ASCII::setNzColumnIndex(int index)
{
    m_columnNzIndex = index;
}

void CT_Reader_Points_ASCII::setNCurvatureIndex(int index)
{
    m_columnNCurvatureIndex = index;
}

void CT_Reader_Points_ASCII::setFirstConfiguration(bool first)
{
    m_firstConfiguration = first;
}

void CT_Reader_Points_ASCII::setLinesToSkip(int skip)
{
    m_nLinesToSkip = skip;
}

void CT_Reader_Points_ASCII::setHasHeader(bool hasHeader)
{
    m_hasHeader = hasHeader;
}

void CT_Reader_Points_ASCII::setValueSeparator(QString sep)
{
    m_separator = sep;
}

void CT_Reader_Points_ASCII::setLocaleName(QString locale)
{
    m_localeName = locale;
}

int CT_Reader_Points_ASCII::xColumnIndex() const
{
    return m_columnXIndex;
}

int CT_Reader_Points_ASCII::yColumnIndex() const
{
    return m_columnYIndex;
}

int CT_Reader_Points_ASCII::zColumnIndex() const
{
    return m_columnZIndex;
}

int CT_Reader_Points_ASCII::intensityColumnIndex() const
{
    return m_columnIntensityIndex;
}

int CT_Reader_Points_ASCII::redColumnIndex() const
{
    return m_columnRedIndex;
}

int CT_Reader_Points_ASCII::greenColumnIndex() const
{
    return m_columnGreenIndex;
}

int CT_Reader_Points_ASCII::blueColumnIndex() const
{
    return m_columnBlueIndex;
}

int CT_Reader_Points_ASCII::nXColumnIndex() const
{
    return m_columnNxIndex;
}

int CT_Reader_Points_ASCII::nYColumnIndex() const
{
    return m_columnNyIndex;
}

int CT_Reader_Points_ASCII::nZColumnIndex() const
{
    return m_columnNzIndex;
}

int CT_Reader_Points_ASCII::nCurvatureIndex() const
{
    return m_columnNCurvatureIndex;
}

bool CT_Reader_Points_ASCII::canLoadPoints() const
{
    return (m_columnXIndex >= 0) && (m_columnYIndex >= 0) && (m_columnZIndex >= 0);
}

bool CT_Reader_Points_ASCII::canLoadIntensity() const
{
    return m_columnIntensityIndex >= 0;
}

bool CT_Reader_Points_ASCII::canLoadColors() const
{
    return (m_columnRedIndex >= 0) && (m_columnGreenIndex >= 0) && (m_columnBlueIndex >= 0);
}

bool CT_Reader_Points_ASCII::canLoadNormals() const
{
    return (m_columnNxIndex >= 0) && (m_columnNyIndex >= 0) && (m_columnNzIndex >= 0);
}

void CT_Reader_Points_ASCII::internalDeclareOutputModels(CT_ReaderOutModelStructureManager& manager)
{
    if(canLoadPoints())
    {
        manager.addItem(m_outPointCloud, tr("Scène(s)"));
    }

    if(canLoadIntensity())
    {
        manager.addItem(m_outIntensity, tr("Intensités"));
    }

    if(canLoadColors())
    {
        manager.addItem(m_outColors, tr("Couleurs"));
    }

    if(canLoadNormals())
    {
        manager.addItem(m_outNormals, tr("Normales"));
    }
}

bool CT_Reader_Points_ASCII::internalReadFile(CT_StandardItemGroup* group)
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

            // create a new point cloud that size was undefined for the moment
            CT_AbstractUndefinedSizePointCloud *uspc = createPointCloud();

            CT_DensePointScalarManager<float>::UPCSSetterPtr intensitySetter = createIntensityArray(uspc);
            CT_DensePointColorManager::UPCSSetterPtr colorSetter = createColorsArray(uspc);
            CT_DensePointNormalManager::UPCSSetterPtr normalSetter = createNormalsArray(uspc);

            Eigen::Vector3d minBB, maxBB;
            initBoundinBox(minBB, maxBB);

            float minIntensity, maxIntensity;
            minIntensity = std::numeric_limits<float>::max();
            maxIntensity = -minIntensity;

            skipLines(stream, currentSizeRead);

            QLocale locale(m_localeName);
            bool error = false;

            // While we did not reached the end of file
            while(!stream.atEnd() && !isStopped() && !error) {
                // Read the currentLine
                ++nLine;
                currentLine = stream.readLine();
                currentSizeRead += currentLine.size();
                setProgress(int((currentSizeRead*100) / fileSize));

                if(currentLine.isEmpty())
                    continue;

                // Read each word separately
                wordsOfLine = currentLine.split(m_separator, QT_SKIP_EMPTY_PARTS);

                // Checking for a valid line
                if(maxIndex >= wordsOfLine.size()) {
                    PS_LOG->addErrorMessage(LogInterface::reader, tr("Error loading at line %1: missing columns.").arg(nLine));
                    error = true;
                }

                CHK_ERR(readPoint(wordsOfLine, locale, point), tr("Error loading point at line %1").arg(nLine));

                if (isPointFiltered(point))
                    continue;

                addPoint(point, uspc, minBB, maxBB);
                CHK_ERR(readAndAddIntensity(wordsOfLine, locale, intensitySetter, minIntensity, maxIntensity), tr("Error loading intensity at line %1").arg(nLine));
                CHK_ERR(readAndAddColor(wordsOfLine, locale, colorSetter), tr("Error loading color at line %1").arg(nLine));
                CHK_ERR(readAndAddNormal(wordsOfLine, locale, normalSetter), tr("Error loading normal at line %1").arg(nLine));

            }

            f.close();

            CT_PCIR pcir = PS_REPOSITORY->registerUndefinedSizePointCloud(uspc);
            CT_Scene* scene = new CT_Scene(pcir);
            group->addSingularItem(m_outPointCloud, scene);

            m_readScene = scene;

            if(intensitySetter != nullptr)
                group->addSingularItem(m_outIntensity, m_outIntensity.createAttributeInstance(pcir, minIntensity, maxIntensity));

            if(colorSetter != nullptr)
                group->addSingularItem(m_outColors, m_outColors.createAttributeInstance(pcir));

            if(normalSetter != nullptr)
                group->addSingularItem(m_outNormals, m_outNormals.createAttributeInstance(pcir));

            return !error;
        }
    }

    return false;
}

CT_AbstractUndefinedSizePointCloud *CT_Reader_Points_ASCII::createPointCloud() const
{
    return PS_REPOSITORY->createNewUndefinedSizePointCloud();
}

CT_DensePointScalarManager<float>::UPCSSetterPtr CT_Reader_Points_ASCII::createIntensityArray(CT_AbstractUndefinedSizePointCloud* uspc)
{
    if(canLoadIntensity())
        return m_outIntensity.createUndefinedPointCloudSizeAttributesSetterPtr(uspc);

    return nullptr;
}

CT_DensePointColorManager::UPCSSetterPtr CT_Reader_Points_ASCII::createColorsArray(CT_AbstractUndefinedSizePointCloud* uspc)
{
    if(canLoadColors())
        return m_outColors.createUndefinedPointCloudSizeAttributesSetterPtr(uspc);

    return nullptr;
}

CT_DensePointNormalManager::UPCSSetterPtr CT_Reader_Points_ASCII::createNormalsArray(CT_AbstractUndefinedSizePointCloud* uspc)
{
    if(canLoadNormals())
        return m_outNormals.createUndefinedPointCloudSizeAttributesSetterPtr(uspc);

    return nullptr;
}

int CT_Reader_Points_ASCII::maxColumnIndex() const
{
    int index = -1;
    index = qMax(index, m_columnXIndex);
    index = qMax(index, m_columnYIndex);
    index = qMax(index, m_columnZIndex);
    index = qMax(index, m_columnIntensityIndex);
    index = qMax(index, m_columnRedIndex);
    index = qMax(index, m_columnGreenIndex);
    index = qMax(index, m_columnBlueIndex);
    index = qMax(index, m_columnNxIndex);
    index = qMax(index, m_columnNyIndex);
    index = qMax(index, m_columnNzIndex);
    index = qMax(index, m_columnNCurvatureIndex);

    return index;
}

void CT_Reader_Points_ASCII::initBoundinBox(Eigen::Vector3d &min, Eigen::Vector3d &max) const
{
    min[0] = std::numeric_limits<double>::max();
    min[1] = min[0];
    min[2] = min[0];

    max[0] = -min[0];
    max[1] = -min[0];
    max[2] = -min[0];
}

void CT_Reader_Points_ASCII::skipLines(QTextStream &stream, qint64 &currentSizeRead) const
{
    for(int i = 0 ; i < m_nLinesToSkip; ++i)
        currentSizeRead += stream.readLine().size();

    if (m_hasHeader)
        currentSizeRead += stream.readLine().size();
}

void CT_Reader_Points_ASCII::updateBoundingBox(const CT_Point &point, Eigen::Vector3d &bboxMin, Eigen::Vector3d &bboxMax) const
{
    bboxMin[0] = qMin(point[CT_Point::X], bboxMin[0]);
    bboxMin[1] = qMin(point[CT_Point::Y], bboxMin[1]);
    bboxMin[2] = qMin(point[CT_Point::Z], bboxMin[2]);

    bboxMax[0] = qMax(point[CT_Point::X], bboxMax[0]);
    bboxMax[1] = qMax(point[CT_Point::Y], bboxMax[1]);
    bboxMax[2] = qMax(point[CT_Point::Z], bboxMax[2]);
}

bool CT_Reader_Points_ASCII::readPoint(const QStringList &wordsOfLine, const QLocale &locale, CT_Point &point) const
{
    bool ok;

    point[CT_Point::X] = locale.toDouble(wordsOfLine.at(m_columnXIndex), &ok);

    if(!ok)
        return false;

    point[CT_Point::Y] = locale.toDouble(wordsOfLine.at(m_columnYIndex), &ok);

    if(!ok)
        return false;

    point[CT_Point::Z] = locale.toDouble(wordsOfLine.at(m_columnZIndex), &ok);

    if(!ok)
        return false;

    return true;
}

void CT_Reader_Points_ASCII::addPoint(const CT_Point &point,
                                      CT_AbstractUndefinedSizePointCloud *array,
                                      Eigen::Vector3d &minBB, Eigen::Vector3d &maxBB) const
{
    array->addPoint(point);
    updateBoundingBox(point, minBB, maxBB);
}

bool CT_Reader_Points_ASCII::readAndAddIntensity(const QStringList &wordsOfLine, const QLocale &locale, CT_DensePointScalarManager<float>::UPCSSetterPtr& setter, float &minI, float &maxI) const
{
    if(setter.get() == nullptr)
        return true;

    bool ok;

    float value = locale.toFloat(wordsOfLine.at(m_columnIntensityIndex), &ok);

    if(!ok)
        return false;

    setter->setLastValue(value);

    minI = qMin(value, minI);
    maxI = qMax(value, maxI);

    return true;
}

bool CT_Reader_Points_ASCII::readAndAddColor(const QStringList &wordsOfLine, const QLocale &locale, CT_DensePointColorManager::UPCSSetterPtr& setter) const
{
    if(setter.get() == nullptr)
        return true;

    bool ok;

    CT_Color color;
    color.r() = uchar(locale.toInt(wordsOfLine.at(m_columnRedIndex), &ok));

    if(!ok)
        return false;

    color.g() = uchar(locale.toInt(wordsOfLine.at(m_columnGreenIndex), &ok));

    if(!ok)
        return false;

    color.b() = uchar(locale.toInt(wordsOfLine.at(m_columnBlueIndex), &ok));

    if(!ok)
        return false;

    setter->setLastValue(color);

    return true;
}

bool CT_Reader_Points_ASCII::readAndAddNormal(const QStringList &wordsOfLine, const QLocale &locale, CT_DensePointNormalManager::UPCSSetterPtr& setter) const
{
    if(setter.get() == nullptr)
        return true;

    bool ok;
    CT_Normal normal;
    normal.x() = locale.toFloat(wordsOfLine.at(m_columnNxIndex), &ok);

    if(!ok)
        return false;

    normal.y() = locale.toFloat(wordsOfLine.at(m_columnNyIndex), &ok);

    if(!ok)
        return false;

    normal.z() = locale.toFloat(wordsOfLine.at(m_columnNzIndex), &ok);

    if(!ok)
        return false;

    if(m_columnNCurvatureIndex >= 0) {
        normal.w() = locale.toFloat(wordsOfLine.at(m_columnNCurvatureIndex), &ok);

        if(!ok)
            return false;
    }

    setter->setLastValue(normal);

    return true;
}
