#include "ct_plyheader.h"

CT_DEFAULT_IA_INIT(CT_PLYHeader)

CT_PLYHeader::CT_PLYHeader() : SuperClass()
{
}

void CT_PLYHeader::setHeader(const PlyHeader &h)
{
    m_header = h;
    setFilePath(m_header.getFilePath());
}


PlyHeader CT_PLYHeader::getHeader() const
{
    return m_header;
}

QString CT_PLYHeader::getDisplayableFileFormat() const
{
    return m_header.getDisplayableFileFormat();
}

float CT_PLYHeader::getFileFormatVersion() const
{
    return m_header.getFileFormatVersion();
}

int CT_PLYHeader::getNumberOfElements() const
{
    return m_header.getElements().size();
}

int CT_PLYHeader::getNumberOfProperties() const
{
    int n = 0;

    QList<PlyElement> elements = m_header.getElements();

    foreach (const PlyElement& el, elements) {
        n += el.getProperties().size();
    }

    return n;
}

bool CT_PLYHeader::isValid() const
{
    return m_header.isValid();
}
