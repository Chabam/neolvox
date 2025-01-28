#include "plyheaderreader.h"

#include <QFile>
#include <QTextStream>

PlyHeaderReader::PlyHeaderReader()
{
}

void PlyHeaderReader::setFilePath(const QString &filepath)
{
    m_filePath = filepath;
}

bool PlyHeaderReader::process()
{
    m_header = PlyHeader();

    QFile file(m_filePath);

    if(file.open(QFile::ReadOnly)) {
        QTextStream stream(&file);

        if(!readMagicWord(stream))
            return false;

        m_header = readFormatAndCreateEmptyHeader(stream, m_filePath);

        QString line;

        int errCode = -3;

        if(m_header.getFileFormat() != PLY_unknown_format) {

            do {
                errCode = readNextObjectAndAddToHeader(stream, m_header, line);
            } while(errCode == 1);
        }

        if(errCode == 0)
            m_header.setEndPositionInFile(stream.pos());

        file.close();

        return m_header.isValid();
    }

    return false;
}

PlyHeader PlyHeaderReader::getHeader() const
{
    return m_header;
}

bool PlyHeaderReader::readMagicWord(QTextStream &stream)
{
    const QString magicWord = "ply";
    bool magicWordFounded = false;
    QString line;

    while(!stream.atEnd()
          && !magicWordFounded) {
        line = stream.readLine().trimmed().toLower();

        if(!line.isEmpty()
                && !(magicWordFounded = (line == magicWord)))
            return false;
    }

    return magicWordFounded;
}

PlyHeader PlyHeaderReader::readFormatAndCreateEmptyHeader(QTextStream &stream,
                                                          const QString &filepath)
{
    PlyHeader header;

    bool ok;
    const QString line = findFileFormat(stream, ok);

    if(!ok)
        return header;

    const QStringList words = line.simplified().split(" ");

    if(words.size() < 2)
        return header;

    const PlyFormatType format = PlyTypeConverter::staticStringToPlyFormatType(words.at(1));

    if(format == PLY_unknown_format)
        return header;

    float version = 1.0;

    if(words.size() > 2) {
        version = words.at(2).toFloat(&ok);

        if(!ok)
            return header;
    }

    return PlyHeader(filepath, format, version);
}

int PlyHeaderReader::readNextObjectAndAddToHeader(QTextStream &stream, PlyHeader &header, QString& lastLineReaded)
{
    QString line;
    bool magicWordFounded = false;

    if(lastLineReaded.isEmpty()
            && !stream.atEnd())
        lastLineReaded = stream.readLine().trimmed().toLower();

    while(!stream.atEnd()
          && !magicWordFounded) {

        if(lastLineReaded.startsWith("comment")) {

            magicWordFounded = true;
            header.addComment(lastLineReaded.right(lastLineReaded.size()-8));
            lastLineReaded = stream.readLine().trimmed().toLower();

        } else if(lastLineReaded.startsWith("element")) {

            magicWordFounded = true;
            bool ok;

            header.addElement(readElement(stream, lastLineReaded, lastLineReaded, ok));

            if(!ok)
                return -2;

        } else if(isEndOfHeader(lastLineReaded)) {
            return 0;
        } else {
            lastLineReaded = stream.readLine().trimmed().toLower();
        }
    }

    if(magicWordFounded)
        return 1;

    return -1;
}

PlyElement PlyHeaderReader::readElement(QTextStream &stream, QString elementLine, QString &lastLineReaded, bool &ok) const
{
    Q_UNUSED(stream)

    PlyElement el("", 0);

    ok = false;

    QStringList words = elementLine.simplified().split(" ");

    const QString elName = words.at(1);

    el = PlyElement(elName, 0);

    if(words.size() < 3)
        return el;

    const int size = words.at(2).toInt(&ok);

    if(!ok)
        return el;

    el = PlyElement(elName, size_t(size));

    while(!stream.atEnd()) {
        lastLineReaded = stream.readLine().trimmed().toLower();

        if(lastLineReaded.startsWith("property")) {

            el.addProperty(readProperty(stream, lastLineReaded, lastLineReaded, ok));

            if(!ok)
                return el;

        } else if(!lastLineReaded.isEmpty()
                  || isEndOfHeader(lastLineReaded))
            return el;
    }

    ok = true;

    return el;
}

PlyProperty PlyHeaderReader::readProperty(QTextStream &stream, QString propertyLine, QString& lastLineReaded, bool &ok) const
{
    Q_UNUSED(stream)
    Q_UNUSED(lastLineReaded)

    const QString notNamed = QObject::tr("Inconnu");

    PlyProperty p(notNamed, PLY_unknown);

    ok = false;

    QStringList words = propertyLine.simplified().split(" ");

    if(words.size() < 2)
        return p;

    const bool isList = words.at(1) == "list";

    PlyType listSizeType = PLY_unknown;
    PlyType valuePropertyType = PLY_unknown;

    if(isList) {
        if(words.size() < 3)
            return p;

        listSizeType = PlyTypeConverter::staticStringToPlyType(words.at(2));

        if(words.size() < 4)
            return PlyProperty(notNamed, listSizeType, PLY_unknown);

        valuePropertyType = PlyTypeConverter::staticStringToPlyType(words.at(3));

        if(words.size() < 5)
            return PlyProperty(notNamed, listSizeType, valuePropertyType);

        p = PlyProperty(words.at(4), listSizeType, valuePropertyType);
        ok = true;

    } else {
        valuePropertyType = PlyTypeConverter::staticStringToPlyType(words.at(1));

        p = PlyProperty(notNamed, valuePropertyType);

        if(words.size() < 3)
            return p;

        p = PlyProperty(words.at(2), valuePropertyType);
        ok = true;
    }

    return p;
}

bool PlyHeaderReader::isEndOfHeader(const QString &line) const
{
    return (line.trimmed() == "end_header");
}

QString PlyHeaderReader::findFileFormat(QTextStream &stream, bool &ok) const
{
    const QString magicWord = "format";
    QString line;
    ok = false;

    while(!stream.atEnd()
          && !ok) {
        line = stream.readLine().trimmed().toLower();

        if(!line.isEmpty()
                && !(ok = (line.startsWith(magicWord))))
            return line;
    }

    return line;
}
