#ifndef CT_ABSTRACTITEM_H
#define CT_ABSTRACTITEM_H

#include "ctlibstructure_global.h"
#include "ct_model/interfacesformodels.h"

#include <QSharedPointer>
#include <iterator>
#include <functional>

class CT_OutAbstractModel;
class CT_AbstractResult;

template<typename InheritedClass, typename InterfaceClass>
class PrototypeModelInterfaceInheritTool : public InheritedClass, public InterfaceClass {
    using SuperClass = InheritedClass;    
public:

    PrototypeModelInterfaceInheritTool() : SuperClass() {
        m_prototypeTool.m_pointer = this;
    }

    PrototypeModelInterfaceInheritTool(const PrototypeModelInterfaceInheritTool<InheritedClass, InterfaceClass>& other) : SuperClass(other) {
        m_prototypeTool.m_pointer = this;
    }

    /**
     * @brief Returns the tool for model
     */
    IPrototypeToolForModel* prototypeToolForModel() const override {
        return &const_cast<PrototypeToolForModel&>(m_prototypeTool);
    }

private:
    class PrototypeToolForModel : public IPrototypeToolForModel {
    public:
        InheritedClass* m_pointer;

        void setModel(const CT_OutAbstractModel* model) override { m_pointer->setModel(model); }
        CT_OutAbstractModel* model() const override { return m_pointer->model(); }
    };

    /**
     * @brief The tool for model
     */
    PrototypeToolForModel   m_prototypeTool;
};

/**
 * @brief Represent an element that can be in the hierarchy of a Result. So it is
 *        the base class of a singular item drawable, a group, an item attribute, etc...
 *
 *        An item know is result and the output model that declare create this type of item
 */
class CTLIBSTRUCTURE_EXPORT CT_AbstractItem : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief A qt style iterator to iterate over CT_AbstractItem
     */
    class IChildrensIteratorQtStyle {
    public:
        virtual ~IChildrensIteratorQtStyle() {}

        /**
         * @brief Returns true if there is at least one item ahead of the
         *        iterator, i.e. the iterator is not at the back of the container;
         *        otherwise returns false.
         */
        virtual bool hasNext() const = 0;

        /**
         * @brief Returns the next item and advances the iterator by one position.
         *        Calling this function on an iterator located at the back of
         *        the container leads to undefined results.
         */
        virtual CT_AbstractItem* next() = 0;

        /**
         * @brief Returns a copy of this object
         */
        virtual IChildrensIteratorQtStyle* copy() const = 0;
    };

    using IChildrensIteratorQtStylePtr = CT_AbstractItem::IChildrensIteratorQtStyle*;

    class ChildrensCollection;

    template<typename ItemT>
    class ChildrensCollectionT;

    /**
     * @brief An object that convert a qt style iterator to a std style iterator for item of specified type
     */
    template<typename ItemT>
    class ChildrensIteratorStdStyleT : public std::iterator<std::input_iterator_tag, ItemT*> {
    public:
        using self_type = ChildrensIteratorStdStyleT<ItemT>;

        ChildrensIteratorStdStyleT() : m_iterator(nullptr), m_currentValue(nullptr) {}
        ChildrensIteratorStdStyleT(const self_type& other) :
            m_iterator(other.m_iterator != nullptr ? other.m_iterator->copy() : nullptr),
            m_currentValue(other.m_currentValue)
        {
        }

        ~ChildrensIteratorStdStyleT() {
            delete m_iterator;
        }

        self_type& operator++() {
            if(m_currentValue == nullptr)
                return *this;

            if(m_iterator->hasNext())
                m_currentValue = static_cast<ItemT*>(m_iterator->next());
            else
                m_currentValue = nullptr;

            return *this;
        }

        typename std::iterator<std::input_iterator_tag, ItemT*>::reference operator*() { return m_currentValue; }
        typename std::iterator<std::input_iterator_tag, ItemT*>::pointer operator->() { return &m_currentValue; }
        bool operator==(const self_type& rhs) { return m_currentValue == rhs.m_currentValue; }
        bool operator!=(const self_type& rhs) { return m_currentValue != rhs.m_currentValue; }
        self_type& operator=(const self_type& other) {
            delete this->m_iterator;
            this->m_iterator = other.m_iterator != nullptr ? other.m_iterator->copy() : nullptr;
            this->m_currentValue = other.m_currentValue;
            return *this;
        }

        template<typename ItemY>
        friend class ChildrensCollectionT;

        ChildrensIteratorStdStyleT(IChildrensIteratorQtStylePtr it) : m_iterator(it), m_currentValue(nullptr) {
            if((m_iterator != nullptr) && m_iterator->hasNext())
                m_currentValue = static_cast<ItemT*>(m_iterator->next());
        }

    private:
        IChildrensIteratorQtStylePtr        m_iterator;
        ItemT*                              m_currentValue;
    };

    /**
     * @brief An object that is useful to iterate over childrens of specified type
     */
    template<typename ItemT>
    class ChildrensCollectionT {
    public:
        using const_iterator = ChildrensIteratorStdStyleT<ItemT>;
        using self_type = ChildrensCollectionT<ItemT>;

        ChildrensCollectionT() : m_iterator(nullptr) {}
        ChildrensCollectionT(IChildrensIteratorQtStylePtr it) : m_iterator(it){}
        ChildrensCollectionT(const self_type& other) :
            m_iterator(other.m_iterator != nullptr ? other.m_iterator->copy() : nullptr)
        {
        }

        ~ChildrensCollectionT() {
            delete m_iterator;
        }

        /**
         * @brief Returns a const STL-style iterator pointing to the first child in the collection of childrens that must be keeped.
         */
        const_iterator begin() const { return const_iterator(m_iterator != nullptr ? m_iterator->copy() : nullptr); }

        /**
         * @brief Returns a const STL-style iterator pointing to the imaginary child after the element item in the collection of childrens that must be keeped.
         */
        const_iterator end() const { return const_iterator(); }

        self_type& operator=(const self_type& other) {
            delete this->m_iterator;
            this->m_iterator = other.m_iterator != nullptr ? other.m_iterator->copy() : nullptr;
            return *this;
        }

    private:
        IChildrensIteratorQtStylePtr m_iterator;
    };

    /**
     * @brief An object that is useful to iterate over childrens
     */
    class ChildrensCollection : public ChildrensCollectionT<CT_AbstractItem> {
        using SuperClass = ChildrensCollectionT<CT_AbstractItem>;
    public:
        ChildrensCollection() : SuperClass() {}
        ChildrensCollection(IChildrensIteratorQtStylePtr it) : SuperClass(it) {}
        ChildrensCollection(const ChildrensCollection& other) : SuperClass(other) {}
        ChildrensCollection& operator=(const ChildrensCollection& other) {
            SuperClass::operator =(other);
            return *this;
        }
    };

public:
    /**
     * @brief A childrens visitor is a function that will receive a const pointer to an item
     *        and return true if visit must continue or false when it must be aborted.
     */
    using ChildrensVisitor = std::function<bool(const CT_AbstractItem*)>;

    /**
     * @brief Constructor from model
     */
    CT_AbstractItem();

    /**
     * @brief Copy constructor.
     *
     *        What is copied :
     *          - Pointer of the result and model of the original item.
     */
    CT_AbstractItem(const CT_AbstractItem& other);

    virtual ~CT_AbstractItem();

    /**
     * @brief Returns the model of the element
     */
    CT_OutAbstractModel* model() const;

    /**
     * @brief Returns the model of the element statically casted to another type
     */
    template<typename T>
    T* modelStaticT() const { return static_cast<T*>(model()); }

    /**
     * @brief Returns the result that contains this element
     */
    CT_AbstractResult* result() const;

    /**
     * @brief Returns the result of the element statically casted to another type
     */
    template<typename T>
    T* resultStaticT() const { return static_cast<T*>(result()); }

    /**
     * @brief Visit childrens but only for tree view. By default call the method "visitChildrens"
     * @return Returns true if no childrens has been visited otherwise returns the result of the visitor.
     */
    virtual bool visitChildrensForTreeView(const ChildrensVisitor& visitor) const;

    /**
     * @brief Returns the number of childrens
     */
    virtual int nChildrens() const = 0;

    /**
     * @brief Visit childrens
     * @return Returns true if no childrens has been visited otherwise returns the result of the visitor.
     */
    virtual bool visitChildrens(const ChildrensVisitor& visitor) const = 0;

    /**
     * @brief Visit childrens that use the specified out model
     * @param outModel : the out model of child
     * @return Returns true if no childrens has been visited otherwise returns the result of the visitor.
     */
    //virtual bool visitChildrensThatUseOutModel(const CT_OutAbstractModel* outModel, const ChildrensVisitor& visitor) const = 0;

    /**
     * @brief Recursively visit childrens
     * @return Returns true if no childrens has been visited otherwise returns the result of the visitor.
     */
    bool recursiveVisitChildrens(const ChildrensVisitor& visitor) const;

    /**
     * @brief Returns the collection of childrens that use the specified out model. You can use the begin and end method to iterate
     *        over child inside the collection
     * @param outModel : the out model of child
     * @return An object that you can use to iterate over childrens
     */
    ChildrensCollection childrensThatUseOutModel(const CT_OutAbstractModel* outModel) const;

    /**
     * @brief Returns the collection of childrens that use the specified out model. You can use the begin and end method to iterate
     *        over child inside the collection
     * @param outModel : the out model of child
     * @return An object that you can use to iterate over childrens
     */
    template<typename ItemT>
    ChildrensCollectionT<ItemT> childrensThatUseOutModelT(const CT_OutAbstractModel* outModel) const {
        return ChildrensCollectionT<ItemT>(createQtStyleIteratorForChildrensThatUseOutModel(outModel));
    }

    /**
     * @brief Change the result that contains this element. The method "willBeRemovedFromResult" is not called.
     * @param newRes : The result to use now. Can be nullptr.
     * @warning Not intended for direct use by plugin developper
     */
    void setResult(const CT_AbstractResult* newRes);

    /**
     * @brief Called from result or parent item to inform that this item will be deleted from the result passed in parameter
     */
    virtual void willBeRemovedFromResult(const CT_AbstractResult* res);

    /**
     * @brief Change the model of this element.
     * @param newRes : The model to use now. Can be nullptr.
     * @warning Not intended for direct use by plugin developper
     */
    void setModel(const CT_OutAbstractModel* model);

protected:
    /**
     * @brief Create and return a qt style iterator to iterate over childrens (groups or items or etc...) that use the specified out model
     * @param outModel : the out model of childrens
     * @return A new qt style iterator to iterate over childrens (groups or items or etc...) that use the specified out model
     */
    virtual IChildrensIteratorQtStylePtr createQtStyleIteratorForChildrensThatUseOutModel(const CT_OutAbstractModel* outModel) const = 0;

private:
    /**
     * @brief The result that contains this item
     */
    CT_AbstractResult*      m_result;

    /**
     * @brief The output model that declare create this type of item
     */
    CT_OutAbstractModel*    m_model;
};

#endif // CT_ABSTRACTITEM_H
