#ifndef CT_XYBHEADER_H
#define CT_XYBHEADER_H

#include "ctlibio/ctlibio_global.h"
#include "ct_itemdrawable/ct_fileheader.h"

class CTLIBIO_EXPORT CT_XYBHeader : public CT_FileHeader
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_XYBHeader, CT_FileHeader, XYB Header)

    using SuperClass = CT_FileHeader;

public:
    CT_XYBHeader();
    CT_XYBHeader(const CT_XYBHeader& other) = default;

    void setScanCenter(const double& cx,
                       const double& cy,
                       const double& cz);

    void setNRows(const int& n);

    void setNCols(const int& n);

    void setOffset(const int& offset);

    double scanCenterX() const {return m_scanCenterX;}
    double scanCenterY() const {return m_scanCenterY;}
    double scanCenterZ() const {return m_scanCenterZ;}
    int nRows() const {return m_nrows;}
    int nCols() const {return m_ncols;}
    int offset() const {return m_offset;}

    CT_ITEM_COPY_IMP(CT_XYBHeader)

private:
    double      m_scanCenterX;
    double      m_scanCenterY;
    double      m_scanCenterZ;
    int         m_nrows;
    int         m_ncols;
    int         m_offset;

    CT_DEFAULT_IA_BEGIN(CT_XYBHeader)
    CT_DEFAULT_IA_V2(CT_XYBHeader, CT_AbstractCategory::staticInitDataValue(), &CT_XYBHeader::scanCenterX, QObject::tr("Scan Center X"))
    CT_DEFAULT_IA_V2(CT_XYBHeader, CT_AbstractCategory::staticInitDataValue(), &CT_XYBHeader::scanCenterY, QObject::tr("Scan Center Y"))
    CT_DEFAULT_IA_V2(CT_XYBHeader, CT_AbstractCategory::staticInitDataValue(), &CT_XYBHeader::scanCenterZ, QObject::tr("Scan Center Z"))
    CT_DEFAULT_IA_V2(CT_XYBHeader, CT_AbstractCategory::staticInitDataValue(), &CT_XYBHeader::nRows, QObject::tr("Scan rows number"))
    CT_DEFAULT_IA_V2(CT_XYBHeader, CT_AbstractCategory::staticInitDataValue(), &CT_XYBHeader::nCols, QObject::tr("Scan columns number"))
    CT_DEFAULT_IA_END(CT_XYBHeader)
};

#endif // CT_XYBHeader_H
