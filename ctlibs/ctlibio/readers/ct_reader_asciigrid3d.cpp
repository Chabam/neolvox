#include "ct_reader_asciigrid3d.h"

#include "ct_log/ct_logmanager.h"

QString load_data_header_str_1 = QObject::tr("Line %1 must begin with \"%2\" but we read : \"%3\"");
QString load_data_header_str_2 = QObject::tr("Unable to convert the ascii format to a %1 in header at line %2");

#define LOAD_DATA_HEADER(VVV, LINESTART, DATATYPESTR, CONVERTTO)    ++line; \
                                                                    currentLine = stream.readLine().toLower(); \
                                                                    if(!currentLine.startsWith(LINESTART)) { \
                                                                        if(showLogMessage)\
                                                                            PS_LOG->addErrorMessage(LogInterface::reader, load_data_header_str_1.arg(line).arg(LINESTART).arg(currentLine) ); \
                                                                        return false; \
                                                                    } \
                                                                    splitline = currentLine.split(QRegExp("\\s+")); \
                                                                    \
                                                                    if(splitline.size() > 1) \
                                                                        VVV = splitline.at(1).CONVERTTO(&read); \
                                                                    \
                                                                    if( !read ) { \
                                                                        if(showLogMessage)\
                                                                            PS_LOG->addErrorMessage(LogInterface::reader, load_data_header_str_2.arg(DATATYPESTR).arg(line) ); \
                                                                        return false; \
                                                                    }


CT_Reader_AsciiGrid3D::CT_Reader_AsciiGrid3D(int subMenuLevel) : SuperClass(subMenuLevel)
{
    m_dataType = QMetaType::Float;

    addNewReadableFormat(FileFormat(QStringList() << "grid3d" << "grd3d" << "GRD3D", tr("Fichiers grille 3D")));

    setToolTip(tr("Chargement d'une grille 3D depuis un fichier ASCII (format inspiré du format raster ESRI GRID)."));
}

CT_Reader_AsciiGrid3D::CT_Reader_AsciiGrid3D(const CT_Reader_AsciiGrid3D &other) : SuperClass(other)
{
    m_dataType = other.m_dataType;
}

QString CT_Reader_AsciiGrid3D::displayableName() const
{
    return tr("Grille 3D, Fichiers ascii GRD3D");
}

bool CT_Reader_AsciiGrid3D::setFilePath(const QString &filepath)
{
    // Test File validity
    if(QFile::exists(filepath))
    {
        QFile f(filepath);

        if(f.open(QIODevice::ReadOnly))
        {
            QTextStream stream(&f);

            int          nCols;
            int          nRows;
            int          nZLev;
            Eigen::Vector3d min;
            double          cellSize;
            double          noDataValue;
            QMetaType::Type  dataType;

            if(readHeader(stream, nCols, nRows, nZLev, min, cellSize, noDataValue, dataType, filepath, true)) {
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

QMetaType::Type CT_Reader_AsciiGrid3D::getTypeUsed() const
{
    return m_dataType;
}

bool CT_Reader_AsciiGrid3D::setTypeToUse(QMetaType::Type dataType)
{
    if(!canReadDataType(dataType))
        return false;

    m_dataType = dataType;
    return true;
}


void CT_Reader_AsciiGrid3D::internalDeclareOutputModels(CT_ReaderOutModelStructureManager& manager)
{
    const QString displayableName = tr("Grille 3D");

    if(m_dataType == QMetaType::Bool)       {manager.addItem(_outGrid_bool, displayableName);}
    if(m_dataType == QMetaType::Int)        {manager.addItem(_outGrid_int, displayableName);}
    if(m_dataType == QMetaType::UInt)       {manager.addItem(_outGrid_uint, displayableName);}
    if(m_dataType == QMetaType::LongLong)   {manager.addItem(_outGrid_qlonglong, displayableName);}
    if(m_dataType == QMetaType::ULongLong)  {manager.addItem(_outGrid_qulonglong, displayableName);}
    if(m_dataType == QMetaType::Double)     {manager.addItem(_outGrid_double, displayableName);}
    if(m_dataType == QMetaType::Long)       {manager.addItem(_outGrid_long, displayableName);}
    if(m_dataType == QMetaType::Short)      {manager.addItem(_outGrid_short, displayableName);}
    if(m_dataType == QMetaType::Char)       {manager.addItem(_outGrid_char, displayableName);}
    if(m_dataType == QMetaType::ULong)      {manager.addItem(_outGrid_ulong, displayableName);}
    if(m_dataType == QMetaType::UShort)     {manager.addItem(_outGrid_ushort, displayableName);}
    if(m_dataType == QMetaType::UChar)      {manager.addItem(_outGrid_uchar, displayableName);}
    if(m_dataType == QMetaType::Float)      {manager.addItem(_outGrid_float, displayableName);}

}

bool CT_Reader_AsciiGrid3D::internalReadFile(CT_StandardItemGroup* group)
{
    if(QFile::exists(filepath()))
    {
        QFile f(filepath());

        if(f.open(QIODevice::Text | QIODevice::ReadOnly))
        {
            QTextStream stream(&f);

            int          nCols;
            int          nRows;
            int          nZLev;
            Eigen::Vector3d min;
            double          cellSize;
            double          noDataValue;
            QMetaType::Type  dataType;

            if(readHeader(stream, nCols, nRows, nZLev, min, cellSize, noDataValue, dataType, filepath(), false)) {

                if(dataType != m_dataType) {
                    f.close();
                    return false;
                }


                if(m_dataType == QMetaType::Bool)
                {
                    CT_Grid3D<bool>* loadedGrid = new CT_Grid3D<bool>(min[0], min[1], min[2], nCols, nRows, nZLev, cellSize, bool(noDataValue), 0.0);
                    loadGrid<bool>(stream, loadedGrid, nCols, nRows, nZLev, noDataValue);
                    group->addSingularItem(_outGrid_bool, loadedGrid);

                }
                if(m_dataType == QMetaType::Int)
                {
                    CT_Grid3D<int>* loadedGrid = new CT_Grid3D<int>(min[0], min[1], min[2], nCols, nRows, nZLev, cellSize, int(noDataValue), 0.0);
                    loadGrid<int>(stream, loadedGrid, nCols, nRows, nZLev, noDataValue);
                    group->addSingularItem(_outGrid_int, loadedGrid);

                }
                if(m_dataType == QMetaType::UInt)
                {
                    CT_Grid3D<uint>* loadedGrid = new CT_Grid3D<uint>(min[0], min[1], min[2], nCols, nRows, nZLev, cellSize, uint(noDataValue), 0.0);
                    loadGrid<uint>(stream, loadedGrid, nCols, nRows, nZLev, noDataValue);
                    group->addSingularItem(_outGrid_uint, loadedGrid);

                }
                if(m_dataType == QMetaType::LongLong)
                {
                    CT_Grid3D<qlonglong>* loadedGrid = new CT_Grid3D<qlonglong>(min[0], min[1], min[2], nCols, nRows, nZLev, cellSize, qlonglong(noDataValue), 0.0);
                    loadGrid<qlonglong>(stream, loadedGrid, nCols, nRows, nZLev, noDataValue);
                    group->addSingularItem(_outGrid_qlonglong, loadedGrid);

                }
                if(m_dataType == QMetaType::ULongLong)
                {
                    CT_Grid3D<qulonglong>* loadedGrid = new CT_Grid3D<qulonglong>(min[0], min[1], min[2], nCols, nRows, nZLev, cellSize, qulonglong(noDataValue), 0.0);
                    loadGrid<qulonglong>(stream, loadedGrid, nCols, nRows, nZLev, noDataValue);
                    group->addSingularItem(_outGrid_qulonglong, loadedGrid);

                }
                if(m_dataType == QMetaType::Double)
                {
                    CT_Grid3D<double>* loadedGrid = new CT_Grid3D<double>(min[0], min[1], min[2], nCols, nRows, nZLev, cellSize, double(noDataValue), 0.0);
                    loadGrid<double>(stream, loadedGrid, nCols, nRows, nZLev, noDataValue);
                    group->addSingularItem(_outGrid_double, loadedGrid);

                }
                if(m_dataType == QMetaType::Long)
                {
                    CT_Grid3D<long>* loadedGrid = new CT_Grid3D<long>(min[0], min[1], min[2], nCols, nRows, nZLev, cellSize, long(noDataValue), 0.0);
                    loadGrid<long>(stream, loadedGrid, nCols, nRows, nZLev, noDataValue);
                    group->addSingularItem(_outGrid_long, loadedGrid);

                }
                if(m_dataType == QMetaType::Short)
                {
                    CT_Grid3D<short>* loadedGrid = new CT_Grid3D<short>(min[0], min[1], min[2], nCols, nRows, nZLev, cellSize, short(noDataValue), 0.0);
                    loadGrid<short>(stream, loadedGrid, nCols, nRows, nZLev, noDataValue);
                    group->addSingularItem(_outGrid_short, loadedGrid);

                }
                if(m_dataType == QMetaType::Char)
                {
                    CT_Grid3D<char>* loadedGrid = new CT_Grid3D<char>(min[0], min[1], min[2], nCols, nRows, nZLev, cellSize, char(noDataValue), 0.0);
                    loadGrid<char>(stream, loadedGrid, nCols, nRows, nZLev, noDataValue);
                    group->addSingularItem(_outGrid_char, loadedGrid);

                }
                if(m_dataType == QMetaType::ULong)
                {
                    CT_Grid3D<ulong>* loadedGrid = new CT_Grid3D<ulong>(min[0], min[1], min[2], nCols, nRows, nZLev, cellSize, ulong(noDataValue), 0.0);
                    loadGrid<ulong>(stream, loadedGrid, nCols, nRows, nZLev, noDataValue);
                    group->addSingularItem(_outGrid_ulong, loadedGrid);

                }
                if(m_dataType == QMetaType::UShort)
                {
                    CT_Grid3D<ushort>* loadedGrid = new CT_Grid3D<ushort>(min[0], min[1], min[2], nCols, nRows, nZLev, cellSize, ushort(noDataValue), 0.0);
                    loadGrid<ushort>(stream, loadedGrid, nCols, nRows, nZLev, noDataValue);
                    group->addSingularItem(_outGrid_ushort, loadedGrid);

                }
                if(m_dataType == QMetaType::UChar)
                {
                    CT_Grid3D<uchar>* loadedGrid = new CT_Grid3D<uchar>(min[0], min[1], min[2], nCols, nRows, nZLev, cellSize, uchar(noDataValue), 0.0);
                    loadGrid<uchar>(stream, loadedGrid, nCols, nRows, nZLev, noDataValue);
                    group->addSingularItem(_outGrid_uchar, loadedGrid);

                }
                if(m_dataType == QMetaType::Float)
                {
                    CT_Grid3D<float>* loadedGrid = new CT_Grid3D<float>(min[0], min[1], min[2], nCols, nRows, nZLev, cellSize, float(noDataValue), 0.0);
                    loadGrid<float>(stream, loadedGrid, nCols, nRows, nZLev, noDataValue);
                    group->addSingularItem(_outGrid_float, loadedGrid);

                }

                f.close();

                return true;
            }

            f.close();
        }
    }

    return false;
}

bool CT_Reader_AsciiGrid3D::readHeader(QTextStream &stream,
                                       int &nCols,
                                       int &nRows,
                                       int &nZLev,
                                       Eigen::Vector3d &min,
                                       double &cellSize,
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
    LOAD_DATA_HEADER(nCols, "ncols", "integer", toInt);
    LOAD_DATA_HEADER(nRows, "nrows", "integer", toInt);
    LOAD_DATA_HEADER(nZLev, "nzlev", "integer", toInt);
    LOAD_DATA_HEADER(min[0], "xllcorner", "real", toDouble);
    LOAD_DATA_HEADER(min[1], "yllcorner", "real", toDouble);
    LOAD_DATA_HEADER(min[2], "zllcorner", "real", toDouble);
    LOAD_DATA_HEADER(cellSize, "cellsize", "real", toDouble);
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
        dataTypeReaded = static_cast<QMetaType::Type>(QMetaType::type(str.data()));

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

bool CT_Reader_AsciiGrid3D::canReadDataType(const QMetaType::Type &dataType)
{
    return getTypeThatCanBeUsed().contains(dataType);
}

QList<QMetaType::Type> CT_Reader_AsciiGrid3D::getTypeThatCanBeUsed()
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
