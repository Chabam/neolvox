#include "plyqtextstream.h"

PlyQTextStream::PlyQTextStream(QIODevice *device)
{
    m_stream = new QTextStream(device);
}

PlyQTextStream::~PlyQTextStream()
{
    delete m_stream;
}

QIODevice* PlyQTextStream::device() const
{
    return m_stream->device();
}

PlyQTextStream &PlyQTextStream::operator>>(qint8 &v)
{
    v = qint8(getInteger());

    return *this;
}

PlyQTextStream &PlyQTextStream::operator>>(quint8 &v)
{
    v = quint8(getUInteger());

    return *this;
}

PlyQTextStream &PlyQTextStream::operator>>(qint16 &v)
{
    v = qint16(getInteger());

    return *this;
}

PlyQTextStream &PlyQTextStream::operator>>(quint16 &v)
{
    v = quint16(getUInteger());

    return *this;
}

PlyQTextStream &PlyQTextStream::operator>>(qint32 &v)
{
    v = qint32(getInteger());

    return *this;
}

PlyQTextStream &PlyQTextStream::operator>>(quint32 &v)
{
    v = quint32(getUInteger());

    return *this;
}

PlyQTextStream &PlyQTextStream::operator>>(float &v)
{
    v = float(getDouble());

    return *this;
}

PlyQTextStream &PlyQTextStream::operator>>(double &v)
{
    v = getDouble();

    return *this;
}

void PlyQTextStream::checkAndReadNewLine()
{
    if(m_stream->atEnd())
        return;

    if(m_currentValues.isEmpty()) {
        QString line = m_stream->readLine().simplified();
        m_currentValues = line.split(" ");
    }
}

qint64 PlyQTextStream::getInteger()
{
    checkAndReadNewLine();

    if(m_currentValues.isEmpty())
        return 0;

    return m_currentValues.takeFirst().toInt();
}

quint64 PlyQTextStream::getUInteger()
{
    checkAndReadNewLine();

    if(m_currentValues.isEmpty())
        return 0;

    return m_currentValues.takeFirst().toUInt();
}

double PlyQTextStream::getDouble()
{
    checkAndReadNewLine();

    if(m_currentValues.isEmpty())
        return 0;

    return m_currentValues.takeFirst().toDouble();
}
