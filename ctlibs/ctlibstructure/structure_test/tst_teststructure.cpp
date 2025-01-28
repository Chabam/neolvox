#include <QString>
#include <QtTest>

#include "ct_categories/ct_stdcategory.h"
#include "ct_global/ct_structurecontext.h"
#include "ct_categories/tools/ct_categorymanager.h"
#include "ct_itemattributes/tools/ct_defaultitemattributemanager.h"
#include "ct_itemattributes/ct_stditemattributet.h"
#include "ct_itemattributes/ct_stditemattributewrappert.h"
#include "ct_itemdrawable/abstract/ct_abstractsingularitemdrawable.h"

#include "ct_model/outModel/ct_outstditemattributemodel.h"

#include "ct_item/abstract/ct_abstractitem.h"

#include "ct_itemdrawable/ct_standarditemgroup.h"
#include "ct_handle/ct_handleoutresultgroup.h"
#include "ct_handle/ct_handleoutstdgroup.h"
#include "ct_handle/ct_handleoutsingularitem.h"
#include "ct_handle/ct_handleoutstditemattribute.h"
#include "ct_handle/ct_handleinresultgroup.h"
#include "ct_handle/ct_handleinresultgroupcopy.h"
#include "ct_handle/ct_handleinstdzeroormoregroup.h"
#include "ct_handle/ct_handleinstdgroup.h"
#include "ct_handle/ct_handleinsingularitem.h"
#include "ct_handle/ct_handleinstditemattribute.h"
#include "ct_handle/ct_handleinstditemattributet.h"

#include "ct_result/ct_resultgroup.h"

#include "ct_model/outModel/manager/ct_outmodelstructuremanager.h"
#include "ct_model/inModel/manager/ct_inmodelstructuremanager.h"

struct NoDebugOutput {
    template<typename T>
    NoDebugOutput& operator<<(const T&) { return *this; }

    NoDebugOutput& noquote() { return *this; }
};

//#define myDebug() qDebug()
#define myDebug() NoDebugOutput()

class MySingularItem : public CT_AbstractSingularItemDrawable {
public:
    MySingularItem() : CT_AbstractSingularItemDrawable() {}
    MySingularItem(const MySingularItem& other) : CT_AbstractSingularItemDrawable(other) {}

    CT_AbstractItemDrawable* copy(const CT_OutAbstractItemModel* newModelToUse, const CT_AbstractResult* newResult) const {
        MySingularItem* c = new MySingularItem(*this);
        c->setModel(newModelToUse);
        c->setResult(newResult);

        return c;
    }
};

class MyStep : public IStepForModel {
public:
    MyStep() { m_tool.thisPointer = this; }
    ~MyStep() { clearResults(); }

    void addResult(CT_AbstractResult* res) { m_results.append(res); }
    void clearResults() { qDeleteAll(m_results); m_results.clear(); }

    IStepToolForModel* stepToolForModel() const { return &const_cast<MyStepToolForModel&>(m_tool); }

    bool visitStepHisOutResults(const IStepToolForModel::OutResultsVisitor& visitor) const {
        for(const CT_AbstractResult* res : m_results) {
            if(!visitor(res))
                return false;
        }

        return true;
    }

private:
    class MyStepToolForModel : public IStepToolForModel {
    public:
        QString displayableCustomName() const { return "Step"; }
        bool isStepAPrototype() const { return false; }
        bool visitStepHisOutResults(const OutResultsVisitor& visitor) const {
            return thisPointer->visitStepHisOutResults(visitor);
        }

        MyStep* thisPointer;
    };

    QList<CT_AbstractResult*>   m_results;
    MyStepToolForModel          m_tool;
};

class TestStructure : public QObject
{
    Q_OBJECT

public:
    TestStructure();

private Q_SLOTS:
    void testAbstractCategoryValueTypeToCategoryTypeConversion();
    void testStdCategoryCreation();
    void testStructureContextCreation();
    void testStdItemAttributeTCreation();
    void testStdItemAttributeWrapperTCreation();
    void testCompleteStructureCreation();
    void testCompleteStructureCreationInDifferentOrder();
    void testCompleteStructureCreationAndOneCopy();
    void testCompleteStructureCreationAndDoubleCopy();
};

TestStructure::TestStructure()
{
}

void TestStructure::testAbstractCategoryValueTypeToCategoryTypeConversion()
{
    QVERIFY(CT_AbstractCategory::staticValueTypeToCategoryType<bool>() == CT_AbstractCategory::BOOLEAN);
    QVERIFY(CT_AbstractCategory::staticValueTypeToCategoryType<double>() == CT_AbstractCategory::DOUBLE);
    QVERIFY(CT_AbstractCategory::staticValueTypeToCategoryType<float>() == CT_AbstractCategory::FLOAT);
    QVERIFY(CT_AbstractCategory::staticValueTypeToCategoryType<long double>() == CT_AbstractCategory::LONG_DOUBLE);
    QVERIFY(CT_AbstractCategory::staticValueTypeToCategoryType<quint8>() == CT_AbstractCategory::UINT8);
    QVERIFY(CT_AbstractCategory::staticValueTypeToCategoryType<quint16>() == CT_AbstractCategory::UINT16);
    QVERIFY(CT_AbstractCategory::staticValueTypeToCategoryType<quint32>() == CT_AbstractCategory::UINT32);
    QVERIFY(CT_AbstractCategory::staticValueTypeToCategoryType<quint64>() == CT_AbstractCategory::UINT64);
    QVERIFY(CT_AbstractCategory::staticValueTypeToCategoryType<qint8>() == CT_AbstractCategory::INT8);
    QVERIFY(CT_AbstractCategory::staticValueTypeToCategoryType<qint16>() == CT_AbstractCategory::INT16);
    QVERIFY(CT_AbstractCategory::staticValueTypeToCategoryType<qint32>() == CT_AbstractCategory::INT32);
    QVERIFY(CT_AbstractCategory::staticValueTypeToCategoryType<qint64>() == CT_AbstractCategory::INT64);
    QVERIFY(CT_AbstractCategory::staticValueTypeToCategoryType<size_t>() == CT_AbstractCategory::SIZE_T);
    QVERIFY(CT_AbstractCategory::staticValueTypeToCategoryType<QString>() == CT_AbstractCategory::STRING);
}

void TestStructure::testStdCategoryCreation()
{
    const QString uniqueName("un");
    const QStringList categoryEquivalentUniqueNameCollection{CT_AbstractCategory::DATA_ANGLE, CT_AbstractCategory::DATA_DIMENSION};
    const QString displayableName("dn");
    const QString description("des");

    CT_StdCategory cat(uniqueName,
                       categoryEquivalentUniqueNameCollection,
                       displayableName,
                       description);

    QVERIFY(cat.uniqueName() == uniqueName);
    QVERIFY(cat.displayableName() == displayableName);
    QVERIFY(cat.description() == description);

    for(const QString& catName : categoryEquivalentUniqueNameCollection) {
        QVERIFY(cat.isEquivalentTo(catName) == true);
    }

    QVERIFY(cat.isEquivalentTo(CT_AbstractCategory::DATA_VALUE) == true);
    QVERIFY(cat.isEquivalentTo(CT_AbstractCategory::DATA_X) == false);
    QVERIFY(cat.isEquivalentTo(CT_AbstractCategory::DATA_X_DIMENSION) == false);
}

void TestStructure::testStructureContextCreation()
{
    QVERIFY(PS_STRUCTURECONTEXT != NULL);
    QVERIFY(PS_CATEGORY_MANAGER != NULL);
    QVERIFY(PS_DIAM != NULL);
    QVERIFY(PS_CATEGORY_MANAGER->findByUniqueName(CT_AbstractCategory::DATA_VALUE) != NULL);
    QVERIFY(PS_CATEGORY_MANAGER->findByUniqueName(CT_AbstractCategory::DATA_ID) != NULL);
    QVERIFY(PS_CATEGORY_MANAGER->findByUniqueName(CT_AbstractCategory::DATA_DISPLAYABLE_NAME) != NULL);
    QVERIFY(PS_CATEGORY_MANAGER->findByUniqueName(CT_AbstractCategory::DATA_FILE_NAME) != NULL);
    QVERIFY(PS_CATEGORY_MANAGER->findByUniqueName(CT_AbstractCategory::DATA_COORDINATE) != NULL);
    QVERIFY(PS_CATEGORY_MANAGER->findByUniqueName(CT_AbstractCategory::DATA_X) != NULL);
    QVERIFY(PS_CATEGORY_MANAGER->findByUniqueName(CT_AbstractCategory::DATA_Y) != NULL);
    QVERIFY(PS_CATEGORY_MANAGER->findByUniqueName(CT_AbstractCategory::DATA_Z) != NULL);
    QVERIFY(PS_CATEGORY_MANAGER->findByUniqueName(CT_AbstractCategory::DATA_CX) != NULL);
    QVERIFY(PS_CATEGORY_MANAGER->findByUniqueName(CT_AbstractCategory::DATA_CY) != NULL);
    QVERIFY(PS_CATEGORY_MANAGER->findByUniqueName(CT_AbstractCategory::DATA_CZ) != NULL);
    QVERIFY(PS_CATEGORY_MANAGER->findByUniqueName(CT_AbstractCategory::DATA_DIRECTION) != NULL);
    QVERIFY(PS_CATEGORY_MANAGER->findByUniqueName(CT_AbstractCategory::DATA_X_DIRECTION) != NULL);
    QVERIFY(PS_CATEGORY_MANAGER->findByUniqueName(CT_AbstractCategory::DATA_Y_DIRECTION) != NULL);
    QVERIFY(PS_CATEGORY_MANAGER->findByUniqueName(CT_AbstractCategory::DATA_Z_DIRECTION) != NULL);
    QVERIFY(PS_CATEGORY_MANAGER->findByUniqueName(CT_AbstractCategory::DATA_DIMENSION) != NULL);
    QVERIFY(PS_CATEGORY_MANAGER->findByUniqueName(CT_AbstractCategory::DATA_X_DIMENSION) != NULL);
    QVERIFY(PS_CATEGORY_MANAGER->findByUniqueName(CT_AbstractCategory::DATA_Y_DIMENSION) != NULL);
    QVERIFY(PS_CATEGORY_MANAGER->findByUniqueName(CT_AbstractCategory::DATA_Z_DIMENSION) != NULL);
    QVERIFY(PS_CATEGORY_MANAGER->findByUniqueName(CT_AbstractCategory::DATA_RADIUS) != NULL);
    QVERIFY(PS_CATEGORY_MANAGER->findByUniqueName(CT_AbstractCategory::DATA_R2) != NULL);
    QVERIFY(PS_CATEGORY_MANAGER->findByUniqueName(CT_AbstractCategory::DATA_SIZE) != NULL);
    QVERIFY(PS_CATEGORY_MANAGER->findByUniqueName(CT_AbstractCategory::DATA_NUMBER) != NULL);
    QVERIFY(PS_CATEGORY_MANAGER->findByUniqueName(CT_AbstractCategory::DATA_LENGTH) != NULL);
    QVERIFY(PS_CATEGORY_MANAGER->findByUniqueName(CT_AbstractCategory::DATA_WIDTH) != NULL);
    QVERIFY(PS_CATEGORY_MANAGER->findByUniqueName(CT_AbstractCategory::DATA_HEIGHT) != NULL);
    QVERIFY(PS_CATEGORY_MANAGER->findByUniqueName(CT_AbstractCategory::DATA_AREA) != NULL);
    QVERIFY(PS_CATEGORY_MANAGER->findByUniqueName(CT_AbstractCategory::DATA_ANGLE) != NULL);
    QVERIFY(PS_CATEGORY_MANAGER->findByUniqueName(CT_AbstractCategory::DATA_RATIO) != NULL);
    QVERIFY(PS_CATEGORY_MANAGER->findByUniqueName(CT_AbstractCategory::DATA_RESOLUTION) != NULL);
    QVERIFY(PS_CATEGORY_MANAGER->findByUniqueName(CT_AbstractCategory::DATA_X_RESOLUTION) != NULL);
    QVERIFY(PS_CATEGORY_MANAGER->findByUniqueName(CT_AbstractCategory::DATA_Y_RESOLUTION) != NULL);
    QVERIFY(PS_CATEGORY_MANAGER->findByUniqueName(CT_AbstractCategory::DATA_Z_RESOLUTION) != NULL);
    QVERIFY(PS_CATEGORY_MANAGER->findByUniqueName(CT_AbstractCategory::DATA_NA) != NULL);
    QVERIFY(PS_DIAM->itemAttributes(CT_AbstractSingularItemDrawable::staticType()).isEmpty() == false);
}

void TestStructure::testStdItemAttributeTCreation()
{
    const CT_AbstractCategory* cat = PS_CATEGORY_MANAGER->findByUniqueName(CT_AbstractCategory::DATA_ANGLE);

    const QString data("coucou");
    const QString displayableName("dn");
    const QString shortDescription("des");
    const QString fullDescription("fdes");

    CT_StdItemAttributeT<bool>              attBool(cat, true);
    CT_StdItemAttributeT<double>            attDouble(cat, std::numeric_limits<double>::max());
    CT_StdItemAttributeT<float>             attFloat(cat, std::numeric_limits<float>::max());
    CT_StdItemAttributeT<long double>       attLongDouble(cat, std::numeric_limits<long double>::max());
    CT_StdItemAttributeT<quint8>            attUint8(cat, std::numeric_limits<quint8>::max());
    CT_StdItemAttributeT<quint16>           attUint16(cat, std::numeric_limits<quint16>::max());
    CT_StdItemAttributeT<quint32>           attUint32(cat, std::numeric_limits<quint32>::max());
    CT_StdItemAttributeT<quint64>           attUint64(cat, std::numeric_limits<quint64>::max());
    CT_StdItemAttributeT<qint8>             attInt8(cat, -std::numeric_limits<qint8>::max());
    CT_StdItemAttributeT<qint16>            attInt16(cat, -std::numeric_limits<qint16>::max());
    CT_StdItemAttributeT<qint32>            attInt32(cat, -std::numeric_limits<qint32>::max());
    CT_StdItemAttributeT<qint64>            attInt64(cat, -std::numeric_limits<qint64>::max());
    CT_StdItemAttributeT<size_t>            attSizeT(cat, std::numeric_limits<size_t>::max());
    CT_StdItemAttributeT<QString>           attQString(cat, data);

    QVERIFY(attBool.valueType() == CT_AbstractCategory::BOOLEAN);
    QVERIFY(attDouble.valueType() == CT_AbstractCategory::DOUBLE);
    QVERIFY(attFloat.valueType() == CT_AbstractCategory::FLOAT);
    QVERIFY(attLongDouble.valueType() == CT_AbstractCategory::LONG_DOUBLE);
    QVERIFY(attUint8.valueType() == CT_AbstractCategory::UINT8);
    QVERIFY(attUint16.valueType() == CT_AbstractCategory::UINT16);
    QVERIFY(attUint32.valueType() == CT_AbstractCategory::UINT32);
    QVERIFY(attUint64.valueType() == CT_AbstractCategory::UINT64);
    QVERIFY(attInt8.valueType() == CT_AbstractCategory::INT8);
    QVERIFY(attInt16.valueType() == CT_AbstractCategory::INT16);
    QVERIFY(attInt32.valueType() == CT_AbstractCategory::INT32);
    QVERIFY(attInt64.valueType() == CT_AbstractCategory::INT64);
    QVERIFY(attSizeT.valueType() == CT_AbstractCategory::SIZE_T);
    QVERIFY(attQString.valueType() == CT_AbstractCategory::STRING);

    QCOMPARE(attBool.valueTypeToString(), "BOOL");
    QCOMPARE(attDouble.valueTypeToString(), "DOUBLE");
    QCOMPARE(attFloat.valueTypeToString(), "FLOAT");
    QCOMPARE(attLongDouble.valueTypeToString(), "LONG_DOUBLE");
    QCOMPARE(attUint8.valueTypeToString(), "UNSIGNED INT8");
    QCOMPARE(attUint16.valueTypeToString(), "UNSIGNED INT16");
    QCOMPARE(attUint32.valueTypeToString(), "UNSIGNED INT32");
    QCOMPARE(attUint64.valueTypeToString(), "UNSIGNED INT64");
    QCOMPARE(attInt8.valueTypeToString(), "INT8");
    QCOMPARE(attInt16.valueTypeToString(), "INT16");
    QCOMPARE(attInt32.valueTypeToString(), "INT32");
    QCOMPARE(attInt64.valueTypeToString(), "INT64");

#ifdef ENVIRONMENT64
    QCOMPARE(attSizeT.valueTypeToString(), "UNSIGNED INT64");
#else
    QCOMPARE(attSizeT.valueTypeToString(), "UNSIGNED INT32");
#endif

    QCOMPARE(attQString.valueTypeToString(), "STRING");

    QCOMPARE(attBool.data(NULL), true);
    QCOMPARE(attDouble.data(NULL), std::numeric_limits<double>::max());
    QCOMPARE(attFloat.data(NULL), std::numeric_limits<float>::max());
    QCOMPARE(attLongDouble.data(NULL), std::numeric_limits<long double>::max());
    QCOMPARE(attUint8.data(NULL), std::numeric_limits<quint8>::max());
    QCOMPARE(attUint16.data(NULL), std::numeric_limits<quint16>::max());
    QCOMPARE(attUint32.data(NULL), std::numeric_limits<quint32>::max());
    QCOMPARE(attUint64.data(NULL), std::numeric_limits<quint64>::max());
    QCOMPARE(attInt8.data(NULL), -std::numeric_limits<qint8>::max());
    QCOMPARE(attInt16.data(NULL), -std::numeric_limits<qint16>::max());
    QCOMPARE(attInt32.data(NULL), -std::numeric_limits<qint32>::max());
    QCOMPARE(attInt64.data(NULL), -std::numeric_limits<qint64>::max());
    QCOMPARE(attSizeT.data(NULL), std::numeric_limits<size_t>::max());
    QCOMPARE(attQString.data(NULL), data);

    QVERIFY(attBool.category() == cat);

    QVERIFY(cat->displayableName().isEmpty() == false);
    QCOMPARE(attBool.displayableName(), cat->displayableName());

    CT_OutStdItemAttributeModel<CT_StdItemAttributeT<double>> attDoubleModel(cat,
                                                                             displayableName,
                                                                             shortDescription,
                                                                             fullDescription);

    attDouble.setModel(&attDoubleModel);
    QCOMPARE(attDouble.displayableName(), displayableName);

    CT_AbstractItemAttribute* attUint8Copy = attUint8.copy(NULL, NULL);

    QVERIFY(attUint8Copy != NULL);
    QCOMPARE(static_cast<CT_StdItemAttributeT<quint8>*>(attUint8Copy)->data(NULL), std::numeric_limits<quint8>::max());
    QCOMPARE(attUint8Copy->toDouble(NULL, NULL), std::numeric_limits<quint8>::max());

    delete attUint8Copy;
}

template <typename ItemClass, typename VType>
CT_StdItemAttributeWrapperT<ItemClass, VType>* generateItemAttribute(const QString& categoryUniqueName,
                           VType (ItemClass::*getter)() const)
{
    const CT_AbstractCategory* cat = PS_CATEGORY_MANAGER->findByUniqueName(categoryUniqueName);

    using PrototypeType = CT_StdItemAttributeWrapperT<ItemClass, VType>;

    return new PrototypeType(cat, getter);
}

void TestStructure::testStdItemAttributeWrapperTCreation()
{
    class InternalTest : public CT_AbstractItem {
    public:
        using DataType = float;
        enum { CatEnumValueType = CT_AbstractCategory::FLOAT };

        DataType value() const { return 1.8f; }

        bool visitChildrens(const ChildrensVisitor&) const override { return true; }

        int nChildrens() const override { return 0; }

    protected:
        IChildrensIteratorQtStylePtr createQtStyleIteratorForChildrensThatUseOutModel(const CT_OutAbstractModel*) const {
            return IChildrensIteratorQtStylePtr(NULL);
        }
    };

    class InternalTest2 : public InternalTest {
        public:
    };

    //const CT_AbstractCategory* cat = PS_CATEGORY_MANAGER->findByUniqueName(CT_AbstractCategory::DATA_VALUE);

    using WrapperType = CT_StdItemAttributeWrapperT<InternalTest, InternalTest::DataType>;
    //using WrapperType2 = CT_StdItemAttributeWrapperT<InternalTest2, InternalTest2::DataType>;

    auto attWrapper = generateItemAttribute(CT_AbstractCategory::DATA_VALUE, &InternalTest::value);
    auto attWrapper2 = generateItemAttribute(CT_AbstractCategory::DATA_VALUE, &InternalTest2::value);

    InternalTest it;

    QVERIFY(attWrapper->valueType() == InternalTest::CatEnumValueType);
    QCOMPARE(attWrapper->data(&it), it.value());
    QCOMPARE(attWrapper->valueTypeToString(), "FLOAT");
    QCOMPARE(attWrapper2->valueTypeToString(), "FLOAT");

    CT_AbstractItemAttribute* attWrapperCopy = attWrapper->copy(NULL, NULL);

    QVERIFY(attWrapperCopy != NULL);
    QCOMPARE(static_cast<WrapperType*>(attWrapperCopy)->data(NULL), it.value());
    QCOMPARE(attWrapperCopy->toDouble(NULL, NULL), it.value());

    delete attWrapperCopy;
    delete attWrapper;
    delete attWrapper2;
}

void TestStructure::testCompleteStructureCreation()
{
    const CT_AbstractCategory* cat = PS_CATEGORY_MANAGER->findByUniqueName(CT_AbstractCategory::DATA_VALUE);

    using ItemAttributeClassType = CT_StdItemAttributeT<double>;

    CT_HandleOutResultGroup hResult;
    CT_HandleOutStdGroup hRootGroup;
    CT_HandleOutStdGroup hChildGroup;
    CT_HandleOutSingularItem<MySingularItem> hChildItem;
    CT_HandleOutStdItemAttribute<double> hItemAttribute;

    CT_OutModelStructureManager outManager;
    outManager.addResult(hResult);
    outManager.setRootGroup(hResult, hRootGroup);
    outManager.addGroup(hRootGroup, hChildGroup);
    outManager.addItem(hChildGroup, hChildItem);
    outManager.addItemAttribute(hChildItem, hItemAttribute, cat);

    CT_ResultGroup* result = hResult.createInstance();

    QVERIFY(result != NULL);
    QVERIFY(result->model() == hResult.model());

    CT_StandardItemGroup* rootGroup = hRootGroup.createInstance();
    QVERIFY(rootGroup != NULL);
    result->addRootGroup(hRootGroup, rootGroup);

    QVERIFY(rootGroup->result() == result);
    QVERIFY(rootGroup->model() == hRootGroup.firstModel());
    QVERIFY(rootGroup->model()->result() == result);

    CT_StandardItemGroup* childGroup = hChildGroup.createInstance();
    QVERIFY(childGroup != NULL);
    rootGroup->addGroup(hChildGroup, childGroup);

    QVERIFY(childGroup->result() == result);
    QVERIFY(childGroup->model() == hChildGroup.firstModel());
    QVERIFY(childGroup->model()->result() == result);

    MySingularItem* childItem = hChildItem.createInstance();
    QVERIFY(childItem != NULL);
    childGroup->addSingularItem(hChildItem, childItem);

    QVERIFY(childItem->result() == result);
    QVERIFY(childItem->model() == hChildItem.firstModel());
    QVERIFY(childItem->model()->result() == result);

    ItemAttributeClassType* attribute = hItemAttribute.createInstance(cat, 5.0);
    QVERIFY(attribute != NULL);
    QVERIFY(attribute->category() == cat);
    QVERIFY(attribute->data(NULL) == 5.0);
    childItem->addItemAttribute(hItemAttribute, attribute);

    QVERIFY(attribute->result() == result);
    QVERIFY(attribute->model() == hItemAttribute.firstModel());
    QVERIFY(attribute->model()->result() == result);

    QVERIFY(result->isEmpty() == false);

    for(const CT_StandardItemGroup* g : *result) {
        QVERIFY(g == rootGroup);
    }

    const auto visitorG = [&childGroup](const CT_StandardItemGroup* group) -> bool {
        return group == childGroup;
    };

    QCOMPARE(rootGroup->nGroup(), 1);
    QVERIFY(rootGroup->visitGroups(visitorG) == true);

    const auto visitorI = [&childItem](const CT_AbstractSingularItemDrawable* item) -> bool {
        return item == childItem;
    };

    QCOMPARE(childGroup->nSingularItem(), 1);
    QVERIFY(childGroup->visitSingularItems(visitorI) == true);

    const auto visitorIA = [&attribute](const CT_AbstractItemAttribute* att) -> bool {
        return att == attribute;
    };

    QCOMPARE(childItem->nItemAttributesAdded(), 1);
    QVERIFY(childItem->visitItemAttributesAdded(visitorIA) == true);

    bool rootGroupHasBeenDeleted = false;
    bool childGroupHasBeenDeleted = false;
    bool childItemHasBeenDeleted = false;
    bool itemAttributeHasBeenDeleted = false;

    QObject::connect(rootGroup, &CT_StandardItemGroup::destroyed, [&rootGroupHasBeenDeleted]() {
        rootGroupHasBeenDeleted = true;
    });

    QObject::connect(childGroup, &CT_StandardItemGroup::destroyed, [&childGroupHasBeenDeleted]() {
        childGroupHasBeenDeleted = true;
    });

    QObject::connect(childItem, &CT_StandardItemGroup::destroyed, [&childItemHasBeenDeleted]() {
        childItemHasBeenDeleted = true;
    });

    QObject::connect(attribute, &CT_StandardItemGroup::destroyed, [&itemAttributeHasBeenDeleted]() {
        itemAttributeHasBeenDeleted = true;
    });

    delete result;

    QVERIFY(rootGroupHasBeenDeleted == true);
    QVERIFY(childGroupHasBeenDeleted == true);
    QVERIFY(childItemHasBeenDeleted == true);
    QVERIFY(itemAttributeHasBeenDeleted == true);
}

void TestStructure::testCompleteStructureCreationInDifferentOrder()
{
    CT_HandleOutResultGroup hResult;
    CT_HandleOutStdGroup hRootGroup;
    CT_HandleOutStdGroup hChildGroup;
    CT_HandleOutSingularItem<MySingularItem> hChildItem;

    CT_OutModelStructureManager outManager;
    outManager.addResult(hResult);
    outManager.setRootGroup(hResult, hRootGroup);
    outManager.addGroup(hRootGroup, hChildGroup);
    outManager.addItem(hChildGroup, hChildItem);

    //CT_ResultGroup* result = hResult.createInstance();
    //CT_StandardItemGroup* rootGroup = hRootGroup.createInstance();
    //CT_StandardItemGroup* childGroup = hChildGroup.createInstance();
    //MySingularItem* childItem = hChildItem.createInstance();

    // Must this be allowed ??

    //childGroup->addSingularItem(hChildItem, childItem);
    //rootGroup->addGroup(hChildGroup, childGroup);
    //result->addRootGroup(hRootGroup, rootGroup);

    bool addInDifferentOrderIsAllowed = false;

    QVERIFY(addInDifferentOrderIsAllowed == true);
}

QString createPre(const CT_AbstractModel* m) {
    QString pre;

    CT_AbstractModel* p = m->parentModel();
    while(p != NULL) {
        pre += "    ";
        p = p->parentModel();
    }

    return pre;
}

void printResult(CT_ResultGroup* result) {
    myDebug() << "result = " << result;
    myDebug() << result->model() << "[ res = " << result->model()->result() << "]";

    for(const CT_StandardItemGroup* rootG : *result) {
        myDebug().noquote() << createPre(rootG->model()) << rootG->model() << "[ res = " << rootG->model()->result() << "]";

        QString previousPre;

        rootG->recursiveVisitChildrens([&previousPre](const CT_AbstractItem* child) -> bool {

            QString currentPre = (child->model()->parentModel() == NULL) ? previousPre : createPre(child->model());
            previousPre = currentPre;

            if(child->model()->parentModel() == NULL)
                currentPre += "    ";

            myDebug().noquote() << currentPre << child->model() << "[ res = " << child->model()->result() << "]";

            return true;
        });
    }
}

void TestStructure::testCompleteStructureCreationAndOneCopy()
{
    const CT_AbstractCategory* cat = PS_CATEGORY_MANAGER->findByUniqueName(CT_AbstractCategory::DATA_VALUE);

    // out model 1
    MyStep step1;
    CT_HandleOutResultGroup hOutResult;
    CT_HandleOutStdGroup hOutRootGroup;
    CT_HandleOutStdGroup hOutChildGroup;
    CT_HandleOutSingularItem<MySingularItem> hOutChildItem;
    CT_HandleOutStdItemAttribute<bool> hOutItemAttribute;

    CT_OutModelStructureManager outManager;
    outManager.addResult(hOutResult);
    outManager.setRootGroup(hOutResult, hOutRootGroup);
    outManager.addGroup(hOutRootGroup, hOutChildGroup);
    outManager.addItem(hOutChildGroup, hOutChildItem);
    outManager.addItemAttribute(hOutChildItem, hOutItemAttribute, cat);

    MyStep* currentStep = NULL;
    const auto visitorSetStep = [&currentStep](const CT_OutAbstractResultModel* resultModel) -> bool {
        const_cast<CT_OutAbstractResultModel*>(resultModel)->setStep(currentStep);
        return true;
    };

    currentStep = &step1;
    outManager.visitResults(visitorSetStep);

    myDebug() << "--------- outModel 1 ---------";
    myDebug() << hOutResult.model() << "[" << hOutResult.model()->result() << "]";
    hOutResult.model()->recursiveVisitOutChildrens([](const CT_OutAbstractModel* model) -> bool {
        myDebug().noquote() << createPre(model) << model << "[" << model->result() << "]";
        return true;
    });

    // in model
    CT_HandleInResultGroupCopy<> hInResultCopy;
    CT_HandleInStdZeroOrMoreGroup hInRootGroup;
    CT_HandleInStdGroup<> hInChildGroup;
    CT_HandleInSingularItem<CT_AbstractSingularItemDrawable> hInChildItem;
    CT_HandleInStdItemAttribute<CT_AbstractCategory::BOOLEAN> hInItemAttribute;
    CT_HandleInStdItemAttributeT<bool> hInItemAttributeT;

    CT_InModelStructureManager inManager;
    inManager.addResult(hInResultCopy);
    inManager.setZeroOrMoreRootGroup(hInResultCopy, hInRootGroup);
    inManager.addGroup(hInRootGroup, hInChildGroup);
    inManager.addItem(hInChildGroup, hInChildItem);
    inManager.addItemAttribute(hInChildItem, hInItemAttributeT, CT_AbstractCategory::DATA_VALUE);

    //QVERIFY(inManager.findPossibilitiesInStep(parentStep) == true);

    QVERIFY(hInResultCopy.model()->recursiveFindAllPossibilitiesInModel(*hOutResult.model()) == true);

    QStringList errors;
    bool ok = hInResultCopy.model()->recursiveHasTheMinimumNumberOfPossibilityRequired(&errors);

    if(!ok) {
        myDebug() << errors;
        QVERIFY(false);
    }

    // select all possibilities
    QVERIFY(hInResultCopy.model()->recursiveVisitPossibilities([](const CT_InAbstractModel*, const CT_InStdModelPossibility* possibility) -> bool {
        return const_cast<CT_InStdModelPossibility*>(possibility)->setSelected(true);
    }) == true);

    ok = hInResultCopy.model()->recursiveHasTheMinimumNumberOfSelectedPossibilityRequired(&errors);

    if(!ok) {
        myDebug() << errors;
        QVERIFY(false);
    }

    // out model 2 (copy of out model 1)
    MyStep step2;
    CT_HandleOutSingularItem<MySingularItem> hOutChildItem2;
    CT_OutModelStructureManager outManager2;
    outManager2.addResultCopy(hInResultCopy);
    outManager2.addItem(hInChildGroup, hOutChildItem2); // add an item in the group

    QCOMPARE(hOutChildItem2.nModels(), 1);

    currentStep = &step2;
    outManager2.visitResults(visitorSetStep);

    CT_OutResultModelGroupCopy* finalOutResultModel2 = NULL;

    myDebug() << "--------- outModel 2 ---------";
    outManager2.visitResults([&finalOutResultModel2](const CT_OutAbstractResultModel* resultModel) -> bool {
        finalOutResultModel2 = dynamic_cast<CT_OutResultModelGroupCopy*>(const_cast<CT_OutAbstractResultModel*>(resultModel));

        myDebug() << resultModel << "[" << resultModel->result() << "]";
        resultModel->recursiveVisitOutChildrens([](const CT_OutAbstractModel* model) -> bool {
            myDebug().noquote() << createPre(model) << model << "[" << model->result() << "]";
            return true;
        });

        return true;
    });

    QVERIFY(finalOutResultModel2 != NULL);
    QVERIFY(hOutChildItem2.firstModel() != NULL);
    QVERIFY(hOutChildItem2.firstModel()->parentModel() == finalOutResultModel2->rootGroup()->groups().at(0));

    // create the real output 1
    auto outResult1 = hOutResult.createInstance();
    auto rootGroup = hOutRootGroup.createInstance();
    auto childGroup = hOutChildGroup.createInstance();
    MySingularItem* childItem = hOutChildItem.createInstance();
    auto itemAttribute = hOutItemAttribute.createInstance(cat, false);

    outResult1->addRootGroup(hOutRootGroup, rootGroup);
    rootGroup->addGroup(hOutChildGroup, childGroup);
    childGroup->addSingularItem(hOutChildItem, childItem);
    childItem->addItemAttribute(hOutItemAttribute, itemAttribute);

    outResult1->setComplete();
    step1.addResult(outResult1);

    myDebug() << "--------- outResult 1 ---------";
    printResult(outResult1);

    QVERIFY(outResult1->model()->result() == outResult1);

    for(const CT_StandardItemGroup* rootG : *outResult1) {
        QVERIFY(rootG->model()->result() == outResult1);

        const bool resultOfModelOK = rootG->recursiveVisitChildrens([&outResult1](const CT_AbstractItem* child) -> bool {
            return ((child->model()->result() == NULL) || (child->model()->result() == outResult1));
        });

        QVERIFY(resultOfModelOK == true);
    }

    CT_HandleInResultGroupCopy<>::ResultType* outResult1Copied = NULL;

    // create the real output 2
    const auto visitor = [&outResult1Copied](const CT_OutAbstractResultModel* resultModel) -> bool {
        if(outResult1Copied != NULL)
            return false;

        CT_OutAbstractResultModel* rm = const_cast<CT_OutAbstractResultModel*>(resultModel);
        outResult1Copied = static_cast<CT_HandleInResultGroupCopy<>::ResultType*>(rm->createResult());
        return true;
    };

    QVERIFY(outManager2.visitResults(visitor) == true);
    QVERIFY(outResult1Copied != NULL);
    QVERIFY(outResult1Copied->nRootGroups() == 1);

    myDebug() << "--------- outResult 2 ---------";
    printResult(outResult1Copied);

    myDebug() << "--------- new item model ---------";
    myDebug() << hOutChildItem2.firstModel() << "with parent = [" << hOutChildItem2.firstModel()->parentModel() << "]";

    myDebug() << "--------- iterator ---------";

    // iterate over copies
    for(CT_ResultGroup* result : hInResultCopy.iterateOutputs()) {
        Q_UNUSED(result)
    }

    // iterate over inputs
    for(const CT_ResultGroup* result : hInResultCopy.iterateInputs()) {
        Q_UNUSED(result)
    }

    // iterate over copies
    // and add item to found groups in real output 2
    for(CT_StandardItemGroup* group : hInChildGroup.iterateOutputs(hInResultCopy)) {
        myDebug() << group->model() << " <- " << hOutChildItem2.firstModel();
        group->addSingularItem(hOutChildItem2, hOutChildItem2.createInstance());
    }

    int countChildItem = 0;

    // iterate over inputs
    // and add item to found groups in real output 2
    for(const CT_StandardItemGroup* group : hInChildGroup.iterateInputs(hInResultCopy)) {
        for(const CT_AbstractSingularItemDrawable* item : group->singularItems(hInChildItem)) {
            Q_UNUSED(item)
            ++countChildItem;
        }
    }

    QCOMPARE(countChildItem, 1);

    // verify the final structure
    myDebug() << "--------- outResult 2 (final) ---------";
    printResult(outResult1Copied);

    QVERIFY(outResult1Copied->model()->result() == outResult1Copied);

    for(const CT_StandardItemGroup* rootG : *outResult1Copied) {
        QVERIFY(rootG->result() == outResult1Copied);
        QVERIFY(rootG->model()->result() == outResult1Copied);

        const bool resultOfModelOK = rootG->recursiveVisitChildrens([&outResult1Copied, &outResult1, &hOutChildItem2](const CT_AbstractItem* child) -> bool {

            const CT_StandardItemGroup* group = dynamic_cast<const CT_StandardItemGroup*>(child);

            if(group || (child->model() == hOutChildItem2.firstModel()))
                return (child->model()->result() == outResult1Copied);

            return ((child->model()->result() == NULL) || (child->model()->result() == outResult1));
        });

        QVERIFY(resultOfModelOK == true);
    }

    outResult1Copied->setComplete();
    step2.addResult(outResult1Copied);
}

void TestStructure::testCompleteStructureCreationAndDoubleCopy()
{
    const CT_AbstractCategory* cat = PS_CATEGORY_MANAGER->findByUniqueName(CT_AbstractCategory::DATA_VALUE);
    MyStep* currentStep = NULL;
    CT_OutAbstractResultModel* resultModelFound = NULL;

    const auto visitorSetStep = [&currentStep, &resultModelFound](const CT_OutAbstractResultModel* resultModel) -> bool {
        resultModelFound = const_cast<CT_OutAbstractResultModel*>(resultModel);
        resultModelFound->setStep(currentStep);
        return true;
    };

    // out model 1
    MyStep step1;
    CT_HandleOutResultGroup hOutResult;
    CT_HandleOutStdGroup hOutRootGroup;
    CT_HandleOutStdGroup hOutChildGroup;
    CT_HandleOutSingularItem<MySingularItem> hOutChildItem;
    CT_HandleOutStdItemAttribute<bool> hOutItemAttribute;

    CT_OutModelStructureManager outManager;
    outManager.addResult(hOutResult);
    outManager.setRootGroup(hOutResult, hOutRootGroup);
    outManager.addGroup(hOutRootGroup, hOutChildGroup);
    outManager.addItem(hOutChildGroup, hOutChildItem);
    outManager.addItemAttribute(hOutChildItem, hOutItemAttribute, cat);

    currentStep = &step1;
    outManager.visitResults(visitorSetStep);

    // create the real output 1
    auto outResult1 = hOutResult.createInstance();
    auto rootGroup = hOutRootGroup.createInstance();
    auto childGroup = hOutChildGroup.createInstance();
    MySingularItem* childItem = hOutChildItem.createInstance();
    auto itemAttribute = hOutItemAttribute.createInstance(cat, false);

    outResult1->addRootGroup(hOutRootGroup, rootGroup);
    rootGroup->addGroup(hOutChildGroup, childGroup);
    childGroup->addSingularItem(hOutChildItem, childItem);
    childItem->addItemAttribute(hOutItemAttribute, itemAttribute);

    outResult1->setComplete();
    step1.addResult(outResult1);

    // in model 1
    CT_HandleInResultGroupCopy<> hInResultCopy;
    CT_HandleInStdZeroOrMoreGroup hInRootGroup;
    CT_HandleInStdGroup<> hInChildGroup;
    CT_HandleInSingularItem<CT_AbstractSingularItemDrawable> hInChildItem;

    CT_InModelStructureManager inManager;
    inManager.addResult(hInResultCopy);
    inManager.setZeroOrMoreRootGroup(hInResultCopy, hInRootGroup);
    inManager.addGroup(hInRootGroup, hInChildGroup);
    inManager.addItem(hInChildGroup, hInChildItem);

    hInResultCopy.model()->recursiveFindAllPossibilitiesInModel(*hOutResult.model());

    // select all possibilities
    hInResultCopy.model()->recursiveVisitPossibilities([](const CT_InAbstractModel*, const CT_InStdModelPossibility* possibility) -> bool {
        return const_cast<CT_InStdModelPossibility*>(possibility)->setSelected(true);
    });

    // out model 2 (copy of out model 1)
    MyStep step2;
    CT_HandleOutStdItemAttribute<float> hOutItemAttribute2;
    CT_OutModelStructureManager outManager2;
    outManager2.addResultCopy(hInResultCopy);
    outManager2.addItemAttribute(hInChildItem, hOutItemAttribute2, cat); // add an item attribute in the item

    resultModelFound = NULL;
    currentStep = &step2;
    outManager2.visitResults(visitorSetStep);

    CT_OutAbstractResultModelGroup* outResultModel2 = static_cast<CT_OutAbstractResultModelGroup*>(resultModelFound);

    // create the real output 2
    CT_HandleInResultGroupCopy<>::ResultType* outResult1Copied = NULL;
    outManager2.visitResults([&outResult1Copied](const CT_OutAbstractResultModel* resultModel) -> bool {
        if(outResult1Copied != NULL)
            return false;

        CT_OutAbstractResultModel* rm = const_cast<CT_OutAbstractResultModel*>(resultModel);
        outResult1Copied = static_cast<CT_HandleInResultGroupCopy<>::ResultType*>(rm->createResult());
        return true;
    });

    // add item attribute to found items in real output 1
    for(CT_AbstractSingularItemDrawable* item : hInChildItem.iterateOutputs(hInResultCopy)) {
        item->addItemAttribute(hOutItemAttribute2, hOutItemAttribute2.createInstance(cat, 2.4));
    }

    outResult1Copied->setComplete();
    step2.addResult(outResult1Copied);

    // in model 2
    CT_HandleInResultGroupCopy<> hInResultCopy2;
    CT_HandleInStdZeroOrMoreGroup hInRootGroup2;
    CT_HandleInStdGroup<> hInChildGroup2;
    CT_HandleInSingularItem<CT_AbstractSingularItemDrawable> hInChildItem2;

    CT_InModelStructureManager inManager2;
    inManager2.addResult(hInResultCopy2);
    inManager2.setZeroOrMoreRootGroup(hInResultCopy2, hInRootGroup2);
    inManager2.addGroup(hInRootGroup2, hInChildGroup2);
    inManager2.addItem(hInChildGroup2, hInChildItem2);

    hInResultCopy2.model()->recursiveFindAllPossibilitiesInModel(*outResultModel2);

    // select all possibilities
    hInResultCopy2.model()->recursiveVisitPossibilities([](const CT_InAbstractModel*, const CT_InStdModelPossibility* possibility) -> bool {
        return const_cast<CT_InStdModelPossibility*>(possibility)->setSelected(true);
    });

    // out model 3
    MyStep step3;
    CT_HandleOutStdItemAttribute<QString> hOutItemAttribute3;
    CT_OutModelStructureManager outManager3;
    outManager3.addResultCopy(hInResultCopy2);
    outManager3.addItemAttribute(hInChildItem2, hOutItemAttribute3, cat); // add an item attribute in items

    resultModelFound = NULL;
    currentStep = &step3;
    outManager3.visitResults(visitorSetStep);

    // create the real output 3
    CT_HandleInResultGroupCopy<>::ResultType* outResult2Copied = NULL;
    outManager3.visitResults([&outResult2Copied](const CT_OutAbstractResultModel* resultModel) -> bool {
        if(outResult2Copied != NULL)
            return false;

        CT_OutAbstractResultModel* rm = const_cast<CT_OutAbstractResultModel*>(resultModel);
        outResult2Copied = static_cast<CT_HandleInResultGroupCopy<>::ResultType*>(rm->createResult());
        return true;
    });

    // add item attribute to found items in real output 2
    for(CT_AbstractSingularItemDrawable* item : hInChildItem2.iterateOutputs(hInResultCopy2)) {
        item->addItemAttribute(hOutItemAttribute3, hOutItemAttribute3.createInstance(cat, "coucou"));
    }

    outResult2Copied->setComplete();
    step3.addResult(outResult2Copied);
}

QTEST_APPLESS_MAIN(TestStructure)

#include "tst_teststructure.moc"
