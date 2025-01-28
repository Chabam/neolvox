#include "ct_instdresultmodelpossibility.h"

#include "ct_model/inModel/abstract/ct_inabstractresultmodel.h"
#include "ct_model/outModel/abstract/ct_outabstractresultmodel.h"

CT_InStdResultModelPossibility::CT_InStdResultModelPossibility() : SuperClass(),
    m_inModel(nullptr),
    m_autoDeleteInResultModel(true)
{
}

CT_InStdResultModelPossibility::CT_InStdResultModelPossibility(const CT_InStdResultModelPossibility& other) : SuperClass(other),
    m_inModel(static_cast<CT_InAbstractResultModel*>(other.m_inModel->copy(true))),
    m_autoDeleteInResultModel(true)
{
}

CT_InStdResultModelPossibility::~CT_InStdResultModelPossibility()
{
    if(m_autoDeleteInResultModel)
        delete m_inModel;
}

CT_InAbstractResultModel* CT_InStdResultModelPossibility::inResultModel() const
{
    return m_inModel;
}

void CT_InStdResultModelPossibility::setAutoDeleteInResultModel(bool autoDeleteIt)
{
    m_autoDeleteInResultModel = autoDeleteIt;
}

bool CT_InStdResultModelPossibility::mustAutoDeleteInResultModel() const
{
    return m_autoDeleteInResultModel;
}

CT_OutAbstractResultModel* CT_InStdResultModelPossibility::outResultModel() const
{
    return static_cast<CT_OutAbstractResultModel*>(outModel());
}

CT_InStdModelPossibility* CT_InStdResultModelPossibility::copy() const
{
    return new CT_InStdResultModelPossibility(*this);
}

bool CT_InStdResultModelPossibility::setSelected(bool check)
{
    if(isSelected() != check) {

        if(!SuperClass::setSelected(check))
            return false;

        for(const CT_InStdModelPossibility* p : m_inModel->savedPossibilities()) {
            if(p->outModel() == outModel())
                return const_cast<CT_InStdModelPossibility*>(p)->setSelected(check);
        }
    }

    return true;
}

void CT_InStdResultModelPossibility::setInResultModel(CT_InAbstractResultModel* model)
{
    if(m_autoDeleteInResultModel)
        delete m_inModel;

    m_inModel = model;
}
