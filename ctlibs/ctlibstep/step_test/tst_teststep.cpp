#include <QString>
#include <QtTest>
#include <QCoreApplication>

#include "steptesteasy.h"
#include "steptestinputoutput.h"

class TestStep : public QObject
{
    Q_OBJECT

public:
    TestStep();

private Q_SLOTS:
    void testCaseOutput();
    void testCaseInputOutput();
};

TestStep::TestStep()
{
}

void TestStep::testCaseOutput()
{
    STepTestEasy step;
    step.init();
    step.showPreInputConfigurationDialog();
    step.showInputResultConfigurationDialog();
    step.showPostConfigurationDialog();
    step.finalizeConfiguration();

    step.execute();
}

void TestStep::testCaseInputOutput()
{
    STepTestEasy step;
    step.init();
    QVERIFY(step.showPreInputConfigurationDialog() == true);
    QVERIFY(step.showInputResultConfigurationDialog() == true);
    QVERIFY(step.showPostConfigurationDialog() == true);
    QVERIFY(step.finalizeConfiguration() == true);

    StepTestInputOutput* step2 = new StepTestInputOutput();
    step2->setParentStep(&step);
    QVERIFY(step.appendStep(step2) == true);

    step2->init();
    QVERIFY(step2->showPreInputConfigurationDialog() == true);
    QVERIFY(step2->showInputResultConfigurationDialog() == true);
    QVERIFY(step2->showPostConfigurationDialog() == true);
    QVERIFY(step2->finalizeConfiguration() == true);

    //step2->showInputResultConfigurationDialog();

    StepTestInputOutput* step3 = new StepTestInputOutput();
    step3->setParentStep(step2);
    QVERIFY(step2->appendStep(step3) == true);

    step3->init();
    QVERIFY(step3->showPreInputConfigurationDialog() == true);
    QVERIFY(step3->showInputResultConfigurationDialog() == true);
    QVERIFY(step3->showPostConfigurationDialog() == true);
    QVERIFY(step3->finalizeConfiguration() == true);

    //step3->showInputResultConfigurationDialog();

    step.execute();
    step2->execute();
    step3->execute();

    QVERIFY(step.m_outputItemsDisplayableName.isEmpty() == false);
    QVERIFY(step.m_outputGroupsDisplayableName.isEmpty() == false);
    QVERIFY(step.m_outputGroupsDisplayableName == step2->m_inputGroupsDisplayableName);
    QVERIFY(step.m_outputItemsDisplayableName == step2->m_inputItemsDisplayableName);
    QVERIFY(step2->m_outputGroupsDisplayableName == step3->m_inputGroupsDisplayableName);
    QVERIFY(step2->m_outputItemsDisplayableName == step3->m_inputItemsDisplayableName);
}

QTEST_MAIN(TestStep)

#include "tst_teststep.moc"
