#include "lvox3_reader_asciigrid3d.h"

#define LOAD_DATA_HEADER(VVV, LINESTART, DATATYPESTR, CONVERTTO)    ++line; \
                                                                    currentLine = stream.readLine().toLower(); \
                                                                    if(!currentLine.startsWith(LINESTART)) { \
                                                                        if(showLogMessage)\
                                                                            PS_LOG->addErrorMessage(LogInterface::reader, tr("Line %1 must begin with \"%2\" but we read : \"%3\"").arg(line).arg(LINESTART).arg(currentLine) ); \
                                                                        return false; \
                                                                    } \
                                                                    splitline = currentLine.split(QRegExp("\\s+")); \
                                                                    \
                                                                    if(splitline.size() > 1) \
                                                                        VVV = splitline.at(1).CONVERTTO(&read); \
                                                                    \
                                                                    if( !read ) { \
                                                                        if(showLogMessage)\
                                                                            PS_LOG->addErrorMessage(LogInterface::reader, tr("Unable to convert the ascii format to a %1 in header at line %2").arg(DATATYPESTR).arg(line) ); \
                                                                        return false; \
                                                                    }

#define CREATE_GRID_MODEL(METATYPE, REALTYPE)   if(m_dataType == METATYPE) \
                                                    addOutItemDrawableModel(modelName, new LVOX3_Grid3D<REALTYPE>(), displayableName);

#define LOAD_GRID(METATYPE, REALTYPE)if(m_dataType == METATYPE) { \
                                            LVOX3_Grid3D<REALTYPE>* loadedGrid = new LVOX3_Grid3D<REALTYPE>( NULL, \
                                                                                                 NULL, \
                                                                                                 min[0], \
                                                                                                 min[1], \
                                                                                                 min[2], \
                                                                                                 nCols, \
                                                                                                 nRows, \
                                                                                                 nZLev, \
                                                                                                 xcellSize, \
                                                                                                 ycellSize, \
                                                                                                 zcellSize, \
                                                                                                 noDataValue, \
                                                                                                 0.0); \
                                            loadGrid<REALTYPE>(stream, loadedGrid, DEF_LVOX3_Reader_ASCIIGRD3D_grid3DOut, nCols, nRows, nZLev, noDataValue); \
                                        }

LVOX3_Reader_AsciiGrid3D::LVOX3_Reader_AsciiGrid3D()
{
    m_dataType = QMetaType::Float;
}

QString LVOX3_Reader_AsciiGrid3D::GetReaderName() const
{
    return tr("Grille 3D, Fichiers ascii GRD3DLVOX");
}

CT_StepsMenu::LevelPredefined LVOX3_Reader_AsciiGrid3D::getReaderSubMenuName() const
{
    return CT_StepsMenu::LP_Voxels;
}

bool LVOX3_Reader_AsciiGrid3D::setFilePath(const QString &filepath)
{
    // Test File validity
    if(QFile::exists(filepath))
    {
        QFile f(filepath);

        if(f.open(QIODevice::ReadOnly))
        {
            QTextStream stream(&f);

            size_t          nCols;
            size_t          nRows;
            size_t          nZLev;
            Eigen::Vector3d min;
            double          xcellSize;
            double          ycellSize;
            double          zcellSize;
            double          noDataValue;
            QMetaType::Type  dataType;

            if(readHeader(stream, nCols, nRows, nZLev, min, xcellSize,ycellSize,zcellSize, noDataValue, dataType, filepath, true)) {
                f.close();

                if(CT_AbstractReader::setFilePath(filepath)) {
                    m_dataType = dataType;

                    return true;
                }
            }

            f.close();
        }
    }

    return false;
}

QMetaType::Type LVOX3_Reader_AsciiGrid3D::getTypeUsed() const
{
    return m_dataType;
}

bool LVOX3_Reader_AsciiGrid3D::setTypeToUse(QMetaType::Type dataType)
{
    if(!canReadDataType(dataType))
        return false;

    m_dataType = dataType;
    return true;
}

CT_AbstractReader* LVOX3_Reader_AsciiGrid3D::copy() const
{
    return new LVOX3_Reader_AsciiGrid3D();
}

void LVOX3_Reader_AsciiGrid3D::protectedInit()
{
    addNewReadableFormat(FileFormat(QStringList() << "grid3dlvox" << "grd3dlvox" << "GRD3DLVOX", tr("Fichiers grille 3D LVOX")));

    setToolTip(tr("Chargement d'une grille 3D depuis un fichier ASCII ré-implementée pour LVOX résolution x,y,z"));
}

void LVOX3_Reader_AsciiGrid3D::protectedCreateOutItemDrawableModelList()
{
    CT_AbstractReader::protectedCreateOutItemDrawableModelList();

    const QString modelName = DEF_LVOX3_Reader_ASCIIGRD3D_grid3DOut;
    const QString displayableName = tr("Grille 3D");

    CREATE_GRID_MODEL(QMetaType::Bool, bool);
    CREATE_GRID_MODEL(QMetaType::Int, int);
    CREATE_GRID_MODEL(QMetaType::UInt, uint);
    CREATE_GRID_MODEL(QMetaType::LongLong, qlonglong);
    CREATE_GRID_MODEL(QMetaType::ULongLong, qulonglong);
    CREATE_GRID_MODEL(QMetaType::Double, double);
    CREATE_GRID_MODEL(QMetaType::Long, long);
    CREATE_GRID_MODEL(QMetaType::Short, short);
    CREATE_GRID_MODEL(QMetaType::Char, char);
    CREATE_GRID_MODEL(QMetaType::ULong, ulong);
    CREATE_GRID_MODEL(QMetaType::UShort, ushort);
    CREATE_GRID_MODEL(QMetaType::UChar, uchar);
    CREATE_GRID_MODEL(QMetaType::Float, float);
}

bool LVOX3_Reader_AsciiGrid3D::protectedReadFile()
{
    if(QFile::exists(filepath()))
    {
        QFile f(filepath());

        if(f.open(QIODevice::Text | QIODevice::ReadOnly))
        {
            QTextStream stream(&f);

            size_t          nCols;
            size_t          nRows;
            size_t          nZLev;
            Eigen::Vector3d min;
            double          xcellSize;
            double          ycellSize;
            double          zcellSize;
            double          noDataValue;
            QMetaType::Type  dataType;

            if(readHeader(stream, nCols, nRows, nZLev, min, xcellSize, ycellSize, zcellSize, noDataValue, dataType, filepath(), false)) {

                if(dataType != m_dataType) {
                    f.close();
                    return false;
                }

                LOAD_GRID(QMetaType::Bool, bool);
                LOAD_GRID(QMetaType::Int, int);
                LOAD_GRID(QMetaType::UInt, uint);
                LOAD_GRID(QMetaType::LongLong, qlonglong);
                LOAD_GRID(QMetaType::ULongLong, qulonglong);
                LOAD_GRID(QMetaType::Double, double);
                LOAD_GRID(QMetaType::Long, long);
                LOAD_GRID(QMetaType::Short, short);
                LOAD_GRID(QMetaType::Char, char);
                LOAD_GRID(QMetaType::ULong, ulong);
                LOAD_GRID(QMetaType::UShort, ushort);
                LOAD_GRID(QMetaType::UChar, uchar);
                LOAD_GRID(QMetaType::Float, float);

                f.close();

                return true;
            }

            f.close();
        }
    }

    return false;
}

bool LVOX3_Reader_AsciiGrid3D::readHeader(QTextStream &stream,
                                       size_t &nCols,
                                       size_t &nRows,
                                       size_t &nZLev,
                                       Eigen::Vector3d &min,
                                       double &xcellSize,
                                       double &ycellSize,
                                       double &zcellSize,
                                       double &noDataValue,
                                       QMetaType::Type &dataType,
                                       const QString& filepath,
                                       bool showLogMessage)
{
    QString     currentLine;
    QStringList splitline;
    bool        read = false;
    int line = 0;

    if(showLogMessage)
        PS_LOG->addInfoMessage(LogInterface::reader, tr("Read header of file \"%1\"").arg(filepath));

    // Initialise les parametres de la grille
    LOAD_DATA_HEADER(nCols, "ncols", "integer", toULongLong);
    LOAD_DATA_HEADER(nRows, "nrows", "integer", toULongLong);
    LOAD_DATA_HEADER(nZLev, "nzlev", "integer", toULongLong);
    LOAD_DATA_HEADER(min[0], "xllcorner", "real", toDouble);
    LOAD_DATA_HEADER(min[1], "yllcorner", "real", toDouble);
    LOAD_DATA_HEADER(min[2], "zllcorner", "real", toDouble);
    LOAD_DATA_HEADER(xcellSize, "xcellsize", "real", toDouble);
    LOAD_DATA_HEADER(ycellSize, "ycellsize", "real", toDouble);
    LOAD_DATA_HEADER(zcellSize, "zcellsize", "real", toDouble);
    LOAD_DATA_HEADER(noDataValue, "nodata_value", "real", toDouble);

    QString noDataValueString = splitline.at(1);

    qint64 currentPos = stream.pos();

    dataType = QMetaType::Float;
    QMetaType::Type dataTypeReaded = QMetaType::UnknownType;

    read = false;
    ++line;
    currentLine = stream.readLine().toLower();
    splitline = currentLine.split(QRegExp("\\s+"));

    if(currentLine.startsWith("datatype")
            && (splitline.size() > 1)) {
        std::string str = splitline.at(1).toStdString();

        // find type by name of the metatype
        dataTypeReaded = (QMetaType::Type)QMetaType::type(str.data());

        if(canReadDataType(dataTypeReaded)) {
            dataType = dataTypeReaded;

            if(showLogMessage)
                PS_LOG->addInfoMessage(LogInterface::reader, tr("Data type \"%1\" founded in header").arg(QMetaType::typeName(dataTypeReaded)));
        } else if(showLogMessage)
            PS_LOG->addWarningMessage(LogInterface::reader, tr("Unable to read the data type \"%1\" founded at line %2. Create "
                                                               "a grid of \"float\" by default.").arg(QMetaType::typeName(dataTypeReaded))
                                                                                             .arg(line) );

    } else {
        // find type by intuition
        dataType = QMetaType::Int;

        if(noDataValueString.contains(QLocale().decimalPoint()))
            dataType = QMetaType::Float;

        if(showLogMessage) {
            PS_LOG->addWarningMessage(LogInterface::reader, tr("Data type defined as \"%1\" (by intuition). If you want to force the data "
                                                               "type you must insert the string \"datatype XXXX\" at line %2 (line after the "
                                                               "NODATA_value). XXXX must be replaced by \"%3\" for int, \"%4\" for float, \"%5\" for "
                                                               "bool, etc...").arg(QMetaType::typeName(dataType))
                                                                              .arg(line)
                                                                              .arg(QMetaType::typeName(QMetaType::Int))
                                                                              .arg(QMetaType::typeName(QMetaType::Float))
                                                                              .arg(QMetaType::typeName(QMetaType::Bool)) );
        }

        if(!currentLine.startsWith("datatype"))
            stream.seek(currentPos);
    }

    return true;
}

bool LVOX3_Reader_AsciiGrid3D::canReadDataType(const QMetaType::Type &dataType)
{
    return getTypeThatCanBeUsed().contains(dataType);
}

QList<QMetaType::Type> LVOX3_Reader_AsciiGrid3D::getTypeThatCanBeUsed()
{
    QList<QMetaType::Type> l;
    l << QMetaType::Bool;
    l << QMetaType::Int;
    l << QMetaType::UInt;
    l << QMetaType::LongLong;
    l << QMetaType::ULongLong;
    l << QMetaType::Double;
    l << QMetaType::Long;
    l << QMetaType::Short;
    l << QMetaType::Char;
    l << QMetaType::ULong;
    l << QMetaType::UShort;
    l << QMetaType::UChar;
    l << QMetaType::Float;

    return l;
}
