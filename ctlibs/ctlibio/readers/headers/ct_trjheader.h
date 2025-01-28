#ifndef CT_TRJHEADER_H
#define CT_TRJHEADER_H

#include "ctlibio/ctlibio_global.h"
#include "ct_itemdrawable/ct_fileheader.h"

class CTLIBIO_EXPORT CT_TRJHeader : public CT_FileHeader
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_TRJHeader, CT_FileHeader, TRJ Header)

    using SuperClass = CT_FileHeader;

public:
    CT_TRJHeader();
    CT_TRJHeader(const CT_TRJHeader& other);

    void setData(QString recog, int version, int posCnt, QString desc, double begTime, double endTime, int origNbr, int number, QString group, int hdrSize);

    QString getRecog() const;
    int getVersion() const;
    int getPosCnt() const;
    QString getDesc() const;
    double getBegTime() const;
    double getEndTime() const;
    int getOrigNbr() const;
    int getNumber() const;
    QString getGroup() const;
    int getHdrSize() const;

    CT_ITEM_COPY_IMP(CT_TRJHeader)

private:

    QString         m_Recog;        // TSCANTRJ
    int             m_Version;      // File version 20010715
    int             m_PosCnt;       // Number of position records
    QString         m_Desc;         // Description
    double          m_BegTime;      // First time stamp
    double          m_EndTime;      // Last time stamp
    int             m_OrigNbr;      // Original number (before any splitting)
    int             m_Number;       // Line number (in laser points)
    QString         m_Group;        // Group (session description)
    int             m_HdrSize ;


    CT_DEFAULT_IA_BEGIN(CT_TRJHeader)
    CT_DEFAULT_IA_V2(CT_TRJHeader, CT_AbstractCategory::staticInitDataValue(), &CT_TRJHeader::getRecog, QObject::tr("TSCANTRJ"))
    CT_DEFAULT_IA_V2(CT_TRJHeader, CT_AbstractCategory::staticInitDataValue(), &CT_TRJHeader::getVersion, QObject::tr("File version"))
    CT_DEFAULT_IA_V2(CT_TRJHeader, CT_AbstractCategory::staticInitDataValue(), &CT_TRJHeader::getPosCnt, QObject::tr("Number of position records"))
    CT_DEFAULT_IA_V2(CT_TRJHeader, CT_AbstractCategory::staticInitDataValue(), &CT_TRJHeader::getDesc, QObject::tr("Description"))
    CT_DEFAULT_IA_V2(CT_TRJHeader, CT_AbstractCategory::staticInitDataValue(), &CT_TRJHeader::getBegTime, QObject::tr("First time stamp"))
    CT_DEFAULT_IA_V2(CT_TRJHeader, CT_AbstractCategory::staticInitDataValue(), &CT_TRJHeader::getEndTime, QObject::tr("Last time stamp"))
    CT_DEFAULT_IA_V2(CT_TRJHeader, CT_AbstractCategory::staticInitDataValue(), &CT_TRJHeader::getOrigNbr, QObject::tr("Original number"))
    CT_DEFAULT_IA_V2(CT_TRJHeader, CT_AbstractCategory::staticInitDataValue(), &CT_TRJHeader::getNumber, QObject::tr("Line number"))
    CT_DEFAULT_IA_V2(CT_TRJHeader, CT_AbstractCategory::staticInitDataValue(), &CT_TRJHeader::getGroup, QObject::tr("Group"))
    CT_DEFAULT_IA_V2(CT_TRJHeader, CT_AbstractCategory::staticInitDataValue(), &CT_TRJHeader::getHdrSize, QObject::tr("HdrSize"))
    CT_DEFAULT_IA_END(CT_TRJHeader)
};

#endif // CT_TRJHEADER_H
