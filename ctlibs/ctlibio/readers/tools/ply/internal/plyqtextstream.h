#ifndef PLYQTEXTSTREAM_H
#define PLYQTEXTSTREAM_H

#include <QTextStream>
#include <QList>

/**
 * @brief Use it to read a ascii ply file.
 *
 *        It was necessary to create a special class for read
 *        an ascii ply file because some "operator>>" doesn't
 *        exist in QTextStream.
 */
class PlyQTextStream
{
public:
    enum FloatingPointPrecision {
        SinglePrecision,
        DoublePrecision
    };

    PlyQTextStream(QIODevice *device);
    ~PlyQTextStream();

    QIODevice* device() const;

    PlyQTextStream& operator>>(qint8& v);
    PlyQTextStream& operator>>(quint8& v);
    PlyQTextStream& operator>>(qint16& v);
    PlyQTextStream& operator>>(quint16& v);
    PlyQTextStream& operator>>(qint32& v);
    PlyQTextStream& operator>>(quint32& v);
    PlyQTextStream& operator>>(float& v);
    PlyQTextStream& operator>>(double& v);

    void setFloatingPointPrecision(int p) { Q_UNUSED(p) }

    /**
     * @brief Called by the ply reader to skip an element
     */
    int skipRawData(const qint64& nBytes) { Q_UNUSED(nBytes) return -1; }

private:
    QTextStream*    m_stream;
    QStringList     m_currentValues;

    void checkAndReadNewLine();
    qint64 getInteger();
    quint64 getUInteger();
    double getDouble();
};

#endif // PLYQTEXTSTREAM_H
