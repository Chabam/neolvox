
class TestException {};

#define MODELS_ASSERT(cond) ((cond) ? static_cast<void>(0) : throw TestException{})

#include <QString>
#include <QtTest>

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
#include "ct_model/inModel/views/ctg_inmodelpossibilitieschoice.h"

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
    QString nameFromType() const { return QString(); }
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
    QString valueTypeToString() const { return QString(); }

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

class TestModels : public QObject
{
    Q_OBJECT

public:
    typedef CT_OutStdGroupModel<StdGroupForTest> CT_OutStdGroupModelForTest;
    TestModels();

private Q_SLOTS:
    void testInZeroOrMoreCreation();
    void testInSingularItemCreation();
    void testInStdGroupCreation();
    void testInStdItemAttributeCreation();
    void testInResultNotNeedInputCreation();
    void testInResultGroupCreation();
    void testInResultGroupToCopyCreation();

    void testOutSingularItemCreation();
    void testOutStdGroupCreation();
    void testOutOpfGroupCreation();
    void testOutStdItemAttributeCreation();
    void testOutResultGroupCreation();
    void testOutResultGroupCopyCreation();

    void testResearchInZeroOrMoreWithMultipleItemAtDifferentLevel();
    void testResearchInZeroOrMoreWithMultipleItemAtDifferentLevelBis();
    void testResearchInZeroOrMoreWithMultipleItemAtDifferentLevelAndOfDifferentType();
    void testResearchInZeroOrMoreWithOnGroupWithMultipleItemAtDifferentLevelAndOfDifferentType();
    void testNotCompatibleInZeroOrMoreWithOnGroupWithMultipleItemAtDifferentLevelAndOfDifferentType();

    void testInHandleStructureStaticCheck();
    void testOutHandleStructureStaticCheck();
    void testInHandleStructureCreation();
    void testInHandleStructureCreationWithAbstractHandle();
    void testOutHandleStructureCreation();

    void testHandleResultCopyAddItemRemoveGroup();

    void testWidgetSelection();
    void testWidgetSelection2();
};

TestModels::TestModels()
{
}

void TestModels::testInZeroOrMoreCreation()
{
    CT_InZeroOrMoreGroupModel model(StdGroupForTest::staticType(), "name from type");

    QVERIFY(model.displayableName() != QString());
    QVERIFY(model.shortDescription() != QString());
    QVERIFY(model.detailledDescription() == model.shortDescription());
    QVERIFY(model.isEmpty() == true);
    QVERIFY(model.parentModel() == nullptr);
    QVERIFY(model.rootModel() == &model);
    QVERIFY(model.step() == nullptr);
    QVERIFY(model.minimumNumberOfPossibilityToSelect() == 0);
    QVERIFY(model.maximumNumberOfPossibilityThatCanBeSelected() == 0);
    QVERIFY(model.isOptionnal() == true);
    QVERIFY(model.isObligatory() == false);
    QVERIFY(model.needOutputModel() == false);
    QVERIFY(model.nSaveCycle() == 0);
    QVERIFY(model.isAtLeastOnePossibilitySelected() == false);
    QVERIFY(model.recursiveIsAtLeastOnePossibilitySelected() == false);
    QVERIFY(model.hasTheMinimumNumberOfSelectedPossibilityRequired() == true);
    QVERIFY(model.recursiveHasTheMinimumNumberOfSelectedPossibilityRequired() == true);
    QVERIFY(model.nPossibilitySaved() == 0);
    QVERIFY(model.nPossibilitySelected() == 0);
    QVERIFY(model.itemType() != QString());
    QVERIFY(model.itemShortType() != QString());
    QVERIFY(model.parentGroup() == nullptr);
    QVERIFY(model.rootGroup() == &model);

    CT_InZeroOrMoreGroupModel* cpy = static_cast<CT_InZeroOrMoreGroupModel*>(model.copy(true));
    QVERIFY(cpy->displayableName() == model.displayableName());
    QVERIFY(cpy->shortDescription() == model.shortDescription());
    QVERIFY(cpy->detailledDescription() == model.detailledDescription());
    QVERIFY(cpy->isEmpty() == model.isEmpty());
    QVERIFY(cpy->parentModel() == model.parentModel());
    QVERIFY(cpy->rootModel() == cpy);
    QVERIFY(cpy->step() == model.step());
    QVERIFY(cpy->minimumNumberOfPossibilityToSelect() == model.minimumNumberOfPossibilityToSelect());
    QVERIFY(cpy->maximumNumberOfPossibilityThatCanBeSelected() == model.maximumNumberOfPossibilityThatCanBeSelected());
    QVERIFY(cpy->isOptionnal() == model.isOptionnal());
    QVERIFY(cpy->isObligatory() == model.isObligatory());
    QVERIFY(cpy->needOutputModel() == model.needOutputModel());
    QVERIFY(cpy->nSaveCycle() == model.nSaveCycle());
    QVERIFY(cpy->isAtLeastOnePossibilitySelected() == model.isAtLeastOnePossibilitySelected());
    QVERIFY(cpy->recursiveIsAtLeastOnePossibilitySelected() == model.recursiveIsAtLeastOnePossibilitySelected());
    QVERIFY(cpy->hasTheMinimumNumberOfSelectedPossibilityRequired() == model.hasTheMinimumNumberOfSelectedPossibilityRequired());
    QVERIFY(cpy->recursiveHasTheMinimumNumberOfSelectedPossibilityRequired() == model.recursiveHasTheMinimumNumberOfSelectedPossibilityRequired());
    QVERIFY(cpy->nPossibilitySaved() == model.nPossibilitySaved());
    QVERIFY(cpy->nPossibilitySelected() == model.nPossibilitySelected());
    QVERIFY(cpy->itemType() == model.itemType());
    QVERIFY(cpy->itemShortType() == model.itemShortType());
    QVERIFY(cpy->parentGroup() == nullptr);
    QVERIFY(cpy->rootGroup() == cpy);

    delete cpy;
}

void TestModels::testInSingularItemCreation()
{
    const QString itemType("coucou/hello");
    const QString itemNameFromType("name from type");
    const QString itemShortType("hello");
    const QString displayableName("dn");
    const QString shortDescription("sd");
    const QString detailledDescription("dd");
    const int minSelect = 3;
    const int maxSelect = 5;

    CT_InStdSingularItemModel model(itemType,
                                    itemNameFromType,
                                    displayableName,
                                    shortDescription,
                                    detailledDescription,
                                    minSelect,
                                    maxSelect);

    QVERIFY(model.displayableName() == displayableName);
    QVERIFY(model.shortDescription() == shortDescription);
    QVERIFY(model.detailledDescription() == detailledDescription);
    QVERIFY(model.isEmpty() == true);
    QVERIFY(model.parentModel() == nullptr);
    QVERIFY(model.rootModel() == &model);
    QVERIFY(model.step() == nullptr);
    QVERIFY(model.minimumNumberOfPossibilityToSelect() == minSelect);
    QVERIFY(model.maximumNumberOfPossibilityThatCanBeSelected() == maxSelect);
    QVERIFY(model.isOptionnal() == false);
    QVERIFY(model.isObligatory() == true);
    QVERIFY(model.needOutputModel() == true);
    QVERIFY(model.nSaveCycle() == 0);
    QVERIFY(model.isAtLeastOnePossibilitySelected() == false);
    QVERIFY(model.recursiveIsAtLeastOnePossibilitySelected() == false);
    QVERIFY(model.hasTheMinimumNumberOfSelectedPossibilityRequired() == false);
    QVERIFY(model.recursiveHasTheMinimumNumberOfSelectedPossibilityRequired() == false);
    QVERIFY(model.nPossibilitySaved() == 0);
    QVERIFY(model.nPossibilitySelected() == 0);
    QVERIFY(model.itemType() == itemType);
    QVERIFY(model.itemShortType() == itemShortType);
    QVERIFY(model.itemNameFromType() == itemNameFromType);
    QVERIFY(model.parentGroup() == nullptr);
    QVERIFY(model.rootGroup() == nullptr);

    CT_InStdSingularItemModel* cpy = static_cast<CT_InStdSingularItemModel*>(model.copy(true));
    QVERIFY(cpy->displayableName() == model.displayableName());
    QVERIFY(cpy->shortDescription() == model.shortDescription());
    QVERIFY(cpy->detailledDescription() == model.detailledDescription());
    QVERIFY(cpy->isEmpty() == model.isEmpty());
    QVERIFY(cpy->parentModel() == model.parentModel());
    QVERIFY(cpy->rootModel() == cpy);
    QVERIFY(cpy->step() == model.step());
    QVERIFY(cpy->minimumNumberOfPossibilityToSelect() == model.minimumNumberOfPossibilityToSelect());
    QVERIFY(cpy->maximumNumberOfPossibilityThatCanBeSelected() == model.maximumNumberOfPossibilityThatCanBeSelected());
    QVERIFY(cpy->isOptionnal() == model.isOptionnal());
    QVERIFY(cpy->isObligatory() == model.isObligatory());
    QVERIFY(cpy->needOutputModel() == model.needOutputModel());
    QVERIFY(cpy->nSaveCycle() == model.nSaveCycle());
    QVERIFY(cpy->isAtLeastOnePossibilitySelected() == model.isAtLeastOnePossibilitySelected());
    QVERIFY(cpy->recursiveIsAtLeastOnePossibilitySelected() == model.recursiveIsAtLeastOnePossibilitySelected());
    QVERIFY(cpy->hasTheMinimumNumberOfSelectedPossibilityRequired() == model.hasTheMinimumNumberOfSelectedPossibilityRequired());
    QVERIFY(cpy->recursiveHasTheMinimumNumberOfSelectedPossibilityRequired() == model.recursiveHasTheMinimumNumberOfSelectedPossibilityRequired());
    QVERIFY(cpy->nPossibilitySaved() == model.nPossibilitySaved());
    QVERIFY(cpy->nPossibilitySelected() == model.nPossibilitySelected());
    QVERIFY(cpy->itemType() == model.itemType());
    QVERIFY(cpy->itemShortType() == model.itemShortType());
    QVERIFY(cpy->parentGroup() == nullptr);
    QVERIFY(cpy->rootGroup() == nullptr);

    delete cpy;
}

void TestModels::testInStdGroupCreation()
{
    const QString groupType("coucou/hello");
    const QString groupNameFromType("name from type");
    const QString groupShortType("hello");
    const QString displayableName("dn");
    const QString shortDescription("sd");
    const QString detailledDescription("dd");
    const int minSelect = 3;
    const int maxSelect = 5;

    CT_InStdGroupModel model(groupType,
                             groupNameFromType,
                             displayableName,
                             shortDescription,
                             detailledDescription,
                             minSelect,
                             maxSelect);

    QVERIFY(model.itemType() == groupType);
    QVERIFY(model.displayableName() == displayableName);
    QVERIFY(model.shortDescription() == shortDescription);
    QVERIFY(model.detailledDescription() == detailledDescription);
    QVERIFY(model.isEmpty() == true);
    QVERIFY(model.parentModel() == nullptr);
    QVERIFY(model.rootModel() == &model);
    QVERIFY(model.step() == nullptr);
    QVERIFY(model.minimumNumberOfPossibilityToSelect() == minSelect);
    QVERIFY(model.maximumNumberOfPossibilityThatCanBeSelected() == maxSelect);
    QVERIFY(model.isOptionnal() == false);
    QVERIFY(model.isObligatory() == true);
    QVERIFY(model.needOutputModel() == true);
    QVERIFY(model.nSaveCycle() == 0);
    QVERIFY(model.isAtLeastOnePossibilitySelected() == false);
    QVERIFY(model.recursiveIsAtLeastOnePossibilitySelected() == false);
    QVERIFY(model.hasTheMinimumNumberOfSelectedPossibilityRequired() == false);
    QVERIFY(model.recursiveHasTheMinimumNumberOfSelectedPossibilityRequired() == false);
    QVERIFY(model.nPossibilitySaved() == 0);
    QVERIFY(model.nPossibilitySelected() == 0);
    QVERIFY(model.itemType() == groupType);
    QVERIFY(model.itemNameFromType() == groupNameFromType);
    QVERIFY(model.itemShortType() == groupShortType);
    QVERIFY(model.parentGroup() == nullptr);
    QVERIFY(model.rootGroup() == &model);

    CT_InStdGroupModel* cpy = static_cast<CT_InStdGroupModel*>(model.copy(true));
    QVERIFY(cpy->displayableName() == model.displayableName());
    QVERIFY(cpy->shortDescription() == model.shortDescription());
    QVERIFY(cpy->detailledDescription() == model.detailledDescription());
    QVERIFY(cpy->isEmpty() == model.isEmpty());
    QVERIFY(cpy->parentModel() == model.parentModel());
    QVERIFY(cpy->rootModel() == cpy);
    QVERIFY(cpy->step() == model.step());
    QVERIFY(cpy->minimumNumberOfPossibilityToSelect() == model.minimumNumberOfPossibilityToSelect());
    QVERIFY(cpy->maximumNumberOfPossibilityThatCanBeSelected() == model.maximumNumberOfPossibilityThatCanBeSelected());
    QVERIFY(cpy->isOptionnal() == model.isOptionnal());
    QVERIFY(cpy->isObligatory() == model.isObligatory());
    QVERIFY(cpy->needOutputModel() == model.needOutputModel());
    QVERIFY(cpy->nSaveCycle() == model.nSaveCycle());
    QVERIFY(cpy->isAtLeastOnePossibilitySelected() == model.isAtLeastOnePossibilitySelected());
    QVERIFY(cpy->recursiveIsAtLeastOnePossibilitySelected() == model.recursiveIsAtLeastOnePossibilitySelected());
    QVERIFY(cpy->hasTheMinimumNumberOfSelectedPossibilityRequired() == model.hasTheMinimumNumberOfSelectedPossibilityRequired());
    QVERIFY(cpy->recursiveHasTheMinimumNumberOfSelectedPossibilityRequired() == model.recursiveHasTheMinimumNumberOfSelectedPossibilityRequired());
    QVERIFY(cpy->nPossibilitySaved() == model.nPossibilitySaved());
    QVERIFY(cpy->nPossibilitySelected() == model.nPossibilitySelected());
    QVERIFY(cpy->itemType() == model.itemType());
    QVERIFY(cpy->itemShortType() == model.itemShortType());
    QVERIFY(cpy->parentGroup() == nullptr);
    QVERIFY(cpy->rootGroup() == cpy);

    delete cpy;
}

void TestModels::testInStdItemAttributeCreation()
{
    const QString categoryType("categ");
    const int valueType = 1;
    const QString displayableName("dn");
    const QString shortDescription("sd");
    const QString detailledDescription("dd");
    const int minSelect = 3;
    const int maxSelect = 5;

    CT_InStdItemAttributeModel model(valueType,
                                     categoryType,
                                     displayableName,
                                     shortDescription,
                                     detailledDescription,
                                     minSelect,
                                     maxSelect);

    QVERIFY(model.displayableName() == displayableName);
    QVERIFY(model.shortDescription() == shortDescription);
    QVERIFY(model.detailledDescription() == detailledDescription);
    QVERIFY(model.isEmpty() == true);
    QVERIFY(model.parentModel() == nullptr);
    QVERIFY(model.rootModel() == &model);
    QVERIFY(model.step() == nullptr);
    QVERIFY(model.minimumNumberOfPossibilityToSelect() == minSelect);
    QVERIFY(model.maximumNumberOfPossibilityThatCanBeSelected() == maxSelect);
    QVERIFY(model.isOptionnal() == false);
    QVERIFY(model.isObligatory() == true);
    QVERIFY(model.needOutputModel() == true);
    QVERIFY(model.nSaveCycle() == 0);
    QVERIFY(model.isAtLeastOnePossibilitySelected() == false);
    QVERIFY(model.recursiveIsAtLeastOnePossibilitySelected() == false);
    QVERIFY(model.hasTheMinimumNumberOfSelectedPossibilityRequired() == false);
    QVERIFY(model.recursiveHasTheMinimumNumberOfSelectedPossibilityRequired() == false);
    QVERIFY(model.nPossibilitySaved() == 0);
    QVERIFY(model.nPossibilitySelected() == 0);
    QVERIFY(model.valueType() == valueType);
    QVERIFY(model.categoriesType().contains(categoryType) == true);

    CT_InStdItemAttributeModel* cpy = static_cast<CT_InStdItemAttributeModel*>(model.copy(true));
    QVERIFY(cpy->displayableName() == model.displayableName());
    QVERIFY(cpy->shortDescription() == model.shortDescription());
    QVERIFY(cpy->detailledDescription() == model.detailledDescription());
    QVERIFY(cpy->isEmpty() == model.isEmpty());
    QVERIFY(cpy->parentModel() == model.parentModel());
    QVERIFY(cpy->rootModel() == cpy);
    QVERIFY(cpy->step() == model.step());
    QVERIFY(cpy->minimumNumberOfPossibilityToSelect() == model.minimumNumberOfPossibilityToSelect());
    QVERIFY(cpy->maximumNumberOfPossibilityThatCanBeSelected() == model.maximumNumberOfPossibilityThatCanBeSelected());
    QVERIFY(cpy->isOptionnal() == model.isOptionnal());
    QVERIFY(cpy->isObligatory() == model.isObligatory());
    QVERIFY(cpy->needOutputModel() == model.needOutputModel());
    QVERIFY(cpy->nSaveCycle() == model.nSaveCycle());
    QVERIFY(cpy->isAtLeastOnePossibilitySelected() == model.isAtLeastOnePossibilitySelected());
    QVERIFY(cpy->recursiveIsAtLeastOnePossibilitySelected() == model.recursiveIsAtLeastOnePossibilitySelected());
    QVERIFY(cpy->hasTheMinimumNumberOfSelectedPossibilityRequired() == model.hasTheMinimumNumberOfSelectedPossibilityRequired());
    QVERIFY(cpy->recursiveHasTheMinimumNumberOfSelectedPossibilityRequired() == model.recursiveHasTheMinimumNumberOfSelectedPossibilityRequired());
    QVERIFY(cpy->nPossibilitySaved() == model.nPossibilitySaved());
    QVERIFY(cpy->nPossibilitySelected() == model.nPossibilitySelected());
    QVERIFY(cpy->valueType() == model.valueType());
    QVERIFY(cpy->categoriesType() == model.categoriesType());

    delete cpy;
}

void TestModels::testInResultNotNeedInputCreation()
{
    CT_InResultModelNotNeedInputResult model;

    QVERIFY(model.displayableName() != QString());
    QVERIFY(model.shortDescription() == model.displayableName());
    QVERIFY(model.detailledDescription() == model.shortDescription());
    QVERIFY(model.isEmpty() == true);
    QVERIFY(model.parentModel() == nullptr);
    QVERIFY(model.rootModel() == &model);
    QVERIFY(model.step() == nullptr);
    QVERIFY(model.minimumNumberOfPossibilityToSelect() == 0);
    QVERIFY(model.maximumNumberOfPossibilityThatCanBeSelected() == 0);
    QVERIFY(model.isOptionnal() == true);
    QVERIFY(model.isObligatory() == false);
    QVERIFY(model.needOutputModel() == false);
    QVERIFY(model.nSaveCycle() == 0);
    QVERIFY(model.isAtLeastOnePossibilitySelected() == false);
    QVERIFY(model.recursiveIsAtLeastOnePossibilitySelected() == false);
    QVERIFY(model.hasTheMinimumNumberOfSelectedPossibilityRequired() == true);
    QVERIFY(model.recursiveHasTheMinimumNumberOfSelectedPossibilityRequired() == true);
    QVERIFY(model.nPossibilitySaved() == 0);
    QVERIFY(model.nPossibilitySelected() == 0);
    QVERIFY(model.isRecursive() == false);
    QVERIFY(model.originalModel() == &model);
    QVERIFY(model.recursiveOriginalModel() == &model);

    CT_InResultModelNotNeedInputResult* cpy = static_cast<CT_InResultModelNotNeedInputResult*>(model.copy(true));
    QVERIFY(cpy->displayableName() == model.displayableName());
    QVERIFY(cpy->shortDescription() == model.shortDescription());
    QVERIFY(cpy->detailledDescription() == model.detailledDescription());
    QVERIFY(cpy->isEmpty() == model.isEmpty());
    QVERIFY(cpy->parentModel() == model.parentModel());
    QVERIFY(cpy->rootModel() == cpy);
    QVERIFY(cpy->step() == model.step());
    QVERIFY(cpy->minimumNumberOfPossibilityToSelect() == model.minimumNumberOfPossibilityToSelect());
    QVERIFY(cpy->maximumNumberOfPossibilityThatCanBeSelected() == model.maximumNumberOfPossibilityThatCanBeSelected());
    QVERIFY(cpy->isOptionnal() == model.isOptionnal());
    QVERIFY(cpy->isObligatory() == model.isObligatory());
    QVERIFY(cpy->needOutputModel() == model.needOutputModel());
    QVERIFY(cpy->nSaveCycle() == model.nSaveCycle());
    QVERIFY(cpy->isAtLeastOnePossibilitySelected() == model.isAtLeastOnePossibilitySelected());
    QVERIFY(cpy->recursiveIsAtLeastOnePossibilitySelected() == model.recursiveIsAtLeastOnePossibilitySelected());
    QVERIFY(cpy->hasTheMinimumNumberOfSelectedPossibilityRequired() == model.hasTheMinimumNumberOfSelectedPossibilityRequired());
    QVERIFY(cpy->recursiveHasTheMinimumNumberOfSelectedPossibilityRequired() == model.recursiveHasTheMinimumNumberOfSelectedPossibilityRequired());
    QVERIFY(cpy->nPossibilitySaved() == model.nPossibilitySaved());
    QVERIFY(cpy->nPossibilitySelected() == model.nPossibilitySelected());
    QVERIFY(cpy->isRecursive() == model.isRecursive());
    QVERIFY(cpy->originalModel() == &model);
    QVERIFY(cpy->recursiveOriginalModel() == &model);

    delete cpy;
}

void TestModels::testInResultGroupCreation()
{
    const QString displayableName("dn");
    const QString shortDescription("sd");
    const bool recursive = true;

    CT_InResultModelGroup model(displayableName, shortDescription, recursive);

    QVERIFY(model.displayableName() == displayableName);
    QVERIFY(model.shortDescription() == shortDescription);
    QVERIFY(model.detailledDescription() == model.shortDescription());
    QVERIFY(model.isEmpty() == true);
    QVERIFY(model.parentModel() == nullptr);
    QVERIFY(model.rootModel() == &model);
    QVERIFY(model.step() == nullptr);
    QVERIFY(model.minimumNumberOfPossibilityToSelect() == 1);
    QVERIFY(model.maximumNumberOfPossibilityThatCanBeSelected() == -1);
    QVERIFY(model.isOptionnal() == false);
    QVERIFY(model.isObligatory() == true);
    QVERIFY(model.needOutputModel() == true);
    QVERIFY(model.nSaveCycle() == 0);
    QVERIFY(model.isAtLeastOnePossibilitySelected() == false);
    QVERIFY(model.recursiveIsAtLeastOnePossibilitySelected() == false);
    QVERIFY(model.hasTheMinimumNumberOfSelectedPossibilityRequired() == false);
    QVERIFY(model.recursiveHasTheMinimumNumberOfSelectedPossibilityRequired() == false);
    QVERIFY(model.nPossibilitySaved() == 0);
    QVERIFY(model.nPossibilitySelected() == 0);
    QVERIFY(model.isRecursive() == recursive);
    QVERIFY(model.originalModel() == &model);
    QVERIFY(model.recursiveOriginalModel() == &model);
    QVERIFY(model.rootGroup() == nullptr);

    CT_InResultModelGroup* cpy = static_cast<CT_InResultModelGroup*>(model.copy(true));
    QVERIFY(cpy->displayableName() == model.displayableName());
    QVERIFY(cpy->shortDescription() == model.shortDescription());
    QVERIFY(cpy->detailledDescription() == model.detailledDescription());
    QVERIFY(cpy->isEmpty() == model.isEmpty());
    QVERIFY(cpy->parentModel() == model.parentModel());
    QVERIFY(cpy->rootModel() == cpy);
    QVERIFY(cpy->step() == model.step());
    QVERIFY(cpy->minimumNumberOfPossibilityToSelect() == model.minimumNumberOfPossibilityToSelect());
    QVERIFY(cpy->maximumNumberOfPossibilityThatCanBeSelected() == model.maximumNumberOfPossibilityThatCanBeSelected());
    QVERIFY(cpy->isOptionnal() == model.isOptionnal());
    QVERIFY(cpy->isObligatory() == model.isObligatory());
    QVERIFY(cpy->needOutputModel() == model.needOutputModel());
    QVERIFY(cpy->nSaveCycle() == model.nSaveCycle());
    QVERIFY(cpy->isAtLeastOnePossibilitySelected() == model.isAtLeastOnePossibilitySelected());
    QVERIFY(cpy->recursiveIsAtLeastOnePossibilitySelected() == model.recursiveIsAtLeastOnePossibilitySelected());
    QVERIFY(cpy->hasTheMinimumNumberOfSelectedPossibilityRequired() == model.hasTheMinimumNumberOfSelectedPossibilityRequired());
    QVERIFY(cpy->recursiveHasTheMinimumNumberOfSelectedPossibilityRequired() == model.recursiveHasTheMinimumNumberOfSelectedPossibilityRequired());
    QVERIFY(cpy->nPossibilitySaved() == model.nPossibilitySaved());
    QVERIFY(cpy->nPossibilitySelected() == model.nPossibilitySelected());
    QVERIFY(cpy->isRecursive() == model.isRecursive());
    QVERIFY(cpy->originalModel() == &model);
    QVERIFY(cpy->recursiveOriginalModel() == &model);
    QVERIFY(cpy->rootGroup() == nullptr);

    delete cpy;
}

void TestModels::testInResultGroupToCopyCreation()
{
    const QString displayableName("dn");
    const QString shortDescription("sd");
    const bool recursive = true;

    CT_InResultModelGroupToCopy model(displayableName, shortDescription, recursive);

    QVERIFY(model.displayableName() == displayableName);
    QVERIFY(model.shortDescription() == shortDescription);
    QVERIFY(model.detailledDescription() == model.shortDescription());
    QVERIFY(model.isEmpty() == true);
    QVERIFY(model.parentModel() == nullptr);
    QVERIFY(model.rootModel() == &model);
    QVERIFY(model.step() == nullptr);
    QVERIFY(model.minimumNumberOfPossibilityToSelect() == 1);
    QVERIFY(model.maximumNumberOfPossibilityThatCanBeSelected() == -1);
    QVERIFY(model.isOptionnal() == false);
    QVERIFY(model.isObligatory() == true);
    QVERIFY(model.needOutputModel() == true);
    QVERIFY(model.nSaveCycle() == 0);
    QVERIFY(model.isAtLeastOnePossibilitySelected() == false);
    QVERIFY(model.recursiveIsAtLeastOnePossibilitySelected() == false);
    QVERIFY(model.hasTheMinimumNumberOfSelectedPossibilityRequired() == false);
    QVERIFY(model.recursiveHasTheMinimumNumberOfSelectedPossibilityRequired() == false);
    QVERIFY(model.nPossibilitySaved() == 0);
    QVERIFY(model.nPossibilitySelected() == 0);
    QVERIFY(model.isRecursive() == recursive);
    QVERIFY(model.originalModel() == &model);
    QVERIFY(model.recursiveOriginalModel() == &model);
    QVERIFY(model.rootGroup() == nullptr);

    CT_InResultModelGroupToCopy* cpy = static_cast<CT_InResultModelGroupToCopy*>(model.copy(true));
    QVERIFY(cpy->displayableName() == model.displayableName());
    QVERIFY(cpy->shortDescription() == model.shortDescription());
    QVERIFY(cpy->detailledDescription() == model.detailledDescription());
    QVERIFY(cpy->isEmpty() == model.isEmpty());
    QVERIFY(cpy->parentModel() == model.parentModel());
    QVERIFY(cpy->rootModel() == cpy);
    QVERIFY(cpy->step() == model.step());
    QVERIFY(cpy->minimumNumberOfPossibilityToSelect() == model.minimumNumberOfPossibilityToSelect());
    QVERIFY(cpy->maximumNumberOfPossibilityThatCanBeSelected() == model.maximumNumberOfPossibilityThatCanBeSelected());
    QVERIFY(cpy->isOptionnal() == model.isOptionnal());
    QVERIFY(cpy->isObligatory() == model.isObligatory());
    QVERIFY(cpy->needOutputModel() == model.needOutputModel());
    QVERIFY(cpy->nSaveCycle() == model.nSaveCycle());
    QVERIFY(cpy->isAtLeastOnePossibilitySelected() == model.isAtLeastOnePossibilitySelected());
    QVERIFY(cpy->recursiveIsAtLeastOnePossibilitySelected() == model.recursiveIsAtLeastOnePossibilitySelected());
    QVERIFY(cpy->hasTheMinimumNumberOfSelectedPossibilityRequired() == model.hasTheMinimumNumberOfSelectedPossibilityRequired());
    QVERIFY(cpy->recursiveHasTheMinimumNumberOfSelectedPossibilityRequired() == model.recursiveHasTheMinimumNumberOfSelectedPossibilityRequired());
    QVERIFY(cpy->nPossibilitySaved() == model.nPossibilitySaved());
    QVERIFY(cpy->nPossibilitySelected() == model.nPossibilitySelected());
    QVERIFY(cpy->isRecursive() == model.isRecursive());
    QVERIFY(cpy->originalModel() == &model);
    QVERIFY(cpy->recursiveOriginalModel() == &model);
    QVERIFY(model.rootGroup() == nullptr);

    delete cpy;
}

void TestModels::testOutSingularItemCreation()
{
    const QString displayableName("dn");
    const QString shortDescription("sd");
    const QString detailledDescription("dd");

    CT_OutStdSingularItemModel<ItemForTest> model(displayableName,
                                                  shortDescription,
                                                  detailledDescription);

    QVERIFY(model.displayableName() == displayableName);
    QVERIFY(model.shortDescription() == shortDescription);
    QVERIFY(model.detailledDescription() == detailledDescription);
    QVERIFY(model.isEmpty() == true);
    QVERIFY(model.parentModel() == nullptr);
    QVERIFY(model.rootModel() == &model);
    QVERIFY(model.step() == nullptr);
    QVERIFY(model.uniqueIndex() == -1);
    QVERIFY(model.recursiveOriginalModel() == &model);
    QVERIFY(model.originalModel() == &model);
    QVERIFY(model.recursiveOriginalModelWithAResult() == nullptr);
    QVERIFY(model.recursiveOriginalModelWithAStep() == nullptr);
    QVERIFY(model.isVisible() == false);
    QVERIFY(model.prototype() != nullptr);
    QVERIFY(model.itemDrawable() == model.prototype());
    QVERIFY(model.result() == nullptr);
    QVERIFY(model.recursiveFinalize() == true);
    QVERIFY(model.parentGroup() == nullptr);
    QVERIFY(model.rootGroup() == nullptr);
}

void TestModels::testOutStdGroupCreation()
{
    const QString displayableName("dn");
    const QString shortDescription("sd");
    const QString detailledDescription("dd");

    CT_OutStdGroupModelForTest model(displayableName,
                                     shortDescription,
                                     detailledDescription);

    QVERIFY(model.displayableName() == displayableName);
    QVERIFY(model.shortDescription() == shortDescription);
    QVERIFY(model.detailledDescription() == detailledDescription);
    QVERIFY(model.isEmpty() == true);
    QVERIFY(model.parentModel() == nullptr);
    QVERIFY(model.rootModel() == &model);
    QVERIFY(model.step() == nullptr);
    QVERIFY(model.uniqueIndex() == -1);
    QVERIFY(model.recursiveOriginalModel() == &model);
    QVERIFY(model.originalModel() == &model);
    QVERIFY(model.recursiveOriginalModelWithAResult() == nullptr);
    QVERIFY(model.recursiveOriginalModelWithAStep() == nullptr);
    QVERIFY(model.isVisible() == false);
    QVERIFY(model.prototype() != nullptr);
    QVERIFY(model.itemDrawable() == model.prototype());
    QVERIFY(model.result() == nullptr);
    QVERIFY(model.recursiveFinalize() == true);
    QVERIFY(model.parentGroup() == nullptr);
    QVERIFY(model.rootGroup() == &model);
}

void TestModels::testOutOpfGroupCreation()
{
    const QString displayableName("dn");
    const QString shortDescription("sd");
    const QString detailledDescription("dd");
    const quint8 opfLevel = 8;

    CT_OutOPFNodeGroupModel<OPFGroupForTest> model(displayableName,
                                                   shortDescription,
                                                   detailledDescription);

    QVERIFY(model.displayableName() == displayableName);
    QVERIFY(model.shortDescription() == shortDescription);
    QVERIFY(model.detailledDescription() == detailledDescription);
    QVERIFY(model.isEmpty() == true);
    QVERIFY(model.parentModel() == nullptr);
    QVERIFY(model.rootModel() == &model);
    QVERIFY(model.step() == nullptr);
    QVERIFY(model.uniqueIndex() == -1);
    QVERIFY(model.recursiveOriginalModel() == &model);
    QVERIFY(model.originalModel() == &model);
    QVERIFY(model.recursiveOriginalModelWithAResult() == nullptr);
    QVERIFY(model.recursiveOriginalModelWithAStep() == nullptr);
    QVERIFY(model.isVisible() == false);
    QVERIFY(model.prototype() != nullptr);
    QVERIFY(model.itemDrawable() == model.prototype());
    QVERIFY(model.result() == nullptr);
    QVERIFY(model.recursiveFinalize() == true);
    QVERIFY(model.parentGroup() == nullptr);
    QVERIFY(model.rootGroup() == &model);
    QVERIFY(model.opfLevel() == 0);
    model.setOPFLevel(opfLevel);
    QVERIFY(model.opfLevel() == opfLevel);
}

void TestModels::testOutStdItemAttributeCreation()
{

    const QString displayableName("dn");
    const QString shortDescription("sd");

    CT_OutStdItemAttributeModel<ItemAttributeForTest<0>> model(static_cast<ICategoryForModel*>(nullptr),
                                                               displayableName,
                                                               shortDescription);

    QVERIFY(model.displayableName() == displayableName);
    QVERIFY(model.shortDescription() == shortDescription);
    QVERIFY(model.detailledDescription() == model.shortDescription());
    QVERIFY(model.isEmpty() == true);
    QVERIFY(model.parentModel() == nullptr);
    QVERIFY(model.rootModel() == &model);
    QVERIFY(model.step() == nullptr);
    QVERIFY(model.uniqueIndex() == -1);
    QVERIFY(model.recursiveOriginalModel() == &model);
    QVERIFY(model.originalModel() == &model);
    QVERIFY(model.recursiveOriginalModelWithAResult() == nullptr);
    QVERIFY(model.recursiveOriginalModelWithAStep() == nullptr);
    QVERIFY(model.isVisible() == false);
    QVERIFY(model.prototype() != nullptr);
    QVERIFY(model.itemAttribute() == model.prototype());
    QVERIFY(model.result() == nullptr);
    QVERIFY(model.recursiveFinalize() == true);
    QVERIFY(model.isADefaultItemAttributeModel() == false);
}

void TestModels::testOutResultGroupCreation()
{
    const QString displayableName("dn");
    const QString shortDescription("sd");
    const QString detailledDescription("dd");

    CT_OutStdGroupModelForTest* rootGroup = new CT_OutStdGroupModelForTest(displayableName,
                                                                           shortDescription,
                                                                           detailledDescription);

    const QString resultName("rn");
    const QString resultDisplayableName("dn");
    const QString resultShortDescription("sd");

    CT_OutResultModelGroup<ResultGroupForTest> model(rootGroup,
                                                resultName,
                                                resultDisplayableName,
                                                resultShortDescription);

    QVERIFY(model.displayableName() == resultDisplayableName);
    QVERIFY(model.shortDescription() == resultShortDescription);
    QVERIFY(model.detailledDescription() == model.shortDescription());
    QVERIFY(model.isEmpty() == false);
    QVERIFY(model.parentModel() == nullptr);
    QVERIFY(model.rootModel() == &model);
    QVERIFY(model.step() == nullptr);
    QVERIFY(model.uniqueIndex() == -1);
    QVERIFY(model.recursiveOriginalModel() == &model);
    QVERIFY(model.originalModel() == &model);
    QVERIFY(model.recursiveOriginalModelWithAResult() == nullptr);
    QVERIFY(model.recursiveOriginalModelWithAStep() == nullptr);
    QVERIFY(model.isVisible() == false);
    QVERIFY(model.prototype() != nullptr);
    QVERIFY(dynamic_cast<ResultGroupForTest*>(model.prototype()) != nullptr);
    QVERIFY(model.result() == nullptr);
    QVERIFY(model.recursiveFinalize() == true);
    QVERIFY(model.resultName() == resultName);
    QVERIFY(model.rootGroup() == rootGroup);

    IResultForModel* r = model.createResult();

    QVERIFY(r != nullptr);

    QVERIFY(model.recursiveOriginalModelWithAResult() == &model);
    QVERIFY(rootGroup->parentModel() == &model);
    QVERIFY(rootGroup->rootModel() == &model);
    QVERIFY(rootGroup->recursiveOriginalModel() == rootGroup);
    QVERIFY(rootGroup->originalModel() == rootGroup);
    QVERIFY(rootGroup->recursiveOriginalModelWithAResult() == rootGroup);
    QVERIFY(rootGroup->recursiveOriginalModelWithAStep() == nullptr);

    delete r;
}

CT_OutAbstractResultModelGroup* createSimpleOutResultModel(const QString& resultName)
{
    const QString displayableName("dn");
    const QString shortDescription("sd");
    const QString detailledDescription("dd");

    auto* rootGroup = new TestModels::CT_OutStdGroupModelForTest(displayableName,
                                                                 shortDescription,
                                                                 detailledDescription);

    CT_OutStdSingularItemModel<ItemForTest>* item = new CT_OutStdSingularItemModel<ItemForTest>();

    rootGroup->addItem(item);

    const QString resultDisplayableName("dn");
    const QString resultShortDescription("sd");

    CT_OutResultModelGroup<ResultGroupForTest>* model = new CT_OutResultModelGroup<ResultGroupForTest>(rootGroup,
                                                                                             resultName,
                                                                                             resultDisplayableName,
                                                                                             resultShortDescription);
    model->setUniqueIndex(0);
    rootGroup->setUniqueIndex(1);
    item->setUniqueIndex(2);

    return model;
}


CT_InResultModelGroupToCopy* createSimpleInResultModel()
{
    const QString displayableName("dn");
    const QString shortDescription("sd");
    const bool recursive = true;

    CT_InResultModelGroupToCopy* inResultModel = new CT_InResultModelGroupToCopy(displayableName, shortDescription, recursive);
    CT_InZeroOrMoreGroupModel* rootGroup = inResultModel->setZeroOrMoreRootGroup(StdGroupForTest::staticType(), "name from type");
    inResultModel->addStdItemModel(rootGroup, ItemForTest::staticType(), "name from type", "dnCoucou");

    return inResultModel;
}

void TestModels::testOutResultGroupCopyCreation()
{
    const QString finalResultName("rn");

    StepForTest step1;
    CT_OutAbstractResultModelGroup* outStep1 = createSimpleOutResultModel(finalResultName);
    outStep1->setStep(&step1);

    IResultForModel* rStep1 = outStep1->createResult();
    QVERIFY(outStep1->result() == rStep1);

    QVERIFY(rStep1 != nullptr);
    QVERIFY(dynamic_cast<ResultGroupForTest*>(rStep1) != nullptr);
    QVERIFY(static_cast<ResultGroupForTest*>(rStep1)->name() == finalResultName);

    step1.setResult(rStep1);

    // ########## CONFIGURE STEP 2 INPUT MODEL ###########
    StepForTest step2;
    CT_InResultModelGroupToCopy* inStep2 = createSimpleInResultModel();
    inStep2->setStep(&step2);

    QVERIFY(inStep2->recursiveFindAllPossibilitiesInModel(*outStep1) == true);
    QVERIFY(inStep2->nPossibilitySaved() == 1);

    const auto visitorP = [](const CT_InAbstractModel*, const CT_InStdModelPossibility* possibility) -> bool {
        return const_cast<CT_InStdModelPossibility*>(possibility)->setSelected(true);
    };

    // check all possitibilies
    QVERIFY(inStep2->recursiveVisitPossibilities(visitorP) == true);
    QVERIFY(inStep2->nPossibilitySelected() == 1);

    const auto visitorC = [](const CT_InAbstractModel* model) -> bool {
        return (model->nPossibilitySelected() == 1);
    };

    // check that all possibilities is checked and that all model has only one possibility !
    QVERIFY(inStep2->recursiveVisitInChildrensOrInModelOfPossibilities(visitorC) == true);

    // ########## CHECK STEP 2 CREATE OUTPUT RESULT ###########
    const QString resultModelDisplayableName("dn");
    const QString resultModelShortDescription("sd");

    CT_OutResultModelGroupCopy outStep2(inStep2, finalResultName, resultModelDisplayableName, resultModelShortDescription);
    outStep2.setStep(&step2);

    IResultForModel* rStep2 = outStep2.createResult();
    QVERIFY(outStep2.result() == rStep2);

    QVERIFY(rStep2 != nullptr);
    QVERIFY(static_cast<ResultGroupForTest*>(rStep2)->name() == finalResultName);

    step2.setResult(rStep2);

    // ########## CONFIGURE STEP 3 INPUT MODEL ###########
    StepForTest step3;
    CT_InResultModelGroupToCopy* inStep3 = createSimpleInResultModel();
    inStep3->setStep(&step3);

    QVERIFY(inStep3->recursiveFindAllPossibilitiesInModel(outStep2) == true);
    QVERIFY(inStep3->nPossibilitySaved() == 1);

    // check all possitibilies
    QVERIFY(inStep3->recursiveVisitPossibilities(visitorP) == true);
    QVERIFY(inStep3->nPossibilitySelected() == 1);

    // check that all possibilities is checked and that all model has only one possibility !
    QVERIFY(inStep3->recursiveVisitInChildrensOrInModelOfPossibilities(visitorC) == true);

    // ########## CHECK STEP 3 CREATE OUTPUT RESULT ###########

    CT_OutResultModelGroupCopy outStep3(inStep3, finalResultName, resultModelDisplayableName, resultModelShortDescription);
    outStep3.setStep(&step3);

    IResultForModel* rStep3 = outStep3.createResult();
    QVERIFY(outStep3.result() == rStep3);

    QVERIFY(rStep3 != nullptr);
    QVERIFY(static_cast<ResultGroupForTest*>(rStep3)->name() == finalResultName);

    step3.setResult(rStep3);

    delete outStep1;
    delete inStep2;
    delete inStep3;
}

void TestModels::testResearchInZeroOrMoreWithMultipleItemAtDifferentLevel()
{
    // input model :
    // *
    // |--- inputItem = ItemForTest

    CT_InZeroOrMoreGroupModel inRootGroupModel(StdGroupForTest::staticType(), "name from type");
        CT_InStdSingularItemModel* inItemModel = new CT_InStdSingularItemModel(ItemForTest::staticType(), "name from type");

    inRootGroupModel.addItem(inItemModel);

    // output model :
    // root
    //   |--- group1 (possibility for inRootGroupModel)
    //   |      |--- ItemForTest (possibility for inItemModel)
    //   |
    //   |--- group2
    //          |--- group3 (possibility for inRootGroupModel)
    //                  |--- ItemForTest (possibility for inItemModel)

    CT_OutStdGroupModelForTest outRootGroupModel;

        CT_OutStdGroupModelForTest* outGroupModel1 = new CT_OutStdGroupModelForTest();
            auto outItemModel1 = new CT_OutStdSingularItemModel<ItemForTest>();
            outGroupModel1->addItem(outItemModel1);

        CT_OutStdGroupModelForTest* outGroupModel2 = new CT_OutStdGroupModelForTest();
            CT_OutStdGroupModelForTest* outGroupModel3 = new CT_OutStdGroupModelForTest();
                auto outItemModel2 = new CT_OutStdSingularItemModel<ItemForTest>();
                outGroupModel3->addItem(outItemModel2);

        outGroupModel2->addGroup(outGroupModel3);

    outRootGroupModel.addGroup(outGroupModel1);
    outRootGroupModel.addGroup(outGroupModel2);

    QVERIFY(inRootGroupModel.recursiveFindAllPossibilitiesInModel(outRootGroupModel) == true);
    QCOMPARE(inRootGroupModel.nPossibilitySaved(), 2);
    QCOMPARE(inItemModel->nPossibilitySaved(), 2);
    QVERIFY(inRootGroupModel.getPossibilitiesSaved().at(0)->outModel() == outGroupModel1);
    QVERIFY(inRootGroupModel.getPossibilitiesSaved().at(1)->outModel() == outGroupModel3);
    QVERIFY(inItemModel->getPossibilitiesSaved().at(0)->outModel() == outItemModel1);
    QVERIFY(inItemModel->getPossibilitiesSaved().at(1)->outModel() == outItemModel2);
}

void recursivePrintOutModel(CT_OutAbstractModel* model) {
     qDebug() << model->displayableName();

    model->recursiveVisitOutChildrens([](const CT_OutAbstractModel* child) -> bool {
        qDebug() << child->displayableName();
        return true;
    });
}

void recursivePrintInModel(CT_InAbstractModel* model) {
    qDebug() << "IN : " << model->displayableName();

    if(model->nPossibilitySaved() == 0) {
        qDebug() << "NO POSS";
    } else {
        for(const CT_InStdModelPossibility* p : model->possibilitiesGroup()->savedPossibilities()) {
            qDebug() << "POSS : " << p->outModel()->displayableName();
        }
    }

    model->recursiveVisitInChildrens([](const CT_InAbstractModel* child) -> bool {
        qDebug() << "IN : " << child->displayableName();

        if(child->nPossibilitySaved() == 0) {
            qDebug() << "NO POSS";
        } else {
            for(const CT_InStdModelPossibility* p : child->possibilitiesGroup()->savedPossibilities()) {
                qDebug() << "POSS : " << p->outModel()->displayableName();
            }
        }

        return true;
    });
}

void TestModels::testResearchInZeroOrMoreWithMultipleItemAtDifferentLevelBis()
{
    // input model :
    // *
    // |--- Group
    //         |--- inputItem = ItemForTest

    CT_InZeroOrMoreGroupModel inRootGroupModel(StdGroupForTest::staticType(), "name from type");
        CT_InStdGroupModel* inGroup1 = new CT_InStdGroupModel(StdGroupForTest::staticType(), "name from type");
            CT_InStdSingularItemModel* inItemModel = new CT_InStdSingularItemModel(ItemForTest::staticType(), "name from type", "item");
        inGroup1->addItem(inItemModel);

    inRootGroupModel.addGroup(inGroup1);

    // output model :
    // root
    //   |--- ItemForTest (possibility for inItemModel)
    //   |--- ItemForTest (possibility for inItemModel)

    CT_OutStdGroupModelForTest outRootGroupModel("root group");
        auto outItemModel1 = new CT_OutStdSingularItemModel<ItemForTest>("item 1");
        auto outItemModel2 = new CT_OutStdSingularItemModel<ItemForTest>("item 2");

    outRootGroupModel.addItem(outItemModel1);
    outRootGroupModel.addItem(outItemModel2);

    QVERIFY(inRootGroupModel.recursiveFindAllPossibilitiesInModel(outRootGroupModel) == true);
    QCOMPARE(inRootGroupModel.nPossibilitySaved(), 0);
    QCOMPARE(inGroup1->nPossibilitySaved(), 1);
    QCOMPARE(inItemModel->nPossibilitySaved(), 2);
    QVERIFY(inGroup1->getPossibilitiesSaved().at(0)->outModel() == &outRootGroupModel);
    QVERIFY(inItemModel->getPossibilitiesSaved().at(0)->outModel() == outItemModel1);
    QVERIFY(inItemModel->getPossibilitiesSaved().at(1)->outModel() == outItemModel2);

    //recursivePrintOutModel(&outRootGroupModel);
    //recursivePrintInModel(&inRootGroupModel);
}

void TestModels::testResearchInZeroOrMoreWithMultipleItemAtDifferentLevelAndOfDifferentType()
{
    // input model :
    // *
    // |--- inputItem = ItemForTest
    // |--- inputItem = ItemForTest2

    CT_InZeroOrMoreGroupModel inRootGroupModel(StdGroupForTest::staticType(), "name from type");
        CT_InStdSingularItemModel* inItemModel1 = new CT_InStdSingularItemModel(ItemForTest::staticType(), "name from type");
        CT_InStdSingularItemModel* inItemModel2 = new CT_InStdSingularItemModel(ItemForTest2::staticType(), "name from type");

    inRootGroupModel.addItem(inItemModel1);
    inRootGroupModel.addItem(inItemModel2);

    // output model :
    // root
    //   |--- group1 (possibility for inRootGroupModel)
    //   |      |--- outItemModel1 = ItemForTest (possibility for inItemModel1)
    //   |      |--- group2 (possibility for *)
    //   |              |--- outItemModel2 = ItemForTest2 (possibility for inItemModel2)
    //   |
    //   |--- group3
    //          |--- group4 (possibility for inRootGroupModel)
    //                  |--- outItemModel3 = ItemForTest2 (possibility for inItemModel2)

    CT_OutStdGroupModelForTest outRootGroupModel;

        CT_OutStdGroupModelForTest* outGroupModel1 = new CT_OutStdGroupModelForTest();
            auto outItemModel1 = new CT_OutStdSingularItemModel<ItemForTest>();
            outGroupModel1->addItem(outItemModel1);

            CT_OutStdGroupModelForTest* outGroupModel2 = new CT_OutStdGroupModelForTest();
            auto outItemModel2 = new CT_OutStdSingularItemModel<ItemForTest2>();
            outGroupModel2->addItem(outItemModel2);

        outGroupModel1->addGroup(outGroupModel2);

        CT_OutStdGroupModelForTest* outGroupModel3 = new CT_OutStdGroupModelForTest();
            CT_OutStdGroupModelForTest* outGroupModel4 = new CT_OutStdGroupModelForTest();
                auto outItemModel3 = new CT_OutStdSingularItemModel<ItemForTest2>();
                outGroupModel4->addItem(outItemModel3);

        outGroupModel3->addGroup(outGroupModel4);

    outRootGroupModel.addGroup(outGroupModel1);
    outRootGroupModel.addGroup(outGroupModel3);

    QVERIFY(inRootGroupModel.recursiveFindAllPossibilitiesInModel(outRootGroupModel) == true);

    QCOMPARE(inRootGroupModel.nPossibilitySaved(), 3);
    QCOMPARE(inItemModel1->nPossibilitySaved(), 1);
    QCOMPARE(inItemModel2->nPossibilitySaved(), 2);
    QVERIFY(inRootGroupModel.getPossibilitiesSaved().at(0)->outModel() == outGroupModel2);
    QVERIFY(inRootGroupModel.getPossibilitiesSaved().at(1)->outModel() == outGroupModel1);
    QVERIFY(inRootGroupModel.getPossibilitiesSaved().at(2)->outModel() == outGroupModel4);
    QVERIFY(inItemModel1->getPossibilitiesSaved().at(0)->outModel() == outItemModel1);
    QVERIFY(inItemModel2->getPossibilitiesSaved().at(0)->outModel() == outItemModel2);
    QVERIFY(inItemModel2->getPossibilitiesSaved().at(1)->outModel() == outItemModel3);
}

void TestModels::testResearchInZeroOrMoreWithOnGroupWithMultipleItemAtDifferentLevelAndOfDifferentType()
{
    // input model :
    // *
    // |--- Group
    //        |--- inputItem = ItemForTest
    //        |--- inputItem = ItemForTest2

    CT_InZeroOrMoreGroupModel inRootGroupModel(StdGroupForTest::staticType(), "name from type");
        CT_InStdGroupModel* inGroup1 = new CT_InStdGroupModel(StdGroupForTest::staticType(), "name from type");
            CT_InStdSingularItemModel* inItemModel1 = new CT_InStdSingularItemModel(ItemForTest::staticType(), "name from type");
            CT_InStdSingularItemModel* inItemModel2 = new CT_InStdSingularItemModel(ItemForTest2::staticType(), "name from type");
        inGroup1->addItem(inItemModel1);
        inGroup1->addItem(inItemModel2);

    inRootGroupModel.addGroup(inGroup1);

    // output model :
    // root (possibility for inRootGroupModel)
    //   |--- group1 (possibility for inGroup1)
    //   |      |--- outItemModel1 = ItemForTest (possibility for inItemModel1)
    //   |      |--- outItemModel2 = ItemForTest2 (possibility for inItemModel2)
    //   |
    //   |--- group2
    //          |--- group3 (possibility for inGroup1)
    //                  |--- outItemModel3 = ItemForTest2

    CT_OutStdGroupModelForTest outRootGroupModel;

        CT_OutStdGroupModelForTest* outGroupModel1 = new CT_OutStdGroupModelForTest();
            auto outItemModel1 = new CT_OutStdSingularItemModel<ItemForTest>();
            auto outItemModel2 = new CT_OutStdSingularItemModel<ItemForTest2>();

        outGroupModel1->addItem(outItemModel1);
        outGroupModel1->addItem(outItemModel2);

        CT_OutStdGroupModelForTest* outGroupModel2 = new CT_OutStdGroupModelForTest();
            CT_OutStdGroupModelForTest* outGroupModel3 = new CT_OutStdGroupModelForTest();
                auto outItemModel3 = new CT_OutStdSingularItemModel<ItemForTest2>();
            outGroupModel3->addItem(outItemModel3);

        outGroupModel2->addGroup(outGroupModel3);

    outRootGroupModel.addGroup(outGroupModel1);
    outRootGroupModel.addGroup(outGroupModel2);

    QVERIFY(inRootGroupModel.recursiveFindAllPossibilitiesInModel(outRootGroupModel) == true);

    QCOMPARE(inRootGroupModel.nPossibilitySaved(), 1);
    QCOMPARE(inGroup1->nPossibilitySaved(), 1);
    QCOMPARE(inItemModel1->nPossibilitySaved(), 1);
    QCOMPARE(inItemModel2->nPossibilitySaved(), 1);
    QVERIFY(inRootGroupModel.getPossibilitiesSaved().at(0)->outModel() == &outRootGroupModel);
    QVERIFY(inGroup1->getPossibilitiesSaved().at(0)->outModel() == outGroupModel1);
    QVERIFY(inItemModel1->getPossibilitiesSaved().at(0)->outModel() == outItemModel1);
    QVERIFY(inItemModel2->getPossibilitiesSaved().at(0)->outModel() == outItemModel2);
}

void TestModels::testNotCompatibleInZeroOrMoreWithOnGroupWithMultipleItemAtDifferentLevelAndOfDifferentType()
{
    // input model :
    // *
    // |--- Group
    //        |--- inputItem = ItemForTest
    //        |--- inputItem = ItemForTest2

    CT_InZeroOrMoreGroupModel inRootGroupModel(StdGroupForTest::staticType(), "name from type");
        CT_InStdGroupModel* inGroup1 = new CT_InStdGroupModel(StdGroupForTest::staticType(), "name from type");
            CT_InStdSingularItemModel* inItemModel1 = new CT_InStdSingularItemModel(ItemForTest::staticType(), "name from type");
            CT_InStdSingularItemModel* inItemModel2 = new CT_InStdSingularItemModel(ItemForTest2::staticType(), "name from type");
        inGroup1->addItem(inItemModel1);
        inGroup1->addItem(inItemModel2);

    inRootGroupModel.addGroup(inGroup1);

    // output model :
    // root
    //   |--- group1
    //   |      |--- outItemModel1 = ItemForTest
    //   |      |--- outItemModel2 = ItemForTest3
    //   |
    //   |--- group2
    //          |--- group3
    //                  |--- outItemModel3 = ItemForTest3

    CT_OutStdGroupModelForTest outRootGroupModel;

        CT_OutStdGroupModelForTest* outGroupModel1 = new CT_OutStdGroupModelForTest();
            auto outItemModel1 = new CT_OutStdSingularItemModel<ItemForTest>();
            auto outItemModel2 = new CT_OutStdSingularItemModel<ItemForTest3>();

        outGroupModel1->addItem(outItemModel1);
        outGroupModel1->addItem(outItemModel2);

        CT_OutStdGroupModelForTest* outGroupModel2 = new CT_OutStdGroupModelForTest();
            CT_OutStdGroupModelForTest* outGroupModel3 = new CT_OutStdGroupModelForTest();
                auto outItemModel3 = new CT_OutStdSingularItemModel<ItemForTest3>();
            outGroupModel3->addItem(outItemModel3);

        outGroupModel2->addGroup(outGroupModel3);

    outRootGroupModel.addGroup(outGroupModel1);
    outRootGroupModel.addGroup(outGroupModel2);

    QVERIFY(inRootGroupModel.recursiveFindAllPossibilitiesInModel(outRootGroupModel) == false);

    QCOMPARE(inRootGroupModel.nPossibilitySaved(), 0);
    QCOMPARE(inGroup1->nPossibilitySaved(), 0);
    QCOMPARE(inItemModel1->nPossibilitySaved(), 0);
    QCOMPARE(inItemModel2->nPossibilitySaved(), 0);
}

void TestModels::testInHandleStructureStaticCheck()
{
    using ValueType = int;

    constexpr ValueType BOOLEAN = 0;

    CT_InModelStructureManager                              inManager;
    CT_HandleInResult<ResultGroupForTest>                   handleResult;
    CT_HandleInZeroOrMoreGroup<StdGroupForTest>             handleRootGroup;
    CT_HandleInGroup<StdGroupForTest>                       handleGroup;
    CT_HandleInItem<ItemForTest2>                           handleItem;
    CT_HandleInItem<ItemForTest2>                           handleItem2;
    CT_HandleInItemAttribute<AbstractItemAttributeForTest, BOOLEAN> handleItemAttribute;

    CT_HandleInResult<ResultGroupForTest>                   handleResult2;
    CT_HandleInResultCopy<ResultGroupForTest>               handleResultCopy;

    inManager.addResult(handleResult, "coucou", "sd", false);
    inManager.setZeroOrMoreRootGroup(handleResult, handleRootGroup);
    inManager.addGroup(handleRootGroup, handleGroup, "group", "sd", "dd");
    inManager.addItem(handleRootGroup, handleItem, "itemInRootGroup", "sd", "dd");
    inManager.addItem(handleGroup, handleItem2, "itemInGroup", "sd", "dd");
    inManager.addItemAttribute(handleItem2, handleItemAttribute, QStringList{"cat"}, "itemInGroup", "sd", "dd");

    inManager.addResult(handleResult2, "coucou", "sd", false);
    CT_HandleInZeroOrMoreGroup<StdGroupForTest>             handleRootGroup2;
    inManager.setZeroOrMoreRootGroup(handleResult2, handleRootGroup2);

    inManager.addResult(handleResultCopy, "coucou", "sd", false);
    CT_HandleInZeroOrMoreGroup<StdGroupForTest>             handleRootGroup3;
    inManager.setZeroOrMoreRootGroup(handleResultCopy, handleRootGroup3);

    QVERIFY(handleResult.model() != nullptr);
    QVERIFY(handleRootGroup.model() != nullptr);
    QVERIFY(handleGroup.model() != nullptr);
    QVERIFY(handleItem.model() != nullptr);
    QVERIFY(handleItem2.model() != nullptr);
    QVERIFY(handleItemAttribute.model() != nullptr);
    QVERIFY(handleResult2.model() != nullptr);
    QVERIFY(handleRootGroup2.model() != nullptr);
    QVERIFY(handleResultCopy.model() != nullptr);
    QVERIFY(handleRootGroup3.model() != nullptr);
}

void TestModels::testOutHandleStructureStaticCheck()
{
    CT_OutModelStructureManager                         outManager;
    CT_HandleOutResult<ResultGroupForTest>              handleResult;
    CT_HandleOutGroup<StdGroupForTest>                  handleRootGroup;
    CT_HandleOutGroup<StdGroupForTest>                  handleGroup;
    CT_HandleOutItem<ItemForTest>                       handleItem;
    CT_HandleOutItem<ItemForTest2>                      handleItem2;
    CT_HandleOutItemAttribute<ItemAttributeForTest<0>>  handleItemAttribute;

    CT_HandleInResultCopy<ResultGroupForTest>           handleResultCopy1;
    CT_HandleInResultCopy<ResultGroupForTest>           handleResultCopy2;

    CategoryForTest category;

    bool outManagerCantAddAInResultCopyInAddResultMethod = false;
    bool outManagerCantAddAInResultCopyInAddResultCopyMethod = false;

    outManager.addResult(handleResult, "rn", "coucou", "sd");
    outManager.setRootGroup(handleResult, handleRootGroup, "dn", "sd", "dd");
    outManager.addGroup(handleRootGroup, handleGroup, "group", "sd", "dd");
    outManager.addItem(handleRootGroup, handleItem, "itemInRootGroup", "sd", "dd");
    outManager.addItem(handleGroup, handleItem2, "itemInGroup", "sd", "dd");
    outManager.addItemAttribute(handleItem2, handleItemAttribute, &category, "itemInGroup", "sd", "dd");

    try{
        outManager.addResult(handleResultCopy1, "coucou", "sd");
    } catch(TestException) {
        qDebug() << "Exception catched";
        outManagerCantAddAInResultCopyInAddResultMethod = true;
    }

    try{
        outManager.addResultCopy(handleResultCopy2);
    } catch(TestException) {
        qDebug() << "Exception catched";
        outManagerCantAddAInResultCopyInAddResultCopyMethod = true;
    }

    QVERIFY(outManagerCantAddAInResultCopyInAddResultMethod == true);
    QVERIFY(outManagerCantAddAInResultCopyInAddResultCopyMethod == true);
    QVERIFY(handleResult.isValid() == true);
    QVERIFY(handleRootGroup.isValid() == true);
    QVERIFY(handleGroup.isValid() == true);
    QVERIFY(handleItem.isValid() == true);
    QVERIFY(handleItem2.isValid() == true);
    QVERIFY(handleItemAttribute.isValid() == true);
    QVERIFY(handleItemAttribute.firstModel()->itemAttribute()->itemAttributeToolForModel()->category() == &category);
    QVERIFY(handleResultCopy1.isValid() == false);
    QVERIFY(handleResultCopy2.isValid() == false);
}

void TestModels::testInHandleStructureCreation()
{
    // input :
    // handleResult
    //   |
    //   |---handleRootGroup
    //          |--- handleGroup1
    //          |       |--- handleItem2
    //          |
    //          |--- handleGroup2
    //                  |--- handleItem1
    //                          |--- handleItem1Attribute

    const QString resultDisplayableName("rdn");
    const QString resultShortDescription("rsd");
    const bool resultRecursive = false;

    const QString displayableName("dn");
    const QString shortDescription("sd");
    const QString detailledDescription("dd");
    constexpr int valueType = 5;
    const QStringList categories{"0", "1"};

    CT_InModelStructureManager inManager;

    CT_HandleInResult<ResultGroupForTest>               handleResult;
    CT_HandleInGroup<StdGroupForTest>                       handleGroup1;
    CT_HandleInItem<ItemForTest2>                               handleItem2;
    CT_HandleInGroup<StdGroupForTest>                       handleGroup2;
    CT_HandleInItem<ItemForTest>                                handleItem1;
    CT_HandleInItemAttribute<AbstractItemAttributeForTest, valueType>       handleItem1Attribute;

    QVERIFY(handleResult.model() == nullptr);
    QVERIFY(handleGroup1.model() == nullptr);
    QVERIFY(handleGroup2.model() == nullptr);
    QVERIFY(handleItem1.model() == nullptr);
    QVERIFY(handleItem2.model() == nullptr);
    QVERIFY(handleItem1Attribute.model() == nullptr);

    inManager.addResult(handleResult, resultDisplayableName, resultShortDescription, resultRecursive);

    QVERIFY(handleResult.model() != nullptr);
    QVERIFY(handleResult.model()->displayableName() == resultDisplayableName);
    QVERIFY(handleResult.model()->shortDescription() == resultShortDescription);
    QVERIFY(handleResult.model()->isRecursive() == resultRecursive);

    CT_HandleInZeroOrMoreGroup<StdGroupForTest>             handleRootGroup;
    inManager.setZeroOrMoreRootGroup(handleResult, handleRootGroup);

    QVERIFY(handleRootGroup.model() != nullptr);
    QVERIFY(handleRootGroup.model()->parentModel() == handleResult.model());

    inManager.addGroup(handleRootGroup,
                       handleGroup1,
                       displayableName,
                       shortDescription,
                       detailledDescription);

    QVERIFY(handleGroup1.model() != nullptr);
    QVERIFY(handleGroup1.model()->parentModel() == handleRootGroup.model());
    QVERIFY(handleGroup1.model()->displayableName() == displayableName);
    QVERIFY(handleGroup1.model()->shortDescription() == shortDescription);
    QVERIFY(handleGroup1.model()->detailledDescription() == detailledDescription);

    inManager.addGroup(handleRootGroup,
                       handleGroup2,
                       displayableName,
                       shortDescription,
                       detailledDescription);

    QVERIFY(handleGroup2.model() != nullptr);
    QVERIFY(handleGroup1.model()->parentModel() == handleRootGroup.model());
    QVERIFY(handleGroup2.model()->displayableName() == displayableName);
    QVERIFY(handleGroup2.model()->shortDescription() == shortDescription);
    QVERIFY(handleGroup2.model()->detailledDescription() == detailledDescription);

    inManager.addItem(handleGroup2,
                      handleItem1,
                      displayableName,
                      shortDescription,
                      detailledDescription);

    QVERIFY(handleItem1.model() != nullptr);
    QVERIFY(handleItem1.model()->parentModel() == handleGroup2.model());
    QVERIFY(handleItem1.model()->displayableName() == displayableName);
    QVERIFY(handleItem1.model()->shortDescription() == shortDescription);
    QVERIFY(handleItem1.model()->detailledDescription() == detailledDescription);

    inManager.addItem(handleGroup1,
                      handleItem2,
                      displayableName,
                      shortDescription,
                      detailledDescription);

    QVERIFY(handleItem2.model() != nullptr);
    QVERIFY(handleItem2.model()->parentModel() == handleGroup1.model());
    QVERIFY(handleItem2.model()->displayableName() == displayableName);
    QVERIFY(handleItem2.model()->shortDescription() == shortDescription);
    QVERIFY(handleItem2.model()->detailledDescription() == detailledDescription);

    inManager.addItemAttribute(handleItem1,
                               handleItem1Attribute,
                               categories,
                               displayableName,
                               shortDescription,
                               detailledDescription);

    QVERIFY(handleItem1Attribute.model() != nullptr);
    QVERIFY(handleItem1Attribute.model()->parentModel() == handleItem1.model());
    QVERIFY(handleItem1Attribute.model()->displayableName() == displayableName);
    QVERIFY(handleItem1Attribute.model()->shortDescription() == shortDescription);
    QVERIFY(handleItem1Attribute.model()->detailledDescription() == detailledDescription);
    QVERIFY(handleItem1Attribute.model()->valueType() == valueType);
    QVERIFY(handleItem1Attribute.model()->categoriesType() == categories);
}

void TestModels::testInHandleStructureCreationWithAbstractHandle()
{
    // input :
    // handleResult
    //   |
    //   |---handleRootGroup
    //          |--- handleGroup1
    //          |       |--- handleItem2
    //          |
    //          |--- handleGroup2
    //                  |--- handleItem1
    //                          |--- handleItem1Attribute

    const QString resultDisplayableName("rdn");
    const QString resultShortDescription("rsd");
    const bool resultRecursive = false;

    const QString displayableName("dn");
    const QString shortDescription("sd");
    const QString detailledDescription("dd");
    constexpr int valueType = 5;
    const QStringList categories{"0", "1"};

    CT_InModelStructureManager inManager;

    CT_HandleInResult<ResultGroupForTest>               handleResult;
    CT_HandleInGroup<StdGroupForTest>                       handleGroup1;
    CT_HandleInItem<ItemForTest2>                               handleItem2;
    CT_HandleInGroup<StdGroupForTest>                       handleGroup2;
    CT_HandleInItem<ItemForTest>                                handleItem1;
    CT_HandleInItemAttribute<AbstractItemAttributeForTest, valueType>       handleItem1Attribute;

    CT_AbstractHandleInGroup*           abstractHandleGroup = &handleGroup1;
    CT_AbstractHandleInItem*            abstractHandleItem1 = &handleItem1;

    QVERIFY(handleResult.model() == nullptr);
    QVERIFY(handleGroup1.model() == nullptr);
    QVERIFY(handleGroup2.model() == nullptr);
    QVERIFY(handleItem1.model() == nullptr);
    QVERIFY(handleItem2.model() == nullptr);
    QVERIFY(handleItem1Attribute.model() == nullptr);

    inManager.addResult(handleResult, resultDisplayableName, resultShortDescription, resultRecursive);

    QVERIFY(handleResult.model() != nullptr);
    QVERIFY(handleResult.model()->displayableName() == resultDisplayableName);
    QVERIFY(handleResult.model()->shortDescription() == resultShortDescription);
    QVERIFY(handleResult.model()->isRecursive() == resultRecursive);

    CT_HandleInZeroOrMoreGroup<StdGroupForTest>             handleRootGroup;
    inManager.setZeroOrMoreRootGroup(handleResult, handleRootGroup);

    QVERIFY(handleRootGroup.model() != nullptr);
    QVERIFY(handleRootGroup.model()->parentModel() == handleResult.model());

    inManager.addGroup(handleRootGroup,
                       handleGroup1,
                       displayableName,
                       shortDescription,
                       detailledDescription);

    QVERIFY(handleGroup1.model() != nullptr);
    QVERIFY(handleGroup1.model()->parentModel() == handleRootGroup.model());
    QVERIFY(handleGroup1.model()->displayableName() == displayableName);
    QVERIFY(handleGroup1.model()->shortDescription() == shortDescription);
    QVERIFY(handleGroup1.model()->detailledDescription() == detailledDescription);

    inManager.addGroup(handleRootGroup,
                       handleGroup2,
                       displayableName,
                       shortDescription,
                       detailledDescription);

    QVERIFY(handleGroup2.model() != nullptr);
    QVERIFY(handleGroup1.model()->parentModel() == handleRootGroup.model());
    QVERIFY(handleGroup2.model()->displayableName() == displayableName);
    QVERIFY(handleGroup2.model()->shortDescription() == shortDescription);
    QVERIFY(handleGroup2.model()->detailledDescription() == detailledDescription);

    inManager.addItem(handleGroup2,
                      handleItem1,
                      displayableName,
                      shortDescription,
                      detailledDescription);

    QVERIFY(handleItem1.model() != nullptr);
    QVERIFY(handleItem1.model()->parentModel() == handleGroup2.model());
    QVERIFY(handleItem1.model()->displayableName() == displayableName);
    QVERIFY(handleItem1.model()->shortDescription() == shortDescription);
    QVERIFY(handleItem1.model()->detailledDescription() == detailledDescription);

    inManager.addItem(*abstractHandleGroup,
                      handleItem2,
                      displayableName,
                      shortDescription,
                      detailledDescription);

    QVERIFY(handleItem2.model() != nullptr);
    QVERIFY(handleItem2.model()->parentModel() == handleGroup1.model());
    QVERIFY(handleItem2.model()->displayableName() == displayableName);
    QVERIFY(handleItem2.model()->shortDescription() == shortDescription);
    QVERIFY(handleItem2.model()->detailledDescription() == detailledDescription);

    inManager.addItemAttribute(*abstractHandleItem1,
                               handleItem1Attribute,
                               categories,
                               displayableName,
                               shortDescription,
                               detailledDescription);

    QVERIFY(handleItem1Attribute.model() != nullptr);
    QVERIFY(handleItem1Attribute.model()->parentModel() == handleItem1.model());
    QVERIFY(handleItem1Attribute.model()->displayableName() == displayableName);
    QVERIFY(handleItem1Attribute.model()->shortDescription() == shortDescription);
    QVERIFY(handleItem1Attribute.model()->detailledDescription() == detailledDescription);
    QVERIFY(handleItem1Attribute.model()->valueType() == valueType);
    QVERIFY(handleItem1Attribute.model()->categoriesType() == categories);
}

void TestModels::testOutHandleStructureCreation()
{
    // output :
    // handleResult
    //   |
    //   |---handleRootGroup
    //          |--- handleGroup1
    //          |       |--- handleItem2
    //          |
    //          |--- handleGroup2
    //                  |--- handleItem1
    //                          |--- handleItem1Attribute

    const QString resultName("rn");
    const QString resultDisplayableName("rdn");
    const QString resultShortDescription("rsd");

    const QString displayableName("dn");
    const QString shortDescription("sd");
    const QString detailledDescription("dd");
    constexpr int valueType = 5;
    const CategoryForTest category;

    UniqueIndexGeneratorForTest indexGen;
    CT_OutModelStructureManager outManager;

    CT_HandleOutResult<ResultGroupForTest>               handleResult;
    CT_HandleOutGroup<StdGroupForTest>                       handleRootGroup;
    CT_HandleOutGroup<StdGroupForTest>                          handleGroup1;
    CT_HandleOutItem<ItemForTest2>                                  handleItem2;
    CT_HandleOutGroup<StdGroupForTest>                          handleGroup2;
    CT_HandleOutItem<ItemForTest>                                   handleItem1;
    CT_HandleOutItemAttribute<ItemAttributeForTest<valueType>>          handleItem1Attribute;

    QVERIFY(handleResult.model() == nullptr);
    QVERIFY(handleRootGroup.nModels() == 0);
    QVERIFY(handleGroup1.nModels() == 0);
    QVERIFY(handleItem2.nModels() == 0);
    QVERIFY(handleGroup2.nModels() == 0);
    QVERIFY(handleItem1.nModels() == 0);
    QVERIFY(handleItem1Attribute.nModels() == 0);

    outManager.addResult(handleResult, resultName, resultDisplayableName, resultShortDescription);

    QVERIFY(handleResult.model() != nullptr);
    QVERIFY(handleResult.model()->resultName() == resultName);
    QVERIFY(handleResult.model()->displayableName() == resultDisplayableName);
    QVERIFY(handleResult.model()->shortDescription() == resultShortDescription);
    QVERIFY(handleResult.model()->prototype() != nullptr);

    outManager.setRootGroup(handleResult,
                            handleRootGroup,
                            displayableName,
                            shortDescription,
                            detailledDescription);

    QVERIFY(handleRootGroup.firstModel() != nullptr);
    QVERIFY(handleRootGroup.firstModel()->parentModel() == handleResult.model());
    QVERIFY(handleRootGroup.firstModel()->displayableName() == displayableName);
    QVERIFY(handleRootGroup.firstModel()->shortDescription() == shortDescription);
    QVERIFY(handleRootGroup.firstModel()->detailledDescription() == detailledDescription);
    QVERIFY(handleRootGroup.firstModel()->prototype() != nullptr);

    outManager.addGroup(handleRootGroup,
                        handleGroup1,
                        displayableName,
                        shortDescription,
                        detailledDescription);

    QVERIFY(handleGroup1.firstModel() != nullptr);
    QVERIFY(handleGroup1.firstModel()->parentModel() == handleRootGroup.firstModel());
    QVERIFY(handleGroup1.firstModel()->displayableName() == displayableName);
    QVERIFY(handleGroup1.firstModel()->shortDescription() == shortDescription);
    QVERIFY(handleGroup1.firstModel()->detailledDescription() == detailledDescription);
    QVERIFY(handleGroup1.firstModel()->prototype() != nullptr);

    outManager.addGroup(handleRootGroup,
                        handleGroup2,
                        displayableName,
                        shortDescription,
                        detailledDescription);

    QVERIFY(handleGroup2.firstModel() != nullptr);
    QVERIFY(handleGroup1.firstModel()->parentModel() == handleRootGroup.firstModel());
    QVERIFY(handleGroup2.firstModel()->displayableName() == displayableName);
    QVERIFY(handleGroup2.firstModel()->shortDescription() == shortDescription);
    QVERIFY(handleGroup2.firstModel()->detailledDescription() == detailledDescription);
    QVERIFY(handleGroup2.firstModel()->prototype() != nullptr);

    outManager.addItem(handleGroup2,
                       handleItem1,
                       displayableName,
                       shortDescription,
                       detailledDescription);

    QVERIFY(handleItem1.firstModel() != nullptr);
    QVERIFY(handleItem1.firstModel()->parentModel() == handleGroup2.firstModel());
    QVERIFY(handleItem1.firstModel()->displayableName() == displayableName);
    QVERIFY(handleItem1.firstModel()->shortDescription() == shortDescription);
    QVERIFY(handleItem1.firstModel()->detailledDescription() == detailledDescription);
    QVERIFY(handleItem1.firstModel()->prototype() != nullptr);

    outManager.addItem(handleGroup1,
                       handleItem2,
                       displayableName,
                       shortDescription,
                       detailledDescription);

    QVERIFY(handleItem2.firstModel() != nullptr);
    QVERIFY(handleItem2.firstModel()->parentModel() == handleGroup1.firstModel());
    QVERIFY(handleItem2.firstModel()->displayableName() == displayableName);
    QVERIFY(handleItem2.firstModel()->shortDescription() == shortDescription);
    QVERIFY(handleItem2.firstModel()->detailledDescription() == detailledDescription);
    QVERIFY(handleItem2.firstModel()->prototype() != nullptr);

    outManager.addItemAttribute(handleItem1,
                                handleItem1Attribute,
                                &category,
                                displayableName,
                                shortDescription,
                                detailledDescription);

    QVERIFY(handleItem1Attribute.firstModel() != nullptr);
    QVERIFY(handleItem1Attribute.firstModel()->parentModel() == handleItem1.firstModel());
    QVERIFY(handleItem1Attribute.firstModel()->displayableName() == displayableName);
    QVERIFY(handleItem1Attribute.firstModel()->shortDescription() == shortDescription);
    QVERIFY(handleItem1Attribute.firstModel()->detailledDescription() == detailledDescription);
    QVERIFY(handleItem1Attribute.firstModel()->prototype() != nullptr);
    QVERIFY(dynamic_cast<AbstractItemAttributeForTest*>(handleItem1Attribute.firstModel()->prototype())->itemAttributeToolForModel()->valueType() == valueType);
    QVERIFY(dynamic_cast<AbstractItemAttributeForTest*>(handleItem1Attribute.firstModel()->prototype())->itemAttributeToolForModel()->category() == &category);

    outManager.finalize(indexGen);

    QCOMPARE(handleResult.model()->uniqueIndex(), 0);
    QCOMPARE(handleRootGroup.firstModel()->uniqueIndex(), 1);
    QCOMPARE(handleGroup1.firstModel()->uniqueIndex(), 2);
    QCOMPARE(handleItem2.firstModel()->uniqueIndex(), 3);
    QCOMPARE(handleGroup2.firstModel()->uniqueIndex(), 4);
    QCOMPARE(handleItem1.firstModel()->uniqueIndex(), 5);
    QCOMPARE(handleItem1Attribute.firstModel()->uniqueIndex(), 6);
}

void TestModels::testHandleResultCopyAddItemRemoveGroup()
{
    // output 1 :
    // handleResultOut1 [0]
    //   |
    //   |---handleRootGroupOut1 (possibility for handleGroupIn) [1]
    //          |--- handleGroupOut1_1 (possibility for handleGroupIn) [2]
    //          |
    //          |--- handleGroupOut1_2 (possibility for handleGroupIn) [3]

    // input :
    // handleResultIn
    //   |
    //   |--- handleRootGroupIn [*]
    //          |--- handleGroupIn

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
    CT_HandleOutGroup<StdGroupForTest>              handleGroupOut1_2;

    outManager1.addResult(handleResultOut1);
    outManager1.setRootGroup(handleResultOut1, handleRootGroupOut1);
    outManager1.addGroup(handleRootGroupOut1, handleGroupOut1_1);
    outManager1.addGroup(handleRootGroupOut1, handleGroupOut1_2);
    auto indexGenerator = [&currentUniqueIndexForOut]()->int { return currentUniqueIndexForOut++; };
    outManager1.finalize(indexGenerator);

    QCOMPARE(currentUniqueIndexForOut, 4);

    CT_InModelStructureManager inManager;
    CT_HandleInResultCopy<ResultGroupForTest>       handleResultIn;
    CT_HandleInZeroOrMoreGroup<StdGroupForTest>         handleRootGroupIn;
    CT_HandleInGroup<StdGroupForTest, 1, -1>                    handleGroupIn;

    inManager.addResult(handleResultIn);
    inManager.setZeroOrMoreRootGroup(handleResultIn, handleRootGroupIn);
    inManager.addGroup(handleRootGroupIn, handleGroupIn);

    // find all possibilities in out 1
    QVERIFY(handleResultIn.model()->recursiveFindAllPossibilitiesInModel(*handleResultOut1.model()) == true);

    // select all possibilities
    QVERIFY(handleResultIn.model()->recursiveVisitPossibilities([](const CT_InAbstractModel*, const CT_InStdModelPossibility* possibility) -> bool {
        return const_cast<CT_InStdModelPossibility*>(possibility)->setSelected(true);
    }) == true);

    // verify that selection is ok
    QCOMPARE(handleResultIn.model()->nPossibilitySelected(), 1);
    QCOMPARE(handleGroupIn.inModelForSavedPossibilities(handleResultIn)->nPossibilitySelected(), 3);

    CT_OutModelStructureManager outManager2;
    CT_HandleOutItem<ItemForTest>   handleItemOut2_1;

    // add the new out item to a copy of out 1
    outManager2.addResultCopy(handleResultIn);
    outManager2.addItem(handleGroupIn, handleItemOut2_1);
    QCOMPARE(handleItemOut2_1.nModels(), 3);

    // verify that the copy has been correctly created
    CT_OutAbstractResultModel* outResModel = nullptr;

    outManager2.visitResults([&outResModel](CT_OutAbstractResultModel* outResultModel) -> bool {
        Q_ASSERT(outResModel == nullptr);
        outResModel = outResultModel;
        return true;
    });

    QVERIFY(outResModel != nullptr);

    QSet<QString> handleItemOut2_1_addedTo;

    handleItemOut2_1.visitModels([&handleItemOut2_1_addedTo, &handleRootGroupOut1, &handleGroupOut1_1, &handleGroupOut1_2](const CT_OutAbstractModel* model) -> bool {
        CT_OutAbstractModel* originalParentModel = static_cast<CT_OutAbstractModel*>(model->parentModel())->recursiveOriginalModel();

        if(originalParentModel == model->parentModel())
            return false;

        if(originalParentModel == handleRootGroupOut1.firstModel())
            handleItemOut2_1_addedTo.insert("handleRootGroupOut1");
        else if(originalParentModel == handleGroupOut1_1.firstModel())
            handleItemOut2_1_addedTo.insert("handleGroupOut1_1");
        else if(originalParentModel == handleGroupOut1_2.firstModel())
            handleItemOut2_1_addedTo.insert("handleGroupOut1_2");
        else
            return false;

        return true;
    });

    QVERIFY(handleItemOut2_1_addedTo.contains("handleRootGroupOut1") == true);
    QVERIFY(handleItemOut2_1_addedTo.contains("handleGroupOut1_1") == true);
    QVERIFY(handleItemOut2_1_addedTo.contains("handleGroupOut1_2") == true);

    // remove a group
    outManager2.removeGroup(handleResultIn, handleGroupIn);

    // verify that the handle doesn't keep models that was destroyed
    QCOMPARE(handleItemOut2_1.nModels(), 0);

    // verify that we can't not add items because groups has been removed
    outManager2.addItem(handleGroupIn, handleItemOut2_1);
    QCOMPARE(handleItemOut2_1.nModels(), 0);

    outManager2.finalize(indexGenerator);

    QCOMPARE(currentUniqueIndexForOut, 5);
}

void TestModels::testWidgetSelection()
{
    // output 1 :
    // handleResultOut1 [0]
    //   |
    //   |---handleRootGroupOut1 (possibility for handleGroupIn) [1]
    //          |--- handleGroupOut1_1 (possibility for handleGroupIn) [2]
    //          |       |--- handleItemOut1_1 (possibility for handleItemIn) [3]
    //          |
    //          |--- handleGroupOut1_2 (possibility for handleGroupIn) [4]
    //          |       |--- handleItemOut1_2 (possibility for handleItemIn) [5]

    // input :
    // handleResultIn
    //   |
    //   |--- handleRootGroupIn [*]
    //          |--- handleGroupIn

    /*StepForTest myStep;
    CT_OutModelStructureManager outManager1;
    CT_HandleOutResult<ResultGroupForTest>  handleResultOut1;
    CT_HandleOutGroup<StdGroupForTest>          handleRootGroupOut1;
    CT_HandleOutGroup<StdGroupForTest>              handleGroupOut1_1;
    CT_HandleOutItem<ItemForTest>                       handleItemOut1_1;
    CT_HandleOutGroup<StdGroupForTest>              handleGroupOut1_2;
    CT_HandleOutItem<ItemForTest>                       handleItemOut1_2;

    outManager1.addResult(handleResultOut1);
    outManager1.setRootGroup(handleResultOut1, handleRootGroupOut1);
    outManager1.addGroup(handleRootGroupOut1, handleGroupOut1_1, "Out Group 1");
    outManager1.addItem(handleGroupOut1_1, handleItemOut1_1, "Out Item 1");
    outManager1.addGroup(handleRootGroupOut1, handleGroupOut1_2, "Out Group 2");
    outManager1.addItem(handleGroupOut1_2, handleItemOut1_2, "Out Item 2");
    handleResultOut1.model()->setStep(&myStep);

    CT_InModelStructureManager inManager;
    CT_HandleInResultCopy<ResultGroupForTest>       handleResultIn;
    CT_HandleInZeroOrMoreGroup<StdGroupForTest>         handleRootGroupIn;
    CT_HandleInGroup<StdGroupForTest, 1, -1>                    handleGroupIn;
    CT_HandleInItem<ItemForTest, 1, -1>                            handleItemIn;

    inManager.addResult(handleResultIn);
    inManager.setZeroOrMoreRootGroup(handleResultIn, handleRootGroupIn);
    inManager.addGroup(handleRootGroupIn, handleGroupIn);
    inManager.addItem(handleGroupIn, handleItemIn);

    // find all possibilities in out 1
    QVERIFY(handleResultIn.model()->recursiveFindAllPossibilitiesInModel(*handleResultOut1.model()) == true);

    int n = 0;
    char* bla = nullptr;

    QApplication app(n, &bla);
    CT_InResultModelConfigurationManager cm(inManager);

    QVERIFY(cm.createInResultModelConfigurationDialog() == CT_InResultModelConfigurationManager::CreateSuccess);
    QVERIFY(cm.configureInResultModel() == CT_InResultModelConfigurationManager::ConfigureSuccess);*/
}

void TestModels::testWidgetSelection2()
{
    // output 1 :
    // handleResultOut1 [0]
    //   |
    //   |---handleRootGroupOut1 (possibility for handleGroupIn) [1]
    //          |--- handleItemOut1_1 (possibility for handleItemIn) [3]
    //          |--- handleItemOut1_2 (possibility for handleItemIn) [5]

    // input :
    // handleResultIn
    //   |
    //   |--- handleRootGroupIn [*]
    //          |--- handleGroupIn

    StepForTest myStep;
    CT_OutModelStructureManager outManager1;
    CT_HandleOutResult<ResultGroupForTest>  handleResultOut1;
    CT_HandleOutGroup<StdGroupForTest>          handleRootGroupOut1;
    CT_HandleOutItem<ItemForTest>                   handleItemOut1_1;
    CT_HandleOutItem<ItemForTest>                   handleItemOut1_2;

    outManager1.addResult(handleResultOut1);
    outManager1.setRootGroup(handleResultOut1, handleRootGroupOut1);
    outManager1.addItem(handleRootGroupOut1, handleItemOut1_1, "Out Item 1");
    outManager1.addItem(handleRootGroupOut1, handleItemOut1_2, "Out Item 2");
    handleResultOut1.model()->setStep(&myStep);

    CT_InModelStructureManager inManager;
    CT_HandleInResultCopy<ResultGroupForTest>       handleResultIn;
    CT_HandleInZeroOrMoreGroup<StdGroupForTest>         handleRootGroupIn;
    CT_HandleInGroup<StdGroupForTest, 1, -1>                    handleGroupIn;
    CT_HandleInItem<ItemForTest, 1, -1>                            handleItemIn;

    inManager.addResult(handleResultIn);
    inManager.setZeroOrMoreRootGroup(handleResultIn, handleRootGroupIn);
    inManager.addGroup(handleRootGroupIn, handleGroupIn);
    inManager.addItem(handleGroupIn, handleItemIn);

    // find all possibilities in out 1
    QVERIFY(handleResultIn.model()->recursiveFindAllPossibilitiesInModel(*handleResultOut1.model()) == true);

    int n = 0;
    char* bla = nullptr;

    QApplication app(n, &bla);
    CT_InResultModelConfigurationManager cm(inManager);

    QVERIFY(cm.createInResultModelConfigurationDialog<CTG_InModelPossibilitiesChoice>() == CT_InResultModelConfigurationManager::CreateSuccess);
    QVERIFY(cm.configureInResultModel() == CT_InResultModelConfigurationManager::ConfigureSuccess);
}


QTEST_APPLESS_MAIN(TestModels)

#include "tst_testmodels.moc"
