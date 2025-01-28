#include <QApplication>

#include "ct_model/inModel/ct_inzeroormoregroupmodel.h"
#include "ct_model/inModel/ct_instdsingularitemmodel.h"
#include "ct_model/inModel/ct_instdgroupmodel.h"
#include "ct_model/inModel/ct_instditemattributemodel.h"
#include "ct_model/inModel/ct_inresultmodelnotneedinputresult.h"
#include "ct_model/inModel/ct_inresultmodelgroup.h"
#include "ct_model/inModel/ct_inresultmodelgrouptocopy.h"

#include "ct_model/outModel/ct_outstdsingularitemmodel.h"
#include "ct_model/outModel/ct_outstdgroupmodel.h"
#include "ct_model/outModel/ct_outopfnodegroupmodel.h"
#include "ct_model/outModel/ct_outstditemattributemodel.h"
#include "ct_model/outModel/ct_outresultmodelgroup.h"
#include "ct_model/outModel/ct_outresultmodelgroupcopy.h"

#include "ct_model/inModel/tools/ct_instdmodelpossibility.h"
#include "ct_model/inModel/tools/ct_instdresultmodelpossibility.h"

#include "ct_model/inModel/handle/ct_handleinresult.h"
#include "ct_model/inModel/handle/ct_handleinresultcopy.h"
#include "ct_model/inModel/handle/ct_handleingroup.h"
#include "ct_model/inModel/handle/ct_handleinzeroormoregroup.h"
#include "ct_model/inModel/handle/ct_handleinitem.h"
#include "ct_model/inModel/handle/ct_handleinitemattribute.h"
#include "ct_model/outModel/handle/ct_handleoutitem.h"

#include "ct_model/outModel/handle/ct_handleoutresult.h"
#include "ct_model/outModel/handle/ct_handleoutgroup.h"
#include "ct_model/outModel/handle/ct_handleoutitem.h"
#include "ct_model/outModel/handle/ct_handleoutitemattribute.h"

#include "ct_model/inModel/manager/ct_inmodelstructuremanager.h"
#include "ct_model/outModel/manager/ct_outmodelstructuremanager.h"

#include "ct_model/inModel/tools/ct_inresultmodelconfigurationmanager.h"

#include "ctg_modelslinkconfigurationflowview.h"

#include <QTimer>

template<typename ModelT>
class PrototypeToolForModel : public IPrototypeToolForModel {
public:
    using ModelType = ModelT;

    PrototypeToolForModel() : m_model(nullptr) {}

    void setModel(const CT_OutAbstractModel* model) { m_model = const_cast<CT_OutAbstractModel*>(model); }
    CT_OutAbstractModel* model() const { return m_model; }

    CT_OutAbstractModel* m_model;
};

template<typename ConcreteClass>
class ItemDrawableToolForModel : public IItemDrawableToolForModel {
public:
    using ItemAttributesVisitor = std::function<bool(const IItemAttributeForModel*)>;

    QString displayableName() const { return itemInheritPath(); }
    QString nameFromType() const { return itemInheritPath(); }
    QString itemInheritPath() const { return ConcreteClass::staticType(); }
    bool visitItemHisDefaultAttributes(const ItemAttributesVisitor&) const { return true; }
    IItemDrawableForModel* copyItem() const { return new ConcreteClass(); }
};

template<typename ConcreteClass>
class ItemAttributeToolForModel : public IItemAttributeToolForModel {
public:
    ItemAttributeToolForModel() : m_cat(nullptr) {}

    ICategoryForModel* category() const { return m_cat; }
    int valueType() const { return ConcreteClass::ValueType; }

    IItemAttributeForModel* copyItemAttribute() const {
        ConcreteClass* c = new ConcreteClass();
        c->setCategory(m_cat);
        return c;
    }

    ICategoryForModel* m_cat;
};

template<typename ConcreteClass>
class ResultToolForModel : public IResultToolForModel {
public:
    ConcreteClass*  m_pointer;
    QString         m_name;

    void setResultName(const QString& name) { m_name = name; }

    CT_OutAbstractResultModel* resultModel() const { return static_cast<CT_OutAbstractResultModel*>(m_pointer->prototypeToolForModel()->model()); }

    IResultForModel* copyResult(const CT_OutAbstractResultModel* newModelToUse) const {
        ConcreteClass* c = new ConcreteClass(newModelToUse);
        c->resultToolForModel()->setResultName(m_name);
        return c;
    }
};

class StepToolForModel : public IStepToolForModel {
public:
    using OutResultsVisitor = std::function<bool(const IResultForModel* model)>;

    StepToolForModel() : res(nullptr) {}
    ~StepToolForModel() { delete res; }

    QString displayableCustomName() const { return "Step"; }

    bool isStepAPrototype() const { return true; }

    bool visitStepHisOutResults(const OutResultsVisitor& visitor) const {
        if(res != nullptr)
            return visitor(res);

        return true;
    }

    IResultForModel*            res;
};

//************************************//

template<typename ModelT, class Inherited>
class BaseModelForTestWoCopy : public Inherited {
public:
    using ModelType = ModelT;

    BaseModelForTestWoCopy() {}
    BaseModelForTestWoCopy(const ModelType* model) { m_tool.m_model = const_cast<ModelType*>(model); }

    static QString nameFromType(const QString&) { return "name from type"; }

    void setModel(const ModelType* model) { m_tool.m_model = const_cast<ModelType*>(model); }
    ModelType* model() const { return m_tool.m_model; }

    IPrototypeToolForModel* prototypeToolForModel() const { return &const_cast<PrototypeToolForModel<ModelType>&>(m_tool); }

private:
    PrototypeToolForModel<ModelType> m_tool;
};

template<typename ModelType, class Inherited, class ConcreteClass>
class BaseModelForTest : public BaseModelForTestWoCopy<ModelType, Inherited> {
    using SuperClass = BaseModelForTestWoCopy<ModelType, Inherited>;

public:
    BaseModelForTest() : SuperClass() {}
    BaseModelForTest(const ModelType* model) : SuperClass(model) {}
};

template<class Inherit = IItemDrawableForModel>
class BaseItemForTestWoCopy : public BaseModelForTestWoCopy<CT_OutAbstractItemModel, Inherit>  {
    using SuperClass = BaseModelForTestWoCopy<CT_OutAbstractItemModel, Inherit>;
public:
    BaseItemForTestWoCopy() : SuperClass() {}
    BaseItemForTestWoCopy(const CT_OutAbstractItemModel* model) : SuperClass(model) {}
};

template<class ConcreteClass, class Inherit = IItemDrawableForModel>
class BaseItemForTest : public BaseModelForTest<CT_OutAbstractItemModel, Inherit, ConcreteClass>  {
    typedef BaseModelForTest<CT_OutAbstractItemModel, Inherit, ConcreteClass> SuperClass;
public:
    BaseItemForTest() : SuperClass() {}
    BaseItemForTest(const CT_OutAbstractItemModel* model) : SuperClass(model) {}

    IItemDrawableToolForModel* itemToolForModel() const { return &const_cast<ItemDrawableToolForModel<ConcreteClass>&>(m_itemTool); }

private:
    ItemDrawableToolForModel<ConcreteClass> m_itemTool;
};

class ItemForTest : public BaseItemForTest<ItemForTest, IItemDrawableForModel>, public ISingularItemDrawableForModel {
    typedef BaseItemForTest<ItemForTest, IItemDrawableForModel> SuperClass;
public:
    ItemForTest() : SuperClass() {}
    ItemForTest(const CT_OutAbstractItemModel* model) : SuperClass(model) {}

    static QString staticType() { return QString("ItemForTest"); }
};

class ItemForTest2 : public BaseItemForTest<ItemForTest2, IItemDrawableForModel>, public ISingularItemDrawableForModel {
    typedef BaseItemForTest<ItemForTest2, IItemDrawableForModel> SuperClass;
public:
    ItemForTest2() : SuperClass() {}
    ItemForTest2(const CT_OutAbstractItemModel* model) : SuperClass(model) {}

    static QString staticType() { return QString("ItemForTest2"); }
};

class ItemForTest3 : public BaseItemForTest<ItemForTest3, IItemDrawableForModel>, public ISingularItemDrawableForModel {
    typedef BaseItemForTest<ItemForTest3, IItemDrawableForModel> SuperClass;
public:
    ItemForTest3() : SuperClass() {}
    ItemForTest3(const CT_OutAbstractItemModel* model) : SuperClass(model) {}

    static QString staticType() { return QString("ItemForTest3"); }
};

class StdGroupForTest;

class AbstractGroupForTest : public BaseItemForTestWoCopy<IItemDrawableForModel>, public IGroupForModel {
    typedef BaseItemForTestWoCopy<IItemDrawableForModel> SuperClass;
public:
    AbstractGroupForTest() : SuperClass() {}
    AbstractGroupForTest(const CT_OutAbstractItemModel* model) : SuperClass(model) {}

    static QString staticType() { return QString("AbstractGroupForTest"); }
};

class StdGroupForTest : public AbstractGroupForTest {
    typedef AbstractGroupForTest SuperClass;
public:
    StdGroupForTest() : SuperClass() {}
    StdGroupForTest(const CT_OutAbstractItemModel* model) : SuperClass(model) {}

    static QString staticType() { return SuperClass::staticType() + QString("/StdGroupForModel"); }

    IItemDrawableToolForModel* itemToolForModel() const { return &const_cast<ItemDrawableToolForModel<StdGroupForTest>&>(m_itemTool); }

private:
    ItemDrawableToolForModel<StdGroupForTest> m_itemTool;
};

class OPFGroupForTest : public StdGroupForTest {
    typedef StdGroupForTest SuperClass;
public:
    OPFGroupForTest() : SuperClass() {}
    OPFGroupForTest(const CT_OutAbstractItemModel* model) : SuperClass(model) {}

    static QString staticType() { return SuperClass::staticType() + QString("/OPFGroupForTest"); }

    IItemDrawableToolForModel* itemToolForModel() const { return &const_cast<ItemDrawableToolForModel<OPFGroupForTest>&>(m_opfTool); }

private:
    ItemDrawableToolForModel<OPFGroupForTest> m_opfTool;
};

class AbstractItemAttributeForTest : public BaseModelForTestWoCopy<CT_OutAbstractItemAttributeModel, IItemAttributeForModel>
{
    typedef BaseModelForTestWoCopy<CT_OutAbstractItemAttributeModel, IItemAttributeForModel> SuperClass;
public:
    AbstractItemAttributeForTest() : SuperClass() {}
    AbstractItemAttributeForTest(const CT_OutAbstractItemAttributeModel* model) : SuperClass(model) {}
};

template<int vType>
class ItemAttributeForTest : public AbstractItemAttributeForTest {
    typedef AbstractItemAttributeForTest SuperClass;
public:
    enum { ValueType = vType };
    using CategoryType = ICategoryForModel;

    ItemAttributeForTest() : SuperClass() {}
    ItemAttributeForTest(const CT_OutAbstractItemAttributeModel* model) : SuperClass(model) {}

    void setCategory(const ICategoryForModel* cat) { m_iaTool.m_cat = const_cast<ICategoryForModel*>(cat); }
    ICategoryForModel* category() const { return m_iaTool.m_cat; }

    IItemAttributeToolForModel* itemAttributeToolForModel() const { return &const_cast<ItemAttributeToolForModel<ItemAttributeForTest<vType>>&>(m_iaTool); }

private:
    friend class ItemAttributeToolForModel<ItemAttributeForTest<vType>>;
    ItemAttributeToolForModel<ItemAttributeForTest<vType>> m_iaTool;
};

class ResultGroupForTest : public BaseModelForTest<CT_OutAbstractResultModel, IResultForModel, ResultGroupForTest>, public IResultGroupForModel {
    typedef BaseModelForTest<CT_OutAbstractResultModel, IResultForModel, ResultGroupForTest> SuperClass;
public:
    ResultGroupForTest() : SuperClass() { m_resultTool.m_pointer = this; }
    ResultGroupForTest(const CT_OutAbstractResultModel* model) : SuperClass(model) { m_resultTool.m_pointer = this; }

    IResultToolForModel* resultToolForModel() const { return &const_cast<ResultToolForModel<ResultGroupForTest>&>(m_resultTool); }

    QString name() const { return m_resultTool.m_name; }

private:
    ResultToolForModel<ResultGroupForTest> m_resultTool;
};

class UniqueIndexGeneratorForTest
{
public:
    UniqueIndexGeneratorForTest() : m_index(0) {}

    int generateNewIndex() { return m_index++; }
    int operator()() { return m_index++; }
    int currentIndex() const { return m_index; }

private:
    int m_index;
};

class StepForTest : public IStepForModel {
public:
    IStepToolForModel* stepToolForModel() const { return &const_cast<StepToolForModel&>(m_stepTool); }

    void setResult(IResultForModel* newRes) {
        delete m_stepTool.res;
        m_stepTool.res = newRes;
    }

private:
    StepToolForModel m_stepTool;
};

class CategoryForTest : public ICategoryForModel {
    QString uniqueName() const { return "bla"; }
    bool isEquivalentTo(const QString& categoryType) const { return categoryType == uniqueName(); }
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // output 1 :
    // handleResultOut1 [0]
    //   |
    //   |---handleRootGroupOut1 (possibility for handleGroupIn) [1]
    //          |--- handleGroupOut1_1
    //          |
    //          |--- handleGroupOut1_2 (possibility for handleGroupIn) [2]
    //                  |--- handleItemOut2_1 (possibility for handleItemIn) [3]
    //                  |--- handleItemOut2_2 (possibility for handleItemIn) [3]
    //                  |--- handleItemOut2_3 (possibility for handleItemIn) [3]

    // input :
    // handleResultIn
    //   |
    //   |--- handleRootGroupIn [*]
    //          |--- handleGroupIn
    //          |--- handleItemIn

    // select all possibilties of handleGroupIn
    // add handleItemOut2_1 to possibilities of handleGroupIn

    // output 2 must be :
    // Result [4]
    //   |
    //   |---RootGroup [5]
    //          |--- Group1 [6]
    //          |       |--- handleItemOut2_1 [7]
    //          |--- Group2 [8]
    //          |       |--- handleItemOut2_1 [9]
    //          |--- handleItemOut2_1 [10]

    int currentUniqueIndexForOut = 0;

    CT_OutModelStructureManager outManager1;
    CT_HandleOutResult<ResultGroupForTest>  handleResultOut1;
    CT_HandleOutGroup<StdGroupForTest>          handleRootGroupOut1;
    CT_HandleOutGroup<StdGroupForTest>              handleGroupOut1_1;
    CT_HandleOutItem<ItemForTest>                   handleItemOut1_1;
    CT_HandleOutGroup<StdGroupForTest>              handleGroupOut1_2;
    CT_HandleOutItem<ItemForTest>                   handleItemOut2_1;
    CT_HandleOutItem<ItemForTest>                   handleItemOut2_2;
    CT_HandleOutItem<ItemForTest>                   handleItemOut2_3;
    QList<CT_HandleOutItem<ItemForTest>*>           handleItemsOut2_x;

    outManager1.addResult(handleResultOut1);
    outManager1.setRootGroup(handleResultOut1, handleRootGroupOut1);
    outManager1.addGroup(handleRootGroupOut1, handleGroupOut1_1, "Group 1_1", "description simple");
    outManager1.addItem(handleGroupOut1_1, handleItemOut1_1, "Item 1_1");
    outManager1.addGroup(handleRootGroupOut1, handleGroupOut1_2, "Group 1_2");
    outManager1.addItem(handleGroupOut1_2, handleItemOut2_1, "Item 2_1");
    outManager1.addItem(handleGroupOut1_2, handleItemOut2_2, "Item 2_2");
    outManager1.addItem(handleGroupOut1_2, handleItemOut2_3, "Item 2_3");
    for(int i=0;i<10;++i)
    {
        handleItemsOut2_x.append(new CT_HandleOutItem<ItemForTest>());
        outManager1.addItem(handleGroupOut1_2, *handleItemsOut2_x.last(), QString("Item 2_%1").arg(i+4));
    }
    auto indexGenerator = [&currentUniqueIndexForOut]()->int { return currentUniqueIndexForOut++; };
    outManager1.finalize(indexGenerator);

    CT_InModelStructureManager inManager;
    CT_HandleInResultCopy<ResultGroupForTest>       handleResultIn;
    CT_HandleInZeroOrMoreGroup<StdGroupForTest>         handleRootGroupIn;
    CT_HandleInGroup<StdGroupForTest, 1, -1>                    handleGroupIn;
    CT_HandleInItem<ItemForTest, 1, 2>                             handleItemIn;

    inManager.addResult(handleResultIn);
    inManager.setZeroOrMoreRootGroup(handleResultIn, handleRootGroupIn);
    inManager.addGroup(handleRootGroupIn, handleGroupIn);
    inManager.addItem(handleGroupIn, handleItemIn);

    // find all possibilities in out 1
    handleResultIn.model()->recursiveFindAllPossibilitiesInModel(*handleResultOut1.model());

    CT_InAbstractModel* inModelToFind = handleItemIn.inModelForSavedPossibilities(handleResultIn, 0);
    static_cast<CT_InStdResultModelPossibility*>(handleResultIn.model()->possibilitySavedAt(0))->inResultModel()->recursiveVisitPossibilities([&inModelToFind](const CT_InAbstractModel* inModel, const CT_InStdModelPossibility* possibility) {
        if(inModel == inModelToFind) {
            const_cast<CT_InStdModelPossibility*>(possibility)->setSelected(true);
        }

        return true;
    });

    CTG_ModelsLinkConfigurationFlowView view;

    view.setInResultModelPossibility(static_cast<CT_InStdResultModelPossibility*>(handleResultIn.model()->possibilitySavedAt(0)));
    //view.changeInNodePortType(CTG_ModelsLinkConfigurationFlowView::PT_IN);
    //view.setReadOnly(true);

    view.setWindowTitle("Node-based flow editor");
    view.resize(800, 600);
    view.show();
    /*QTimer tmp;
    tmp.setInterval(3000);
    tmp.setSingleShot(false);
    int portType = 2;
    QObject::connect(&tmp, &QTimer::timeout, [&view, &portType]() {
        if(portType == 1)
            portType = 2;
        else
            portType = 1;

        view.changeInNodePortType(portType);
    });

    tmp.start();*/

    return a.exec();
}
