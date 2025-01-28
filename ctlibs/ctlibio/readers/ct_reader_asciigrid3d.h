#ifndef CT_READER_ASCIIGRID3D_H
#define CT_READER_ASCIIGRID3D_H

#include "ct_reader/abstract/ct_abstractreader.h"

#include "ctlibio/ctlibio_global.h"

#include "ct_itemdrawable/ct_grid3d.h"

/**
 * @brief Reader that can load ascii file that represent Grid 3D (show AsciiGrid3D exporter) (*.grid3d *.grd3d)
 */
class CTLIBIO_EXPORT CT_Reader_AsciiGrid3D : public CT_AbstractReader
{
    Q_OBJECT
    typedef CT_AbstractReader SuperClass;

public:
    CT_Reader_AsciiGrid3D(int subMenuLevel = 0);
    CT_Reader_AsciiGrid3D(const CT_Reader_AsciiGrid3D& other);

    /**
     * @brief Returns a displayable name of the reader
     */
    QString displayableName() const override;

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

    READER_ALL_COPY_IMP(CT_Reader_AsciiGrid3D)

private:
    QMetaType::Type m_dataType;

    CT_HandleOutSingularItem<CT_Grid3D<bool> >         _outGrid_bool;
    CT_HandleOutSingularItem<CT_Grid3D<int> >          _outGrid_int;
    CT_HandleOutSingularItem<CT_Grid3D<uint> >         _outGrid_uint;
    CT_HandleOutSingularItem<CT_Grid3D<qlonglong> >    _outGrid_qlonglong;
    CT_HandleOutSingularItem<CT_Grid3D<qulonglong> >   _outGrid_qulonglong;
    CT_HandleOutSingularItem<CT_Grid3D<double> >       _outGrid_double;
    CT_HandleOutSingularItem<CT_Grid3D<long> >         _outGrid_long;
    CT_HandleOutSingularItem<CT_Grid3D<short> >        _outGrid_short;
    CT_HandleOutSingularItem<CT_Grid3D<char> >         _outGrid_char;
    CT_HandleOutSingularItem<CT_Grid3D<ulong> >        _outGrid_ulong;
    CT_HandleOutSingularItem<CT_Grid3D<ushort> >       _outGrid_ushort;
    CT_HandleOutSingularItem<CT_Grid3D<uchar> >        _outGrid_uchar;
    CT_HandleOutSingularItem<CT_Grid3D<float> >        _outGrid_float;


protected:

    void internalDeclareOutputModels(CT_ReaderOutModelStructureManager& manager) override;
    bool internalReadFile(CT_StandardItemGroup* group) override;

    /**
     * @brief Read the header and returns true if it was readed successfully
     */
    bool readHeader(QTextStream &stream,
                    int &nCols,
                    int &nRows,
                    int &nZLev,
                    Eigen::Vector3d &min,
                    double &cellSize,
                    double &noDataValue,
                    QMetaType::Type& dataType,
                    const QString& filepath,
                    bool showLogMessage);

    /**
     * @brief Load the grid (templated to simplify call with a #DEFINE)
     */
    template<typename TYPE>
    void loadGrid(QTextStream& stream,
                  CT_Grid3D<TYPE>* loadedGrid,
                  const int& nCols,
                  const int& nRows,
                  const int& nZLev,
                  const double& noDataValue) {
        double currentValue;
        QString currentValueStr;

        for ( int i = 0 ; (i < nZLev) && !isStopped() ; ++i )
        {
            for ( int j = 0; (j < nRows) && !isStopped(); ++j )
            {
                currentValueStr = stream.readLine().trimmed();

                if(!currentValueStr.isEmpty()) {
                    const QStringList values = currentValueStr.split(QRegExp("\\s+"));

                    for ( int k = 0 ; (k < nCols) && !isStopped() ; ++k )
                    {
                        currentValue = noDataValue;
                        if (values.size() > int(k))
                        {
                            bool ok;
                            currentValue = values.at(int(k)).toDouble(&ok);
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

#endif // CT_READER_ASCIIGRID3D_H
