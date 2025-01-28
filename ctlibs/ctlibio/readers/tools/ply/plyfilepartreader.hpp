#include "plyfilepartreader.h"
#include "readers/tools/ply/internal/plyqtextstream.h"

#include <QFile>
#include <QDataStream>
#include <QDebug>

PLY_FILE_PART_READER_TEMPLATES_DEFINITION
PlyFilePartReaderT::PlyFilePartReader(const PlyHeader& header)
{
    m_header = header;
    m_vertexElementIndex = -1;
    m_vertexElementReader = nullptr;
    m_maxElementIndex = -1;
    m_listener = nullptr;
}

PLY_FILE_PART_READER_TEMPLATES_DEFINITION
PlyFilePartReaderT::~PlyFilePartReader()
{
    clear();
}

PLY_FILE_PART_READER_TEMPLATES_DEFINITION
bool PlyFilePartReaderT::isValid() const
{
    return m_header.isValid() && (m_vertexElementReader != nullptr);
}

PLY_FILE_PART_READER_TEMPLATES_DEFINITION
void PlyFilePartReaderT::clear()
{
    m_allReaders.remove(m_vertexElementIndex);

    delete m_vertexElementReader;
    m_vertexElementReader = nullptr;

    qDeleteAll(m_allReaders.begin(), m_allReaders.end());
    m_allReaders.clear();

    qDeleteAll(m_allSetters.begin(), m_allSetters.end());
    m_allSetters.clear();
}

PLY_FILE_PART_READER_TEMPLATES_DEFINITION
bool PlyFilePartReaderT::setVertexPart(VertexManager* vertexesM,
                                       const int& elementIndex,
                                       const int& xPIndex,
                                       const int& yPIndex,
                                       const int& zPIndex)
{
    clear();

    m_vertexElementIndex = elementIndex;
    m_vertexElementReader = createOrGetElementReader(elementIndex);

    PlyDirectValuesSetter<VertexManager>* setter = new PlyDirectValuesSetter<VertexManager>(getVertexElement().getNumberOfElements(),
                                                                                                  vertexesM);
    registerSetter(setter);

    if(!m_vertexElementReader->addSetterForProperty(xPIndex, 0, setter))
        return false;

    if(!m_vertexElementReader->addSetterForProperty(yPIndex, 1, setter))
        return false;

    if(!m_vertexElementReader->addSetterForProperty(zPIndex, 2, setter))
        return false;

    return true;
}

PLY_FILE_PART_READER_TEMPLATES_DEFINITION
bool PlyFilePartReaderT::addVertexScalarPart(ScalarManager* scalarM,
                                             const int& elementIndex,
                                             const int& scalarPIndex)
{
    if(m_vertexElementReader == nullptr)
        return false;

    const PlyElement el = m_header.getElements().at(elementIndex);
    const size_t noe = el.getNumberOfElements();

    if(noe == getVertexElement().getNumberOfElements()) {

        PlyElementReader* reader = createOrGetElementReader(elementIndex);

        PlyDirectScalarValuesSetter<ScalarManager>* setter = new PlyDirectScalarValuesSetter<ScalarManager>(noe,
                                                                                                scalarM);
        registerSetter(setter);

        return reader->addSetterForProperty(scalarPIndex, 0, setter);

    } else if(noe == 1) {
        PlyElementReader* reader = createOrGetElementReader(elementIndex);

        PlyFillScalarValuesSetter<ScalarManager>* setter = new PlyFillScalarValuesSetter<ScalarManager>(getVertexElement().getNumberOfElements(),
                                                                                            scalarM);
        registerSetter(setter);

        return reader->addSetterForProperty(scalarPIndex, 0, setter);
    }

    return false;
}

PLY_FILE_PART_READER_TEMPLATES_DEFINITION
bool PlyFilePartReaderT::addVertexScalarPart(ScalarManager* scalarM,
                                             const int& indexOfScalarIndexPropertyInVertexElement,
                                             const int& elementIndex,
                                             const int& scalarPIndex)
{

    Q_ASSERT_X(false, "PlyFilePartReaderT::addVertexScalarPart", "TODO");
    qDebug() << "PlyFilePartReaderT::addVertexScalarPart" << ", " <<  "TODO"; return false;

    if(m_vertexElementReader == nullptr)
        return false;

    // we will record index when we read the vertex element
    PlyIndexValuesRecorder* indexRecorder = new PlyIndexValuesRecorder(getVertexElement().getNumberOfElements());
    registerSetter(indexRecorder);

    if(!m_vertexElementReader->addSetterForProperty(indexOfScalarIndexPropertyInVertexElement, 0, indexRecorder))
        return false;

    // we will record scalars when we read the element that contains it
    PlyScalarValuesRecorder* scalarRecorder = new PlyScalarValuesRecorder(m_header.getElements().at(elementIndex).getNumberOfElements());
    registerSetter(scalarRecorder);

    PlyElementReader* reader = createOrGetElementReader(elementIndex);

    if(!reader->addSetterForProperty(scalarPIndex, 0, scalarRecorder))
        return false;

    // TODO
    // we will set scalar when read is finished
    /*PlyIndexAndValuesMapper* mapper = new PlyIndexAndValuesMapper(indexRecorder,
                                                                  scalarRecorder,
                                                                  scalarM);*/
    Q_UNUSED(scalarM);

    return true;
}

PLY_FILE_PART_READER_TEMPLATES_DEFINITION
bool PlyFilePartReaderT::addVertexColorPart(ColorManager* colorM,
                                            const int& elementIndex,
                                            const int& redPIndex,
                                            const int& greenPIndex,
                                            const int& bluePIndex,
                                            const int& alphaPIndex)
{
    if(m_vertexElementReader == nullptr)
        return false;

    const PlyElement el = m_header.getElements().at(elementIndex);
    const size_t noe = el.getNumberOfElements();

    if(noe == getVertexElement().getNumberOfElements()) {

        PlyElementReader* reader = createOrGetElementReader(elementIndex);

        PlyDirectValuesSetter<ColorManager>* setter = new PlyDirectValuesSetter<ColorManager>(noe,
                                                                                              colorM);
        registerSetter(setter);

        if(!reader->addSetterForProperty(redPIndex, 0, setter))
            return false;

        if(!reader->addSetterForProperty(greenPIndex, 1, setter))
            return false;

        if(!reader->addSetterForProperty(bluePIndex, 2, setter))
            return false;

        if((alphaPIndex != -1) && !reader->addSetterForProperty(alphaPIndex, 3, setter))
            return false;

        return true;

    } else if(noe == 1) {
        PlyElementReader* reader = createOrGetElementReader(elementIndex);

        PlyFillValuesSetter<ColorManager>* setter = new PlyFillValuesSetter<ColorManager>(getVertexElement().getNumberOfElements(),
                                                                                          colorM);
        registerSetter(setter);

        if(!reader->addSetterForProperty(redPIndex, 0, setter))
            return false;

        if(!reader->addSetterForProperty(greenPIndex, 1, setter))
            return false;

        if(!reader->addSetterForProperty(bluePIndex, 2, setter))
            return false;

        if((alphaPIndex != -1) && !reader->addSetterForProperty(alphaPIndex, 3, setter))
            return false;

        return true;
    }

    return false;
}

PLY_FILE_PART_READER_TEMPLATES_DEFINITION
bool PlyFilePartReaderT::addVertexNormalPart(NormalManager* normalM,
                                             const int& elementIndex,
                                             const int& nxPIndex,
                                             const int& nyPIndex,
                                             const int& nzPIndex,
                                             const int& nCurvaturePIndex)
{
    if(m_vertexElementReader == nullptr)
        return false;

    const PlyElement el = m_header.getElements().at(elementIndex);
    const size_t noe = el.getNumberOfElements();

    if(noe == getVertexElement().getNumberOfElements()) {

        PlyElementReader* reader = createOrGetElementReader(elementIndex);

        PlyDirectValuesSetter<NormalManager>* setter = new PlyDirectValuesSetter<NormalManager>(noe,
                                                                                                normalM);
        registerSetter(setter);

        if(!reader->addSetterForProperty(nxPIndex, 0, setter))
            return false;

        if(!reader->addSetterForProperty(nyPIndex, 1, setter))
            return false;

        if(!reader->addSetterForProperty(nzPIndex, 2, setter))
            return false;

        if((nCurvaturePIndex != -1) && !reader->addSetterForProperty(nCurvaturePIndex, 3, setter))
            return false;

        return true;

    } else if(noe == 1) {
        PlyElementReader* reader = createOrGetElementReader(elementIndex);

        PlyFillValuesSetter<NormalManager>* setter = new PlyFillValuesSetter<NormalManager>(getVertexElement().getNumberOfElements(),
                                                                                            normalM);
        registerSetter(setter);

        if(!reader->addSetterForProperty(nxPIndex, 0, setter))
            return false;

        if(!reader->addSetterForProperty(nyPIndex, 1, setter))
            return false;

        if(!reader->addSetterForProperty(nzPIndex, 2, setter))
            return false;

        if((nCurvaturePIndex != -1) && !reader->addSetterForProperty(nCurvaturePIndex, 3, setter))
            return false;

        return true;
    }

    return false;
}

PLY_FILE_PART_READER_TEMPLATES_DEFINITION
void PlyFilePartReaderT::setListener(const IPlyReaderListener *listener)
{
    m_listener = const_cast<IPlyReaderListener*>(listener);
}

PLY_FILE_PART_READER_TEMPLATES_DEFINITION
bool PlyFilePartReaderT::process()
{
    if(!isValid())
        return false;

    QFile file(m_header.getFilePath());

    if(file.open(QFile::ReadOnly)) {

        if(m_header.getFileFormat() == PLY_ascii) {
            PlyQTextStream stream(&file);

            readAll(stream);

        } else {
            QDataStream stream(&file);
            stream.setVersion(QDataStream::Qt_4_6); // enable float precision

            if(m_header.getFileFormat() == PLY_binary_big_endian)
                stream.setByteOrder(QDataStream::BigEndian);
            else if(m_header.getFileFormat() == PLY_binary_little_endian)
                stream.setByteOrder(QDataStream::LittleEndian);
            else {
                file.close();
                return false;
            }

            readAll(stream);
        }

        file.close();

        return true;
    }

    return false;
}

PLY_FILE_PART_READER_TEMPLATES_DEFINITION
bool PlyFilePartReaderT::plyReadMustStop() const
{
    if(m_listener != nullptr)
        return m_listener->plyReadMustStop();

    return false;
}

PLY_FILE_PART_READER_TEMPLATES_DEFINITION
void PlyFilePartReaderT::plyProgressChanged(int progress)
{
    if((m_listener != nullptr) && (m_progressDivisor != 0))
        return m_listener->plyProgressChanged((m_currentProgress + progress) / m_progressDivisor);
}

PLY_FILE_PART_READER_TEMPLATES_DEFINITION
PlyElementReader* PlyFilePartReaderT::createOrGetElementReader(const int& elementIndex)
{
    m_maxElementIndex = qMax(m_maxElementIndex, elementIndex);

    PlyElementReader* reader = m_allReaders.value(elementIndex, nullptr);

    if(reader == nullptr) {
        reader = new PlyElementReader(m_header.getElements().at(elementIndex));
        m_allReaders.insert(elementIndex, reader);
    }

    return reader;
}

PLY_FILE_PART_READER_TEMPLATES_DEFINITION
template<typename Streamer>
bool PlyFilePartReaderT::readAll(Streamer& stream)
{
    // go to the end of the header
    stream.device()->seek(m_header.getEndPositionInFile());

    const int nElements = m_header.getElements().size();
    // only for progress
    m_currentProgress = 0;
    m_progressDivisor = nElements;

    // read each elements
    for(int elementIndex = 0; elementIndex < nElements; ++elementIndex) {

        // read only necessary element
        if(elementIndex <= m_maxElementIndex) {

            PlyElementReader* reader = createOrGetElementReader(elementIndex);
            reader->setListener(this);

            if(!reader->process(stream))
                return false;

            m_currentProgress += 100;
        }
    }

    return true;
}

PLY_FILE_PART_READER_TEMPLATES_DEFINITION
void PlyFilePartReaderT::registerSetter(PlyAbstractValuesSetter* setter)
{
    m_allSetters.append(setter);
}
