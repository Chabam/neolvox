#include "ct_instdmodelpossibilityselectiongroup.h"

#include "ct_model/inModel/tools/ct_instdmodelpossibility.h"

CT_InStdModelPossibilitySelectionGroup::CT_InStdModelPossibilitySelectionGroup()
{
    m_enable = true;
    m_minMustBeSelected = 1;
    m_maxCanBeSelected = -1; // no limit
}

CT_InStdModelPossibilitySelectionGroup::CT_InStdModelPossibilitySelectionGroup(const CT_InStdModelPossibilitySelectionGroup& other) :
    m_enable(other.m_enable),
    m_minMustBeSelected(other.m_minMustBeSelected),
    m_maxCanBeSelected(other.m_maxCanBeSelected)
{
}

CT_InStdModelPossibilitySelectionGroup::~CT_InStdModelPossibilitySelectionGroup()
{
    clearPossibilities();
}

void CT_InStdModelPossibilitySelectionGroup::addPossibility(const CT_InStdModelPossibility *p)
{
    CT_InStdModelPossibility* pp = const_cast<CT_InStdModelPossibility*>(p);

    if(!m_possibilitiesSaved.contains(pp)) {
        m_possibilitiesSaved.append(pp);
        pp->setSelectionGroup(this);
    }
}

bool CT_InStdModelPossibilitySelectionGroup::removePossibility(const CT_InStdModelPossibility *p)
{
    CT_InStdModelPossibility* pp = const_cast<CT_InStdModelPossibility*>(p);

    if(m_possibilitiesSaved.removeOne(pp)) {
        pp->setSelectionGroup(nullptr);
        return true;
    }

    return false;
}

int CT_InStdModelPossibilitySelectionGroup::indexOf(const CT_InStdModelPossibility* p)
{
    return m_possibilitiesSaved.indexOf(const_cast<CT_InStdModelPossibility*>(p));
}

bool CT_InStdModelPossibilitySelectionGroup::isSelected(const CT_InStdModelPossibility *p) const
{
    return m_possibilitiesSelected.contains(const_cast<CT_InStdModelPossibility*>(p));
}

void CT_InStdModelPossibilitySelectionGroup::select(CT_InStdModelPossibility *p)
{
    if(isEnabled()
            && !isSelected(p))
    {
        m_possibilitiesSelected.append(p);

        if(maximumNumberOfPossibilityThatCanBeSelected() == -1)
            return;

        while(m_possibilitiesSelected.size() > maximumNumberOfPossibilityThatCanBeSelected())
            m_possibilitiesSelected.first()->setSelected(false); // this will call the method "unSelect(...)"
    }
}

void CT_InStdModelPossibilitySelectionGroup::unSelect(CT_InStdModelPossibility *p)
{
    m_possibilitiesSelected.removeOne(p);
}

void CT_InStdModelPossibilitySelectionGroup::setEnable(bool en)
{
    m_enable = en;

    if(!m_enable) {
        while(!m_possibilitiesSelected.isEmpty())
            m_possibilitiesSelected.first()->setSelected(false); // this will call the method "unSelect(...)"
    }
}

bool CT_InStdModelPossibilitySelectionGroup::isEnabled() const
{
    return m_enable;
}

void CT_InStdModelPossibilitySelectionGroup::setMinimumNumberOfPossibilityThatMustBeSelected(quint8 n)
{
    m_minMustBeSelected = n;
}

quint8 CT_InStdModelPossibilitySelectionGroup::minimumNumberOfPossibilityThatMustBeSelected() const
{
    return m_minMustBeSelected;
}

void CT_InStdModelPossibilitySelectionGroup::setMaximumNumberOfPossibilityThatCanBeSelected(int n)
{
    m_maxCanBeSelected = n;

    if(n != -1) {
        while(m_possibilitiesSelected.size() > maximumNumberOfPossibilityThatCanBeSelected())
            m_possibilitiesSelected.first()->setSelected(false); // this will call the method "unSelect(...)"
    }
}

int CT_InStdModelPossibilitySelectionGroup::maximumNumberOfPossibilityThatCanBeSelected() const
{
    return m_maxCanBeSelected;
}

bool CT_InStdModelPossibilitySelectionGroup::isValid() const
{
    return (m_minMustBeSelected <= m_possibilitiesSelected.size()) && (m_possibilitiesSelected.size() <= m_maxCanBeSelected);
}

const QList<CT_InStdModelPossibility*>& CT_InStdModelPossibilitySelectionGroup::getPossibilities() const
{
    return m_possibilitiesSaved;
}

const QList<CT_InStdModelPossibility*>& CT_InStdModelPossibilitySelectionGroup::getPossibilitiesSelected() const
{
    return m_possibilitiesSelected;
}

CT_InStdModelPossibilitySelectionGroup::Possibilities CT_InStdModelPossibilitySelectionGroup::savedPossibilities() const
{
    return CT_InStdModelPossibilitySelectionGroup::Possibilities(m_possibilitiesSaved);
}

CT_InStdModelPossibilitySelectionGroup::Possibilities CT_InStdModelPossibilitySelectionGroup::selectedPossibilities() const
{
    return CT_InStdModelPossibilitySelectionGroup::Possibilities(m_possibilitiesSelected);
}

bool CT_InStdModelPossibilitySelectionGroup::visitPossibilities(const CT_InStdModelPossibilitySelectionGroup::PossibilityVisitor& visitor) const
{
    for(const CT_InStdModelPossibility* possibility : m_possibilitiesSaved) {
        if(!visitor(possibility))
            return false;
    }

    return true;
}

bool CT_InStdModelPossibilitySelectionGroup::visitSelectedPossibilities(const CT_InStdModelPossibilitySelectionGroup::PossibilityVisitor& visitor) const
{
    for(const CT_InStdModelPossibility* possibility : m_possibilitiesSelected) {
        if(!visitor(possibility))
            return false;
    }

    return true;
}

int CT_InStdModelPossibilitySelectionGroup::nPossibility() const
{
    return m_possibilitiesSaved.size();
}

int CT_InStdModelPossibilitySelectionGroup::nPossibilitySelected() const
{
    return m_possibilitiesSelected.size();
}

void CT_InStdModelPossibilitySelectionGroup::clearPossibilities()
{
    m_possibilitiesSaved.clear();
    m_possibilitiesSelected.clear();
}

