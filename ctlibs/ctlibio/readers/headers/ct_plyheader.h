#ifndef CT_PLYHEADER_H
#define CT_PLYHEADER_H

#include "ctlibio/ctlibio_global.h"
#include "ct_itemdrawable/ct_fileheader.h"

#include "readers/tools/ply/plyheader.h"

/**
 * @brief Computree ply header
 */
class CTLIBIO_EXPORT CT_PLYHeader : public CT_FileHeader
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_PLYHeader, CT_FileHeader, PLY Header)

    using SuperClass = CT_FileHeader;

public:
    CT_PLYHeader();
    CT_PLYHeader(const CT_PLYHeader& other) = default;

    void setHeader(const PlyHeader& h);

    /**
     * @brief Returns the ply header
     */
    PlyHeader getHeader() const;

    /**
     * @brief Returns the file format
     */
    QString getDisplayableFileFormat() const;

    /**
     * @brief Returns the file version
     */
    float getFileFormatVersion() const;

    /**
     * @brief Returns the number of elements in the header
     */
    int getNumberOfElements() const;

    /**
     * @brief Returns the number of properties in the header
     */
    int getNumberOfProperties() const;

    /**
     * @brief Returns true if the header is valid
     */
    bool isValid() const;

    virtual bool hasBoundingBox() const override { return false; }

    CT_ITEM_COPY_IMP(CT_PLYHeader)

private:
    PlyHeader   m_header;

    CT_DEFAULT_IA_BEGIN(CT_PLYHeader)
    CT_DEFAULT_IA_V2(CT_PLYHeader, CT_AbstractCategory::staticInitDataValue(), &CT_PLYHeader::getDisplayableFileFormat, QObject::tr("Format"))
    CT_DEFAULT_IA_V2(CT_PLYHeader, CT_AbstractCategory::staticInitDataValue(), &CT_PLYHeader::getFileFormatVersion, QObject::tr("Version"))
    CT_DEFAULT_IA_V2(CT_PLYHeader, CT_AbstractCategory::staticInitDataNumber(), &CT_PLYHeader::getNumberOfElements, QObject::tr("Number of elements"))
    CT_DEFAULT_IA_V2(CT_PLYHeader, CT_AbstractCategory::staticInitDataNumber(), &CT_PLYHeader::getNumberOfProperties, QObject::tr("Number of properties"))
    CT_DEFAULT_IA_V2(CT_PLYHeader, CT_AbstractCategory::staticInitDataValue(), &CT_PLYHeader::isValid, QObject::tr("Is valid"))
    CT_DEFAULT_IA_END(CT_PLYHeader)
};

#endif // CT_PLYHEADER_H
