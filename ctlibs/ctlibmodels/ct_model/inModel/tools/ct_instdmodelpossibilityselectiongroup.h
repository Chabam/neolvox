#ifndef CT_INSTDMODELPOSSIBILITYSELECTIONGROUP_H
#define CT_INSTDMODELPOSSIBILITYSELECTIONGROUP_H

#include "ctlibmodels_global.h"

#include <QList>
#include <functional>

class CT_InStdModelPossibility;

/**
 * @brief Class that must be used with CT_InStdModelPossibility. A selection group manage the selection/deselection of
 *        possibilities. You can set the minimum and maximum number of possible selection and enable/disable the selection
 *        of possibilities.
 */
class CTLIBMODELS_EXPORT CT_InStdModelPossibilitySelectionGroup
{
public:
    class Possibilities {
        using PossibilityCollection = QList<CT_InStdModelPossibility*>;

    public:
        using const_iterator = PossibilityCollection::const_iterator;

        const_iterator begin() const { return collection.begin(); }
        const_iterator end() const { return collection.end(); }

    private:
        friend class CT_InStdModelPossibilitySelectionGroup;

        Possibilities(const PossibilityCollection& c) : collection(c) {}

        const PossibilityCollection& collection;
    };

public:
    using PossibilityVisitor = std::function<bool(const CT_InStdModelPossibility* possibility)>;

    /**
     * @brief Constructor. By default the minimum = 1 and maximum = no limit
     */
    CT_InStdModelPossibilitySelectionGroup();

    /**
     * @brief Copy constructor. Doesn't copy possibilities ! The collection of possibility will be empty
     */
    CT_InStdModelPossibilitySelectionGroup(const CT_InStdModelPossibilitySelectionGroup& other);
    ~CT_InStdModelPossibilitySelectionGroup();

    /**
     * @brief Add a possibility to this group
     */
    void addPossibility(const CT_InStdModelPossibility *p);

    /**
     * @brief Remove a possibility from this group
     */
    bool removePossibility(const CT_InStdModelPossibility *p);

    /**
     * @brief Returns the index of the specified possibility in the collection of possibilities, -1 if not founded !
     */
    int indexOf(const CT_InStdModelPossibility* p);

    /**
     * @brief Returns all possibilities of this group
     */
    const QList<CT_InStdModelPossibility*>& getPossibilities() const;

    /**
     * @brief Returns all selected possibilities of this group
     */
    const QList<CT_InStdModelPossibility*>& getPossibilitiesSelected() const;

    /**
     * @brief Returns an object that you can use to iterate over saved possibilities
     */
    Possibilities savedPossibilities() const;

    /**
     * @brief Returns an object that you can use to iterate over selected possibilities
     */
    Possibilities selectedPossibilities() const;

    /**
     * @brief Visit all possibilities (selected or not) of this group
     * @param visitor : a lambda expression or a function member that receive a pointer to a
     *                  CT_InStdModelPossibility and return a bool (true to continue the visit, false to stop the visit)
     * @return Returns false if the visitor has stopped the visit
     */
    bool visitPossibilities(const PossibilityVisitor& visitor) const;

    /**
     * @brief Visit all selected possibilities of this group
     * @param visitor : a lambda expression or a function member that receive a pointer to a
     *                  CT_InStdModelPossibility and return a bool (true to continue the visit, false to stop the visit)
     * @return Returns false if the visitor has stopped the visit
     */
    bool visitSelectedPossibilities(const PossibilityVisitor& visitor) const;

    /**
     * @brief Returns the number of possibilities
     */
    int nPossibility() const;

    /**
     * @brief Returns the number of possibility selected
     */
    int nPossibilitySelected() const;

    /**
     * @brief Remove all possibilities from this group
     */
    void clearPossibilities();

    /**
     * @brief Returns true if this possibility is selected
     */
    bool isSelected(const CT_InStdModelPossibility* p) const;

    /**
     * @brief Select this possibility
     */
    void select(CT_InStdModelPossibility* p);

    /**
     * @brief DeSelect this possibility
     */
    void unSelect(CT_InStdModelPossibility* p);

    /**
     * @brief Enable/Disable the selection of possibility. If you set this parameter to false
     *        all selected possibilities will be unselected.
     */
    void setEnable(bool en);

    /**
     * @brief Returns true if this group is enabled.
     */
    bool isEnabled() const;

    /**
     * @brief Set the minimum number of possibility that must be selected in this group to be valid.
     * @warning No check is made to define if minimum is lower than maximum, you are responsible to do this check.
     */
    void setMinimumNumberOfPossibilityThatMustBeSelected(quint8 n);

    /**
     * @brief Returns the minimum number of possibility that must be selected in this group to be valid.
     */
    quint8 minimumNumberOfPossibilityThatMustBeSelected() const;

    /**
     * @brief Set the maximum number of possibility that can be selected in this group. -1 if you want that it was no limit.
     * @warning No check is made to define if minimum is lower than maximum, you are responsible to do this check.
     * @warning If n change and the number of possibility selected is greather than n, all last selected possibilities will be removed while
     *          the number of selected possibilities is equal to n.
     */
    void setMaximumNumberOfPossibilityThatCanBeSelected(int n);

    /**
     * @brief Returns the maximum number of possibility that can be selected in this group.
     */
    int maximumNumberOfPossibilityThatCanBeSelected() const;

    /**
     * @brief Returns true if this group is valid. If number of possibilities selected match the minimum and maximum criteria.
     */
    bool isValid() const;

private:
    using PossibilityCollection = Possibilities::PossibilityCollection;

    PossibilityCollection   m_possibilitiesSaved;
    PossibilityCollection   m_possibilitiesSelected;
    bool                    m_enable;
    quint8                  m_minMustBeSelected;
    int                     m_maxCanBeSelected;
};

#endif // CT_INSTDMODELPOSSIBILITYSELECTIONGROUP_H
