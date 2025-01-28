#include "ct_instdmodelpossibility.h"

#include "ct_model/inModel/tools/ct_instdmodelpossibilityselectiongroup.h"
#include "ct_model/outModel/abstract/ct_outabstractmodel.h"

#include <QDebug>

CT_InStdModelPossibility::CT_InStdModelPossibility()
{
    m_outModel = nullptr;
    m_selectionGroup = nullptr;
    m_autoDeleteOutModel = false;
}

CT_InStdModelPossibility::CT_InStdModelPossibility(const CT_InStdModelPossibility& other) : QObject( nullptr )
{
    m_outModel = other.m_outModel;
    m_selectionGroup = other.m_selectionGroup;
    m_autoDeleteOutModel = false;

    setSelected(other.isSelected());
}

CT_InStdModelPossibility::~CT_InStdModelPossibility()
{
    clearOutModel();

    if(m_selectionGroup != nullptr)
        m_selectionGroup->removePossibility(this);
}

void CT_InStdModelPossibility::setOutModel(const CT_OutAbstractModel &model)
{
    clearOutModel();

    m_outModel = const_cast<CT_OutAbstractModel*>(&model);
}

void CT_InStdModelPossibility::setAutoDeleteOutModel(bool e)
{
    m_autoDeleteOutModel = e;
}

bool CT_InStdModelPossibility::mustAutoDeleteOutModel() const
{
    return m_autoDeleteOutModel;
}

CT_OutAbstractModel* CT_InStdModelPossibility::outModel() const
{
    return m_outModel;
}

bool CT_InStdModelPossibility::isSelected() const
{
    MODELS_ASSERT(m_selectionGroup != nullptr);
    if (m_selectionGroup == nullptr) {qDebug() << "CT_InStdModelPossibility::isSelected" << ", " <<  "m_selectionGroup == nullptr"; return false;}

    return m_selectionGroup->isSelected(const_cast<CT_InStdModelPossibility*>(this));
}

bool CT_InStdModelPossibility::setSelected(bool check)
{
    MODELS_ASSERT(m_selectionGroup != nullptr);
    if (m_selectionGroup == nullptr) {qDebug() << "CT_InStdModelPossibility::setSelected" << ", " <<  "m_selectionGroup == nullptr"; return false;}

    const bool lastValue = isSelected();

    if(check)
        m_selectionGroup->select(this);
    else
        m_selectionGroup->unSelect(this);

    const bool newValue = isSelected();

    if(lastValue != newValue)
        emit selectStateChange(newValue);

    return (newValue == check);
}

void CT_InStdModelPossibility::clearOutModel()
{
    if(m_autoDeleteOutModel)
        delete m_outModel;

    m_outModel = nullptr;
}

void CT_InStdModelPossibility::setSelectionGroup(const CT_InStdModelPossibilitySelectionGroup *group)
{
    m_selectionGroup = const_cast<CT_InStdModelPossibilitySelectionGroup*>(group);
}

CT_InStdModelPossibilitySelectionGroup* CT_InStdModelPossibility::selectionGroup() const
{
    return m_selectionGroup;
}

CT_InStdModelPossibility* CT_InStdModelPossibility::copy() const
{
    return new CT_InStdModelPossibility(*this);
}

