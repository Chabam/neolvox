#ifndef PLYFILEPARTREADER_H
#define PLYFILEPARTREADER_H

#include "plyheader.h"
#include "readers/tools/ply/internal/plyelementreader.h"
#include "readers/tools/ply/internal/setter/plydirectvaluessetter.h"
#include "readers/tools/ply/internal/setter/plyfillvaluessetter.h"
#include "readers/tools/ply/internal/setter/plydirectscalarvaluessetter.h"
#include "readers/tools/ply/internal/setter/plyfillscalarvaluessetter.h"
#include "readers/tools/ply/internal/setter/plyvaluesrecorder.h"
#include "readers/tools/ply/iplyreaderlistener.h"

#define PLY_FILE_PART_READER_TEMPLATES_DEFINITION template<typename VertexManager, \
                                                           typename ScalarManager, \
                                                           typename ColorManager, \
                                                           typename NormalManager>

#define PlyFilePartReaderT PlyFilePartReader<VertexManager, \
                                             ScalarManager, \
                                             ColorManager, \
                                             NormalManager>

/**
 * @brief Use this class to read some part of a ply file (vertex, colors, faces, etc...)
 *
 * This class will used managers to resize array of vertex, scalars, colors, normals, etc... and set it
 * readed values.
 *
 * Array of attributes of vertex (colors, normals, etc...) will be set to the same size than the vertex array
 */
PLY_FILE_PART_READER_TEMPLATES_DEFINITION
class PlyFilePartReader : public IPlyReaderListener
{
public:
    /**
     * @brief Construct a reader
     * @param header : a valid header of the file (use PlyHeaderReader to get it)
     */
    PlyFilePartReader(const PlyHeader& header);
    ~PlyFilePartReader();

    /**
     * @brief Returns true if the vertex part is set and the header is valid
     */
    bool isValid() const;

    /**
     * @brief Reset this reader
     */
    void clear();

    /**
     * @brief Set which properties to use to read vertexes
     * @param vertexesM : object that will be used to resize the array and fill it with readed values
     * @param elementIndex : index of the element in the header that represent vertex
     * @param xPIndex : index of the property in the element that represent x coordinate
     * @param yPIndex : index of the property in the element that represent y coordinate
     * @param zPIndex : index of the property in the element that represent z coordinate
     */
    bool setVertexPart(VertexManager* vertexesM,
                       const int& elementIndex,
                       const int& xPIndex,
                       const int& yPIndex,
                       const int& zPIndex);

    /**
     * @brief Add a reader for map scalar values with vertexes.
     *
     * Three possibilities :
     *  1/ The number of scalar is the same as the number of vertex => each vertex will have a specific scalar value
     *  2/ The number of scalar is 1 => all vertex will have the same value
     *  3/ The number of scalar is different than the number of vertex => the method will return false
     *
     * @param scalarM : object that will be used to resize the array and fill it with readed values
     * @param elementIndex : index of the element in the header that contains scalars
     * @param scalarPIndex : index of the property in the element that represent the scalar
     * @return false if one condition to read the scalar is not verified. true otherwise.
     * @warning Set first the vertex part !
     */
    bool addVertexScalarPart(ScalarManager* scalarM,
                             const int& elementIndex,
                             const int& scalarPIndex);

    /**
     * @brief Add a reader for map scalar values with vertexes.
     *
     * This method is different because it will use a property in the vertex element that represent
     * the index of scalar to use in another element that contains scalars, example of a ply file for
     * this situation :
     *
     * element vertex 8           { define "vertex" element, 8 of them in file }
     * property float x           { vertex contains float "x" coordinate }
     * property float y           { y coordinate is also a vertex property }
     * property float z           { z coordinate, too }
     * property int scalar_index  { scalar index in "scalar" element for each vertex  }
     * element scalar 2           { define "scalar" element, 2 of them in file }
     * property float value       { scalar contains float values }
     * end_header                 { delimits the end of the header }
     * ....
     *
     * @param scalarM : object that will be used to resize the array and fill it with readed values
     * @param indexOfScalarIndexPropertyInVertexElement : index of the property in the "vertex" element
     *        (set with method "setVertexPart") that represent the scalar index ("scalar_index" in the example)
     * @param elementIndex : index of the element in the header that contains scalars
     * @param scalarPIndex : index of the property in the element that represent the scalar
     * @return false if one condition to read the scalar is not verified. true otherwise.
     * @warning Set first the vertex part !
     */
    bool addVertexScalarPart(ScalarManager* scalarM,
                             const int& indexOfScalarIndexPropertyInVertexElement,
                             const int& elementIndex,
                             const int& scalarPIndex);

    /**
     * @brief Add a reader for map colors with vertexes.
     *
     * Three possibilities :
     *  1/ The number of color is the same as the number of vertex => each vertex will have a specific color
     *  2/ The number of color is 1 => all vertex will have the same color
     *  3/ The number of color is different than the number of vertex => the method will return false
     *
     * @param colorM : object that will be used to resize the array and fill it with readed values
     * @param elementIndex : index of the element in the header that contains colors
     * @param redPIndex : index of the property in the element that represent the red channel
     * @param greenPIndex : index of the property in the element that represent the green channel
     * @param bluePIndex : index of the property in the element that represent the blue channel
     * @param alphaPIndex : index of the property in the element that represent the alpha channel (not used if -1)
     * @return false if one condition to read colors is not verified. true otherwise.
     * @warning Set first the vertex part !
     */
    bool addVertexColorPart(ColorManager* colorM,
                            const int& elementIndex,
                            const int& redPIndex,
                            const int& greenPIndex,
                            const int& bluePIndex,
                            const int& alphaPIndex = -1);

    /**
     * @brief Add a reader for map scalar values with vertexes.
     *
     * This method is different because it will use a property in the vertex element that represent
     * the index of color to use in another element that contains colors, example of a ply file for
     * this situation :
     *
     * element vertex 8           { define "vertex" element, 8 of them in file }
     * property float x           { vertex contains float "x" coordinate }
     * property float y           { y coordinate is also a vertex property }
     * property float z           { z coordinate, too }
     * property int color_index   { color index in "color" element for each vertex  }
     * element color 2            { define "color" element, 2 of them in file }
     * property float r           { color contains float "red" value }
     * property float g           { green value }
     * property float b           { blue value }
     * end_header                 { delimits the end of the header }
     * ....
     *
     * @param colorM : object that will be used to resize the array and fill it with readed values
     * @param indexOfColorIndexPropertyInVertexElement : index of the property in the "vertex" element
     *        (set with method "setVertexPart") that represent the color index ("color_index" in the example)
     * @param elementIndex : index of the element in the header that contains colors
     * @param redPIndex : index of the property in the element that represent the red channel
     * @param greenPIndex : index of the property in the element that represent the green channel
     * @param bluePIndex : index of the property in the element that represent the blue channel
     * @param alphaPIndex : index of the property in the element that represent the alpha channel (not used if -1)
     * @return false if one condition to read colors is not verified. true otherwise.
     * @warning Set first the vertex part !
     */
     bool addVertexColorPartFromList(ColorManager* colorM,
                             const int& indexOfColorIndexPropertyInVertexElement,
                             const int& elementIndex,
                             const int& redPIndex,
                             const int& greenPIndex,
                             const int& bluePIndex,
                             const int& alphaPIndex = -1);

    /**
     * @brief Add a reader for map normals with vertexes.
     *
     * Three possibilities :
     *  1/ The number of normal is the same as the number of vertex => each vertex will have a specific normal
     *  2/ The number of normal is 1 => all vertex will have the same normal
     *  3/ The number of normal is different than the number of vertex => the method will return false
     *
     * @param elementIndex : index of the element in the header that contains normals
     * @param normalM : object that will be used to resize the array and fill it with readed values
     * @param nxPIndex : index of the property in the element that represent the x coordinate
     * @param nyPIndex : index of the property in the element that represent the y coordinate
     * @param nzPIndex : index of the property in the element that represent the z coordinate
     * @param nCurvaturePIndex : index of the property in the element that represent the curvature (not used if -1)
     * @return false if one condition to read normals is not verified. true otherwise.
     * @warning Set first the vertex part !
     */
    bool addVertexNormalPart(NormalManager* normalM,
                             const int& elementIndex,
                             const int& nxPIndex,
                             const int& nyPIndex,
                             const int& nzPIndex,
                             const int& nCurvaturePIndex = -1);

    /**
     * @brief Add a reader for map scalar values with vertexes.
     *
     * This method is different because it will use a property in the vertex element that represent
     * the index of normal to use in another element that contains normals, example of a ply file for
     * this situation :
     *
     * element vertex 8           { define "vertex" element, 8 of them in file }
     * property float x           { vertex contains float "x" coordinate }
     * property float y           { y coordinate is also a vertex property }
     * property float z           { z coordinate, too }
     * property int normal_index  { normal index in "normal" element for each vertex  }
     * element normal 2           { define "normal" element, 2 of them in file }
     * property float nx          { normal contains float "nx" coodinate }
     * property float ny          { "ny" coodinate }
     * property float nz          { "nz" coodinate }
     * end_header                 { delimits the end of the header }
     * ....
     *
     * @param normalM : object that will be used to resize the array and fill it with readed values
     * @param indexOfNormalIndexPropertyInVertexElement : index of the property in the "vertex" element
     *        (set with method "setVertexPart") that represent the normal index ("normal_index" in the example)
     * @param elementIndex : index of the element in the header that contains normals
     * @param redPIndex : index of the property in the element that represent the red channel
     * @param greenPIndex : index of the property in the element that represent the green channel
     * @param bluePIndex : index of the property in the element that represent the blue channel
     * @param alphaPIndex : index of the property in the element that represent the alpha channel (not used if -1)
     * @return false if one condition to read normals is not verified. true otherwise.
     * @warning Set first the vertex part !
     */
     bool addVertexNormalPartFromList(NormalManager* normalM,
                              const int& indexOfNormalIndexPropertyInVertexElement,
                              const int& elementIndex,
                              const int& nxPIndex,
                              const int& nyPIndex,
                              const int& nzPIndex,
                              const int& nCurvaturePIndex = -1);

    /**
     * @brief Set a listener
     */
    void setListener(const IPlyReaderListener* listener);

    /**
     * @brief Process it !
     */
    bool process();

    /**
     * @brief This method must returns true if the read must be stopped
     */
    bool plyReadMustStop() const;

    /**
     * @brief This method is called when the progress changed
     */
    void plyProgressChanged(int progress);

private:
    PlyHeader                       m_header;
    int                             m_maxElementIndex;
    int                             m_vertexElementIndex;
    PlyElementReader*               m_vertexElementReader;
    QList<PlyAbstractValuesSetter*> m_allSetters;
    QHash<int, PlyElementReader*>   m_allReaders;
    IPlyReaderListener*             m_listener;
    int                             m_currentProgress;
    int                             m_progressDivisor;

    /**
     * @brief Create if not exist or get the reader for the specified element
     * @param elementIndex : index of the PlyElement
     * @return the reader
     */
    PlyElementReader* createOrGetElementReader(const int& elementIndex);

    /**
     * @brief Return the element which represent vertex
     */
    inline PlyElement getVertexElement() const {
        if(m_vertexElementIndex < 0)
            return PlyElement();

        return m_header.getElements().at(m_vertexElementIndex);
    }

    /**
     * @brief Read vertex, attributes, etc... with the specified streamer
     */
    template<typename Streamer>
    bool readAll(Streamer& stream);

    /**
     * @brief Save the setter to manage is life in memory
     */
    void registerSetter(PlyAbstractValuesSetter* setter);
};

#include "plyfilepartreader.hpp"

#endif // PLYFILEPARTREADER_H
