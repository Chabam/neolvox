#include <QString>
#include <QtTest>

#include "ct_cloudindex/registered/abstract/ct_abstractnotmodifiablecloudindexregisteredt.h"
#include "ct_colorcloud/registered/ct_standardcolorcloudregistered.h"
#include "ct_colorcloud/abstract/ct_abstractmodifiableindexcloudcolormap.h"
#include "ct_cloud/ct_bitcloud.h"
#include "ct_cloudindex/ct_cloudindexstdvectort.h"

#include "ct_iterator/ct_pointiterator.h"
#include "ct_iterator/ct_mutablepointiterator.h"
#include "ct_accessor/ct_pointaccessor.h"
#include "ct_accessibility/ct_iaccesspointcloud.h"

class TestClouds : public QObject
{
    Q_OBJECT

public:
    TestClouds();

private Q_SLOTS:
    void testBitCloud();
    void testPointCloudSimple();
    void testPointIterator();
    void testPointCloudRemoveMiddle();
    void testUndefinedSizePointCloudSimple();
    void testUndefinedSizePointCloudRemoveMiddle();
    void testCloudIndexSyncRemoveMiddle();
    void testCloudIndexVectorSyncRemoveMiddle();
    void testColorCloudSyncRemoveMiddle();
    void testMapColorCloudSyncRemoveMiddle();
    void benchmarkCloudIndexLoop();
    void benchmarkCloudIndexLoopDynamicCast();

private:
    CT_NMPCIR createPointCloud(size_t size, int initVar) const;
    void checkPointCloud(CT_NMPCIR pcir, int initVar, size_t size) const;
    void checkPointCloudIndex(CT_NMPCIR pcir, int initIndex, size_t size) const;
};

TestClouds::TestClouds()
{
}

void TestClouds::testBitCloud()
{
    CT_BitCloud bitCloud;

    QCOMPARE(bitCloud.size(), 0);
    QCOMPARE(bitCloud.m_numberOfBits, 0);
    QCOMPARE(bitCloud.m_collection.size(), 0);

    bitCloud.resize(16);

    QCOMPARE(bitCloud.size(), 16);
    QCOMPARE(bitCloud.m_numberOfBits, 16);
    QCOMPARE(bitCloud.m_collection.size(), 2);

    bitCloud.set(0, true);
    QCOMPARE(bitCloud.m_collection[0], 0x01);
    QCOMPARE(bitCloud.value(0), true);

    bitCloud.set(10, true);
    QCOMPARE(bitCloud.m_collection[1], 0x04);
    QCOMPARE(bitCloud.value(10), true);

    bitCloud.erase(0, 8);
    QCOMPARE(bitCloud.size(), 8);
    QCOMPARE(bitCloud.m_numberOfBits, 8);
    QCOMPARE(bitCloud.m_collection.size(), 1);
    QCOMPARE(bitCloud.value(2), true);

    bitCloud.resize(9);
    QCOMPARE(bitCloud.m_collection[0], 0x04);
    QCOMPARE(bitCloud.m_collection[1], 0x00);

    bitCloud.set(8, true);
    QCOMPARE(bitCloud.m_collection[1], 0x01);

    bitCloud.erase(0, 2);
    QCOMPARE(bitCloud.size(), 7);
    QCOMPARE(bitCloud.m_collection[0], 0x41);
    QCOMPARE(bitCloud.value(0), true);
    QCOMPARE(bitCloud.value(6), true);

    for(size_t i=1; i<32; ++i)
    {
        bitCloud.resize(32);
        bitCloud.fill(0);

        bitCloud.set(31, true);
        bitCloud.set(15, true);
        QVERIFY2(bitCloud.value(31) == true, QString("i == %1").arg(i).toUtf8().data());
        QVERIFY2(bitCloud.value(15) == true, QString("i == %1").arg(i).toUtf8().data());
        bitCloud.erase(0, i);
        QVERIFY2(bitCloud.value(31-i) == true, QString("i == %1").arg(i).toUtf8().data());

        if(i < 16)
        {
            QVERIFY2(bitCloud.value(15-i) == true, QString("i == %1").arg(i).toUtf8().data());
        }
    }

    for(size_t i=1; i<8; ++i)
    {
        bitCloud.resize(8);
        bitCloud.fill(0);
        bitCloud.set(7, true);
        bitCloud.erase(0, i);
        QVERIFY2(bitCloud.value(7-i) == true, QString("i == %1").arg(i).toUtf8().data());
    }

    for(size_t i=1; i<10; ++i)
    {
        bitCloud.resize(8);
        bitCloud.fill(0);
        bitCloud.set(0, true);
        bitCloud.set(7, true);
        bitCloud.erase(1, i);
        QVERIFY2(bitCloud.value(0) == true, QString("i == %1").arg(i).toUtf8().data());

        if(i < 8)
        {
            QVERIFY2(bitCloud.value(7-i) == true, QString("i == %1").arg(i).toUtf8().data());
            QVERIFY2(bitCloud.value(7) == false, QString("i == %1").arg(i).toUtf8().data());
        }
    }

    // accordeon
    // efface du bit 1 au bit i
    // |   32   |   24  |   16  |   8   |
    //  0100000000010000000001000000001x1
    //  100000000010000000001000000001xx1
    //  00000000010000000001000000001xxx1
    //  0000000010000000001000000001xxxx1
    //  000000010000000001000000001xxxxx1
    //  00000010000000001000000001xxxxxx1
    //  0000010000000001000000001xxxxxxx1
    //  000010000000001000000001xxxxxxxx1
    //  00010000000001000000001xxxxxxxxx1
    //  0010000000001000000001xxxxxxxxxx1
    //  ....
    //  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx1

    for(size_t i=1; i<32; ++i)
    {
        bitCloud.resize(32);
        bitCloud.fill(0);
        bitCloud.set(0, true);

        if(i < 31)
            bitCloud.set(i+1, true);

        if(i < 22)
            bitCloud.set(i+10, true);

        if(i < 12)
            bitCloud.set(i+20, true);

        if(i < 2)
            bitCloud.set(i+30, true);

        bitCloud.erase(1, i);

        for(size_t k=0; k<(32-i); ++k)
        {
            const bool indexMustBeTrue = (k == 0) || (k == 1) || (k == 10) || (k == 20) || (k == 30);
            QVERIFY2(bitCloud.value(k) == indexMustBeTrue, QString("(must be %1) i == %2 and k == %3").arg(indexMustBeTrue ? "true" : "false").arg(i).arg(k).toUtf8().data());
        }
    }

    // fenêtre glissante
    // efface du bit i au bit i+6
    // |   32   |   24  |   16  |   8   |
    //  00000010000000001000000001xxxxxx1
    //  0000010000000001000000001xxxxxx01
    //  000010000000001000000001xxxxxx001
    //  00010000000001000000001xxxxxx0001
    //  00010000000001000000001xxxxxx0001
    //  0010000000001000000001xxxxxx00001
    //  010000000001000000001xxxxxx000001
    //  ....
    //  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx1
    for(size_t i=1; i<26; ++i)
    {
        bitCloud.resize(32);
        bitCloud.fill(0);
        bitCloud.set(0, true);

        if(i < 26)
            bitCloud.set(i+6, true);

        if(i < 16)
            bitCloud.set(i+16, true);

        if(i < 6)
            bitCloud.set(i+26, true);

        bitCloud.erase(i, 6);

        for(size_t k=0; k<26; ++k)
        {
            const bool indexMustBeTrue = (k == 0) || (k == i) || (k == (i+20)) || (k == (i+10));
            QVERIFY2(bitCloud.value(k) == indexMustBeTrue, QString("(must be %1) i == %2 and k == %3").arg(indexMustBeTrue ? "true" : "false").arg(i).arg(k).toUtf8().data());
        }
    }

    // accordeon + fenêtre glissante = toutes les possibilités
    for(size_t j=1; j<32; ++j)
    {
        for(size_t i=1; i<(33-j); ++i)
        {
            quint32 valA = 1;

            bitCloud.resize(32);
            bitCloud.fill(0);

            bitCloud.set(0, true);

            bitCloud.set(j-1, true);
            valA |= (quint32(1) << (j-1));

            if((j+i)<32)
            {
                bitCloud.set(j+i, true);
                valA |= (quint32(1) << j);
            }

            if((j+i+5)<32)
            {
                bitCloud.set(j+i+5, true);
                valA |= (quint32(1) << (j+5));
            }

            if((j+i+8)<32)
            {
                bitCloud.set(j+i+8, true);
                valA |= (quint32(1) << (j+8));
            }

            if((j+i+13)<32)
            {
                bitCloud.set(j+i+13, true);
                valA |= (quint32(1) << (j+13));
            }

            if((j+i+18)<32)
            {
                bitCloud.set(j+i+18, true);
                valA |= (quint32(1) << (j+18));
            }

            bitCloud.erase(j, i);

            for(size_t k=0; k<(32-i); ++k)
            {
                const bool indexMustBeTrue = (k == 0) || (k == (j-1)) || (k == j) || (k == (j+5)) || (k == (j+8)) || (k == (j+13)) || (k == (j+18));
                QVERIFY2(bitCloud.value(k) == indexMustBeTrue, QString("(must be %1) i == %2 and j == %3 and k == %4").arg(indexMustBeTrue ? "true" : "false").arg(i).arg(j).arg(k).toUtf8().data());
            }
        }
    }
}

void TestClouds::testPointCloudSimple()
{
    CT_PointAccessor pAccess;
    QCOMPARE(pAccess.size(), (size_t)0);

    CT_NMPCIR pcir = createPointCloud(10, 0);

    QCOMPARE(pAccess.size(), (size_t)10);

    checkPointCloud(pcir, 0, 10);
    checkPointCloudIndex(pcir, 0, 10);

    pcir.clear();

    QCOMPARE(pAccess.size(), (size_t)0);
}

void TestClouds::testPointIterator()
{
    CT_PointAccessor pAccess;
    QCOMPARE(pAccess.size(), (size_t)0);

    CT_NMPCIR pcir = createPointCloud(1, 0);

    QCOMPARE(pAccess.size(), (size_t)1);

    checkPointCloud(pcir, 0, 1);
    checkPointCloudIndex(pcir, 0, 1);

    pcir.clear();

    QCOMPARE(pAccess.size(), (size_t)0);

    checkPointCloud(pcir, 0, 0);
    checkPointCloudIndex(pcir, 0, 0);
}

void TestClouds::testPointCloudRemoveMiddle()
{
    CT_PointAccessor pAccess;
    QCOMPARE(pAccess.size(), (size_t)0);

    CT_NMPCIR pcir = createPointCloud(10, 0);

    QCOMPARE(pAccess.size(), (size_t)10);

    CT_NMPCIR pcir2 = createPointCloud(20, 52);

    QCOMPARE(pAccess.size(), (size_t)30);

    CT_NMPCIR pcir3 = createPointCloud(5, 128);

    QCOMPARE(pAccess.size(), (size_t)35);

    checkPointCloud(pcir, 0, 10);
    checkPointCloud(pcir2, 52, 20);
    checkPointCloud(pcir3, 128, 5);

    checkPointCloudIndex(pcir, 0, 10);
    checkPointCloudIndex(pcir2, 10, 20);
    checkPointCloudIndex(pcir3, 30, 5);

    pcir2.clear();

    QCOMPARE(pAccess.size(), (size_t)15);

    checkPointCloud(pcir, 0, 10);
    checkPointCloud(pcir3, 128, 5);

    checkPointCloudIndex(pcir, 0, 10);
    checkPointCloudIndex(pcir3, 10, 5);

    pcir.clear();

    QCOMPARE(pAccess.size(), (size_t)5);

    checkPointCloud(pcir3, 128, 5);
    checkPointCloudIndex(pcir3, 0, 5);

    pcir3.clear();

    QCOMPARE(pAccess.size(), (size_t)0);
}

void TestClouds::testUndefinedSizePointCloudSimple()
{
    CT_AbstractUndefinedSizePointCloud *pc = PS_REPOSITORY->createNewUndefinedSizePointCloud();

    CT_PointAccessor pAccess;
    QCOMPARE(pAccess.size(), (size_t)0);

    for(int i=0; i<10; ++i)
    {
        CT_Point p;
        p.setX(i);
        p.setY(i);
        p.setZ(i);

        pc->addPoint(p);
    }

    CT_NMPCIR pcir = PS_REPOSITORY->registerUndefinedSizePointCloud(pc);

    checkPointCloud(pcir, 0, 10);
    checkPointCloudIndex(pcir, 0, 10);

    QCOMPARE(pAccess.size(), (size_t)10);
}

void TestClouds::testUndefinedSizePointCloudRemoveMiddle()
{
    CT_PointAccessor pAccess;
    QCOMPARE(pAccess.size(), (size_t)0);

    CT_NMPCIR pcir = createPointCloud(10, 0);

    CT_AbstractUndefinedSizePointCloud *pc = PS_REPOSITORY->createNewUndefinedSizePointCloud();

    QCOMPARE(pAccess.size(), (size_t)10);

    for(int i=13; i<33; ++i)
    {
        CT_Point p;
        p.setX(i);
        p.setY(i);
        p.setZ(i);

        pc->addPoint(p);
    }

    CT_NMPCIR pcir2 = PS_REPOSITORY->registerUndefinedSizePointCloud(pc);

    QCOMPARE(pAccess.size(), (size_t)30);

    CT_NMPCIR pcir3 = createPointCloud(5, 85);

    QCOMPARE(pAccess.size(), (size_t)35);

    checkPointCloud(pcir, 0, 10);
    checkPointCloud(pcir2, 13, 20);
    checkPointCloud(pcir3, 85, 5);

    checkPointCloudIndex(pcir, 0, 10);
    checkPointCloudIndex(pcir2, 10, 20);
    checkPointCloudIndex(pcir3, 30, 5);

    pcir2.clear();

    QCOMPARE(pAccess.size(), (size_t)15);

    checkPointCloud(pcir, 0, 10);
    checkPointCloud(pcir3, 85, 5);

    checkPointCloudIndex(pcir, 0, 10);
    checkPointCloudIndex(pcir3, 10, 5);

    pcir3.clear();

    QCOMPARE(pAccess.size(), (size_t)10);

    checkPointCloud(pcir, 0, 10);
    checkPointCloudIndex(pcir, 0, 10);

    pcir.clear();

    QCOMPARE(pAccess.size(), (size_t)0);
}

void TestClouds::testCloudIndexSyncRemoveMiddle()
{
    CT_PointAccessor pAccess;
    QCOMPARE(pAccess.size(), (size_t)0);

    CT_NMPCIR pcir = createPointCloud(8, 0);
    CT_NMPCIR pcir2 = createPointCloud(26, 21);
    CT_NMPCIR pcir3 = createPointCloud(42, 58);

    CT_MCIR cir = PS_REPOSITORY->createNewIndexCloud(CT_Repository::SyncWithPointCloud);

    cir->abstractModifiableCloudIndex()->addIndex(4); // pcir
    cir->abstractModifiableCloudIndex()->addIndex(5); // pcir
    cir->abstractModifiableCloudIndex()->addIndex(11); // pcir2
    cir->abstractModifiableCloudIndex()->addIndex(14); // pcir2
    cir->abstractModifiableCloudIndex()->addIndex(50); // pcir3
    cir->abstractModifiableCloudIndex()->addIndex(37); // pcir3
    cir->abstractModifiableCloudIndex()->addIndex(45); // pcir3

    QCOMPARE(cir->abstractModifiableCloudIndex()->size(), (size_t)7);
    QCOMPARE(cir->abstractModifiableCloudIndex()->constIndexAt(0), (ct_index_type)4);
    QCOMPARE(cir->abstractModifiableCloudIndex()->constIndexAt(1), (ct_index_type)5);
    QCOMPARE(cir->abstractModifiableCloudIndex()->constIndexAt(2), (ct_index_type)11);
    QCOMPARE(cir->abstractModifiableCloudIndex()->constIndexAt(3), (ct_index_type)14);
    QCOMPARE(cir->abstractModifiableCloudIndex()->constIndexAt(4), (ct_index_type)37);
    QCOMPARE(cir->abstractModifiableCloudIndex()->constIndexAt(5), (ct_index_type)45);
    QCOMPARE(cir->abstractModifiableCloudIndex()->constIndexAt(6), (ct_index_type)50);

    pcir2.clear();

    QCOMPARE(cir->abstractModifiableCloudIndex()->size(), (size_t)5);
    QCOMPARE(cir->abstractModifiableCloudIndex()->constIndexAt(0), (ct_index_type)4);
    QCOMPARE(cir->abstractModifiableCloudIndex()->constIndexAt(1), (ct_index_type)5);
    QCOMPARE(cir->abstractModifiableCloudIndex()->constIndexAt(2), (ct_index_type)11);
    QCOMPARE(cir->abstractModifiableCloudIndex()->constIndexAt(3), (ct_index_type)19);
    QCOMPARE(cir->abstractModifiableCloudIndex()->constIndexAt(4), (ct_index_type)24);

    pcir3.clear();

    QCOMPARE(cir->abstractModifiableCloudIndex()->size(), (size_t)2);
    QCOMPARE(cir->abstractModifiableCloudIndex()->constIndexAt(0), (ct_index_type)4);
    QCOMPARE(cir->abstractModifiableCloudIndex()->constIndexAt(1), (ct_index_type)5);

    pcir.clear();

    QCOMPARE(cir->abstractModifiableCloudIndex()->size(), (size_t)0);
}

void TestClouds::testCloudIndexVectorSyncRemoveMiddle()
{
    CT_PointAccessor pAccess;
    QCOMPARE(pAccess.size(), (size_t)0);

    CT_NMPCIR pcir = createPointCloud(8, 0);
    CT_NMPCIR pcir2 = createPointCloud(26, 21);
    CT_NMPCIR pcir3 = createPointCloud(42, 58);

    CT_MCIR cir = PS_REPOSITORY->createNewIndexCloudT< CT_CloudIndexStdVectorT >(CT_Repository::SyncWithPointCloud);

    cir->abstractModifiableCloudIndex()->addIndex(4); // pcir
    cir->abstractModifiableCloudIndex()->addIndex(5); // pcir
    cir->abstractModifiableCloudIndex()->addIndex(11); // pcir2
    cir->abstractModifiableCloudIndex()->addIndex(14); // pcir2
    cir->abstractModifiableCloudIndex()->addIndex(50); // pcir3
    cir->abstractModifiableCloudIndex()->addIndex(37); // pcir3
    cir->abstractModifiableCloudIndex()->addIndex(45); // pcir3

    QCOMPARE(cir->abstractModifiableCloudIndex()->size(), (size_t)7);
    QCOMPARE(cir->abstractModifiableCloudIndex()->constIndexAt(0), (ct_index_type)4);
    QCOMPARE(cir->abstractModifiableCloudIndex()->constIndexAt(1), (ct_index_type)5);
    QCOMPARE(cir->abstractModifiableCloudIndex()->constIndexAt(2), (ct_index_type)11);
    QCOMPARE(cir->abstractModifiableCloudIndex()->constIndexAt(3), (ct_index_type)14);
    QCOMPARE(cir->abstractModifiableCloudIndex()->constIndexAt(4), (ct_index_type)37);
    QCOMPARE(cir->abstractModifiableCloudIndex()->constIndexAt(5), (ct_index_type)45);
    QCOMPARE(cir->abstractModifiableCloudIndex()->constIndexAt(6), (ct_index_type)50);

    pcir2.clear();

    QCOMPARE(cir->abstractModifiableCloudIndex()->size(), (size_t)5);
    QCOMPARE(cir->abstractModifiableCloudIndex()->constIndexAt(0), (ct_index_type)4);
    QCOMPARE(cir->abstractModifiableCloudIndex()->constIndexAt(1), (ct_index_type)5);
    QCOMPARE(cir->abstractModifiableCloudIndex()->constIndexAt(2), (ct_index_type)11);
    QCOMPARE(cir->abstractModifiableCloudIndex()->constIndexAt(3), (ct_index_type)19);
    QCOMPARE(cir->abstractModifiableCloudIndex()->constIndexAt(4), (ct_index_type)24);

    pcir3.clear();

    QCOMPARE(cir->abstractModifiableCloudIndex()->size(), (size_t)2);
    QCOMPARE(cir->abstractModifiableCloudIndex()->constIndexAt(0), (ct_index_type)4);
    QCOMPARE(cir->abstractModifiableCloudIndex()->constIndexAt(1), (ct_index_type)5);

    pcir.clear();

    QCOMPARE(cir->abstractModifiableCloudIndex()->size(), (size_t)0);
}

void TestClouds::testColorCloudSyncRemoveMiddle()
{
    CT_NMPCIR pcir = createPointCloud(14, 0);
    CT_NMPCIR pcir2 = createPointCloud(80, 10);
    CT_NMPCIR pcir3 = createPointCloud(10, 62);

    CT_CCR ccr = PS_REPOSITORY->createNewColorCloud(CT_Repository::SyncWithPointCloud);

    for(int i=0; i<104; ++i)
    {
        CT_Color &col = ccr->abstractColorCloud()->colorAt(i);
        col(0) = i;
        col(1) = i;
        col(2) = i;
        col(3) = i;
    }

    pcir2.clear();

    QCOMPARE(ccr->abstractColorCloud()->size(), (size_t)24);

    for(size_t i=0; i<14; ++i)
    {
        CT_Color &col = ccr->abstractColorCloud()->colorAt(i);
        QCOMPARE(col(0), (CT_Color::value_type)i);
        QCOMPARE(col(1), (CT_Color::value_type)i);
        QCOMPARE(col(2), (CT_Color::value_type)i);
        QCOMPARE(col(3), (CT_Color::value_type)i);
    }

    for(size_t i=14; i<24; ++i)
    {
        CT_Color &col = ccr->abstractColorCloud()->colorAt(i);
        QCOMPARE(col(0), (CT_Color::value_type)(i+80));
        QCOMPARE(col(1), (CT_Color::value_type)(i+80));
        QCOMPARE(col(2), (CT_Color::value_type)(i+80));
        QCOMPARE(col(3), (CT_Color::value_type)(i+80));
    }
}

void TestClouds::testMapColorCloudSyncRemoveMiddle()
{
    CT_MCIR mccr = PS_REPOSITORY->createNewMapIndexCloudColor(CT_Repository::SyncWithPointCloud);

    CT_NMPCIR pcir = createPointCloud(10, 0);
    CT_NMPCIR pcir2 = createPointCloud(20, 10);
    CT_NMPCIR pcir3 = createPointCloud(10, 30);

    QCOMPARE(mccr->abstractModifiableCloudIndex()->size(), (size_t)0);

    CT_Color col;
    CT_AbstractModifiableIndexCloudColorMap *map = dynamic_cast<CT_AbstractModifiableIndexCloudColorMap*>(mccr->abstractModifiableCloudIndex());
    map->insertIndexAndColor(0, col);
    map->insertIndexAndColor(35, col);
    map->insertIndexAndColor(22, col);
    map->insertIndexAndColor(14, col);
    map->insertIndexAndColor(3, col);

    QCOMPARE(mccr->abstractModifiableCloudIndex()->size(), (size_t)5);
    QCOMPARE(mccr->abstractModifiableCloudIndex()->constIndexAt(0), (ct_index_type)0);
    QCOMPARE(mccr->abstractModifiableCloudIndex()->constIndexAt(1), (ct_index_type)3);
    QCOMPARE(mccr->abstractModifiableCloudIndex()->constIndexAt(2), (ct_index_type)14);
    QCOMPARE(mccr->abstractModifiableCloudIndex()->constIndexAt(3), (ct_index_type)22);
    QCOMPARE(mccr->abstractModifiableCloudIndex()->constIndexAt(4), (ct_index_type)35);

    pcir2.clear();

    QCOMPARE(mccr->abstractModifiableCloudIndex()->size(), (size_t)3);
    QCOMPARE(mccr->abstractModifiableCloudIndex()->constIndexAt(0), (ct_index_type)0);
    QCOMPARE(mccr->abstractModifiableCloudIndex()->constIndexAt(1), (ct_index_type)3);
    QCOMPARE(mccr->abstractModifiableCloudIndex()->constIndexAt(2), (ct_index_type)15);

    pcir.clear();

    QCOMPARE(mccr->abstractModifiableCloudIndex()->size(), (size_t)1);
    QCOMPARE(mccr->abstractModifiableCloudIndex()->constIndexAt(0), (ct_index_type)5);
}

void TestClouds::benchmarkCloudIndexLoop()
{
    CT_NMPCIR pcir = createPointCloud(15000, 0);
    CT_MCIR cir = PS_REPOSITORY->createNewIndexCloud(CT_Repository::SyncWithPointCloud);

    QBENCHMARK {
        CT_AbstractModifiableCloudIndex *index = cir->abstractModifiableCloudIndex();

        size_t size = index->size();

        for(size_t i=0; i<size; ++i) {
            index->constIndexAt(i);
        }
    }
}

void TestClouds::benchmarkCloudIndexLoopDynamicCast()
{
    CT_NMPCIR pcir = createPointCloud(15000, 0);
    CT_MCIR cir = PS_REPOSITORY->createNewIndexCloud(CT_Repository::SyncWithPointCloud);

    QBENCHMARK {
        const CT_AbstractPointCloudIndex *index = dynamic_cast<const CT_AbstractPointCloudIndex*>(cir->abstractModifiableCloudIndex());

        size_t size = index->size();

        for(size_t i=0; i<size; ++i) {
            index->constIndexAt(i);
        }
    }
}

CT_NMPCIR TestClouds::createPointCloud(size_t size, int initVar) const
{
    CT_NMPCIR pcir = PS_REPOSITORY->createNewPointCloud(size);

    CT_MutablePointIterator it(pcir);

    double i = initVar;

    while(it.hasNext())
    {
        CT_Point p;
        p.setX(i);
        p.setY(i);
        p.setZ(i);

        it.next().replaceCurrentPoint(p);
        ++i;
    }

    return pcir;
}

void TestClouds::checkPointCloud(CT_NMPCIR pcir, int initVar, size_t size) const
{
    size_t n = 0;

    CT_PointIterator it(pcir);

    double i = initVar;

    while(it.hasNext())
    {
        const CT_Point &p = it.next().cT();
        QCOMPARE(p(CT_Point::X), i);
        QCOMPARE(p(CT_Point::Y), i);
        QCOMPARE(p(CT_Point::Z), i);

        ++i;
        ++n;
    }

    QCOMPARE(n, size);
}

void TestClouds::checkPointCloudIndex(CT_NMPCIR pcir, int initIndex, size_t size) const
{
    size_t n = 0;

    CT_PointIterator it(pcir);

    size_t i = initIndex;

    while(it.hasNext())
    {
        QCOMPARE(it.next().cIndex(), i);

        ++i;
        ++n;
    }

    QCOMPARE(n, size);
}

QTEST_APPLESS_MAIN(TestClouds)

#include "tst_testclouds.moc"
