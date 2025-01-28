#include "plyheader.h"

PlyHeader::PlyHeader() : PlyHeader("", PLY_unknown_format, 0.0)
{
}

PlyHeader::PlyHeader(const QString &filepath, const PlyFormatType &format, const float &formatVersion)
{
    m_filepath = filepath;
    m_formatType = format;
    m_formatVersion = formatVersion;
    m_endPos = -1;
}

bool PlyHeader::isValid() const
{
    if((m_formatType == PLY_unknown_format)
            || (m_formatVersion <= 0.0f)
            || (m_endPos <= 0)
            || m_filepath.isEmpty())
        return false;

    foreach(const PlyElement& el, m_elements) {
        if(!el.isValid())
            return false;
    }

    return true;
}

void PlyHeader::setFileFormat(const PlyFormatType &format)
{
    m_formatType = format;
}

void PlyHeader::setFileFormatVersion(const float &formatVersion)
{
    m_formatVersion = formatVersion;
}

void PlyHeader::setEndPositionInFile(const qint64 &pos)
{
    m_endPos = pos;
}

qint64 PlyHeader::getEndPositionInFile() const
{
    return m_endPos;
}

QString PlyHeader::getFilePath() const
{
    return m_filepath;
}

PlyFormatType PlyHeader::getFileFormat() const
{
    return m_formatType;
}

QString PlyHeader::getDisplayableFileFormat() const
{
    return PlyTypeConverter::staticPlyFormatTypeToDisplayableString(m_formatType);
}

float PlyHeader::getFileFormatVersion() const
{
    return m_formatVersion;
}

void PlyHeader::addComment(const QString &comment)
{
    m_comments.append(comment);
}

void PlyHeader::addElement(const PlyElement &el)
{
    m_elements.append(el);
}

QList<PlyElement> PlyHeader::getElements() const
{
    return m_elements;
}
