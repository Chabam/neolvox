#include "ct_trjheader.h"

CT_DEFAULT_IA_INIT(CT_TRJHeader)

CT_TRJHeader::CT_TRJHeader() : SuperClass()
{
    m_Version = 0;
    m_PosCnt = 0;
    m_BegTime = 0;
    m_EndTime = 0;
    m_OrigNbr = 0;
    m_Number = 0;
    m_HdrSize = 0;
}

CT_TRJHeader::CT_TRJHeader(const CT_TRJHeader &other) : SuperClass(other)
{
    m_Recog = other.m_Recog;
    m_Version = other.m_Version;
    m_PosCnt = other.m_PosCnt;
    m_Desc = other.m_Desc;
    m_BegTime = other.m_BegTime;
    m_EndTime = other.m_EndTime;
    m_OrigNbr = other.m_OrigNbr;
    m_Number = other.m_Number;
    m_Group = other.m_Group;
    m_HdrSize = other.m_HdrSize;
}

void CT_TRJHeader::setData(QString recog, int version, int posCnt, QString desc, double begTime, double endTime, int origNbr, int number, QString group, int hdrSize)
{
    m_Recog = recog;
    m_Version = version;
    m_PosCnt = posCnt;
    m_Desc = desc;
    m_BegTime = begTime;
    m_EndTime = endTime;
    m_OrigNbr = origNbr;
    m_Number = number;
    m_Group = group;
    m_HdrSize = hdrSize;
}

QString CT_TRJHeader::getRecog() const {return m_Recog;}
int CT_TRJHeader::getVersion() const {return m_Version;}
int CT_TRJHeader::getPosCnt() const {return m_PosCnt;}
QString CT_TRJHeader::getDesc() const {return m_Desc;}
double CT_TRJHeader::getBegTime() const {return m_BegTime;}
double CT_TRJHeader::getEndTime() const {return m_EndTime;}
int CT_TRJHeader::getOrigNbr() const {return m_OrigNbr;}
int CT_TRJHeader::getNumber() const {return m_Number;}
QString CT_TRJHeader::getGroup() const {return m_Group;}
int CT_TRJHeader::getHdrSize() const {return m_HdrSize;}
