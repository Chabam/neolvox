#ifndef LVOX3_READER_ASCIIGRID3D_H
#define LVOX3_READER_ASCIIGRID3D_H

#include "ct_reader/abstract/ct_abstractreader.h"

#include "ctlibio/ctlibio_global.h"
#include "lvox3_reader_asciigrid3d_def_models.h"

#include "tools/3dgrid/lvox3_grid3d.h"

/**
 * @brief (re-implementation of Grid3D for LVOX three resolutions) Reader that can load ascii file that represent Grid 3D (show LVOX3AsciiGrid3D exporter) (*.grid3dlvox *.grd3dlvox)
 */
class LVOX3_Reader_AsciiGrid3D : public CT_AbstractReader
{
    Q_OBJECT

public:

    LVOX3_Reader_AsciiGrid3D(int subMenuLevel = 0);

    LVOX3_Reader_AsciiGrid3D(const LVOX3_Reader_AsciiGrid3D& other);

    /**
     * @brief Returns a displayable name of the reader
     */
    virtual QString displayableName() const override;

    /**
     * @brief Set the filepath to the reader. The file will be opened to search faces, if no face was found
     *        the reader will load a PointCloud otherwise it will load a Mesh.
     */
    bool setFilePath(const QString &filepath) override;

    /**
     * @brief Returns the type used (float by default, modified after call to "setFilePath")
     */
    QMetaType::Type getTypeUsed() const;

    /**
     * @brief Set the type to use when read the grid
     * @return true if the type can be read by this reader, false otherwise
     */
    bool setTypeToUse(QMetaType::Type dataType);

    /**
     * @brief Returns true if the data type specified can be read by this reader
     */
    static bool canReadDataType(const QMetaType::Type& dataType);

    /**
     * @brief Returns all type that can be used
     */
    static QList<QMetaType::Type> getTypeThatCanBeUsed();

    READER_ALL_COPY_IMP(LVOX3_Reader_AsciiGrid3D)

private:
    QMetaType::Type m_dataType;

    CT_HandleOutSingularItem<LVOX3_Grid3D<bool> >         _outGrid_bool;
    CT_HandleOutSingularItem<LVOX3_Grid3D<int> >          _outGrid_int;
    CT_HandleOutSingularItem<LVOX3_Grid3D<uint> >         _outGrid_uint;
    CT_HandleOutSingularItem<LVOX3_Grid3D<qlonglong> >    _outGrid_qlonglong;
    CT_HandleOutSingularItem<LVOX3_Grid3D<qulonglong> >   _outGrid_qulonglong;
    CT_HandleOutSingularItem<LVOX3_Grid3D<double> >       _outGrid_double;
    CT_HandleOutSingularItem<LVOX3_Grid3D<long> >         _outGrid_long;
    CT_HandleOutSingularItem<LVOX3_Grid3D<short> >        _outGrid_short;
    CT_HandleOutSingularItem<LVOX3_Grid3D<char> >         _outGrid_char;
    CT_HandleOutSingularItem<LVOX3_Grid3D<ulong> >        _outGrid_ulong;
    CT_HandleOutSingularItem<LVOX3_Grid3D<ushort> >       _outGrid_ushort;
    CT_HandleOutSingularItem<LVOX3_Grid3D<uchar> >        _outGrid_uchar;
    CT_HandleOutSingularItem<LVOX3_Grid3D<float> >        _outGrid_float;


protected:

    virtual void internalDeclareOutputModels(CT_ReaderOutModelStructureManager& manager) override;

    virtual bool internalReadFile(CT_StandardItemGroup* group) override;

    /**
     * @brief Read the header and returns true if it was readed successfully
     */
    bool readHeader(QTextStream &stream,
                    size_t &nCols,
                    size_t &nRows,
                    size_t &nZLev,
                    Eigen::Vector3d &min,
                    double &cellSizeX,
                    double &cellSizeY,
                    double &cellSizeZ,
                    double &noDataValue,
                    QMetaType::Type& dataType,
                    const QString& filepath,
                    bool showLogMessage);

    /**
     * @brief Load the grid (templated to simplify call with a #DEFINE)
     */
    template<typename TYPE>
    void loadGrid(QTextStream& stream,
                  LVOX3_Grid3D<TYPE>* loadedGrid,
                  const QString& modelName,
                  const size_t& nCols,
                  const size_t& nRows,
                  const size_t& nZLev,
                  const double& noDataValue) {
        Q_UNUSED(modelName)
        double currentValue;
        QString currentValueStr;

        for ( size_t i = 0 ; (i < nZLev) && !isStopped() ; ++i )
        {
            for ( size_t j = 0; (j < nRows) && !isStopped(); ++j )
            {
                currentValueStr = stream.readLine().trimmed();

                if(!currentValueStr.isEmpty()) {
                    const QStringList values = currentValueStr.split(QRegExp("\\s+"));

                    for ( size_t k = 0 ; (k < nCols) && !isStopped() ; ++k )
                    {
                        currentValue = noDataValue;
                        if (static_cast<size_t>(values.size()) > k)
                        {
                            bool ok;
                            currentValue = values.at(k).toDouble(&ok);
                            if (!ok) {currentValue = noDataValue;}
                        }

                        loadedGrid->setValue( k, nRows - j - 1, i, currentValue );
                    }
                }
            }
            stream.readLine();
        }

        loadedGrid->computeMinMax();
    }
};

#endif // LVOX3_READER_ASCIIGRID3D_H
