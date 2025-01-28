#include "ct_houghtransform.h"
#define _USE_MATH_DEFINES
#include "math.h"
#include "ct_math/ct_sphericalline3d.h"


void  CT_HoughTransform::sortHoughtObjects(QList<CT_HoughTransform::HoughObject*> &objects, double minDist, QMultiMap<double, CT_HoughTransform::HoughObject*> &scoreMap)
{
    for (int i = 0 ; i < objects.size() ; i++)
    {
        CT_HoughTransform::HoughObject* object1 = objects.at(i);

        double score = 0;
        for (int j = 0 ; j < objects.size() ; j++)
        {
            if (j != i)
            {
                CT_HoughTransform::HoughObject* object2 = objects.at(j);

                double dist = 0;
                for (int c = 0 ; c < object1->size() ; c++)
                {
                    dist += pow((*object1)(c) - (*object2)(c), 2);
                }
                dist = sqrt(dist);

                double ratio = minDist / dist;
                if (ratio > 1) {ratio = 1;}
                score += ratio;
            }
        }
        scoreMap.insert(score, object1);
    }
}

void CT_HoughTransform::getHoughtDistances(QList<CT_HoughTransform::HoughObject *> &objects, double maxDist, QMultiMap<double, QPair<CT_HoughTransform::HoughObject *, CT_HoughTransform::HoughObject *> *> &outDistMap)
{
    for (int i = 0 ; i < objects.size() ; i++)
    {
        CT_HoughTransform::HoughObject* object1 = objects.at(i);

        for (int j = 0 ; j < objects.size() ; j++)
        {
            if (j != i)
            {
                CT_HoughTransform::HoughObject* object2 = objects.at(j);

                double dist = 0;
                for (int c = 0 ; c < object1->size() ; c++)
                {
                    dist += pow((*object1)(c) - (*object2)(c), 2);
                }
                dist = sqrt(dist);

                if (dist < maxDist)
                {
                    outDistMap.insert(dist, new QPair<CT_HoughTransform::HoughObject*, CT_HoughTransform::HoughObject*> (object1, object2));
                }
            }
        }
    }
}

double CT_HoughTransform::findBestHoughtObjects(QList<CT_HoughTransform::HoughObject *> &objects, double minDist, CT_HoughTransform::HoughObject* bestObject)
{
    Q_UNUSED(bestObject);

    bestObject = nullptr;
    double bestScore = 0;

    for (int i = 0 ; i < objects.size() ; i++)
    {
        CT_HoughTransform::HoughObject* object1 = objects.at(i);

        double score = 0;
        for (int j = 0 ; j < objects.size() ; j++)
        {
            if (j != i)
            {
                CT_HoughTransform::HoughObject* object2 = objects.at(j);

                double dist = 0;
                for (int c = 0 ; c < object1->size() ; c++)
                {
                    dist += pow((*object1)(c) - (*object2)(c), 2);
                }
                dist = sqrt(dist);

                double ratio = minDist / dist;
                if (ratio > 1) {ratio = 1;}
                score += ratio;
            }
        }

        if (score > bestScore)
        {
            bestScore = score;
            bestObject = object1;
        }
    }
    return bestScore;
}

void CT_HoughTransform::createHoughObjectsFromVerticalLines(QList<CT_LineData *> &lines, double referenceHeight, QList<CT_HoughTransform::HoughObject *> &outObjects)
{
    for (int i = 0 ; i < lines.size() ; i++)
    {
        CT_LineData* line = lines.at(i);

        const Eigen::Vector3d &pLow = line->getP1();

        Eigen::Vector3d dir = line->getDirection();
        dir.normalize();

        double t1 = 0;
        double t2 = 0;
        if (dir(2) != 0)
        {
            t1 = (referenceHeight - pLow(2)) / dir(2);
            t2 = (referenceHeight + 1.0 - pLow(2)) / dir(2);
        }

        QVector<double> coords(4);

        coords[0] = pLow(0) + t1*dir(0);
        coords[1] = pLow(1) + t1*dir(1);
        coords[3] = pLow(0) + t2*dir(0);
        coords[4] = pLow(1) + t2*dir(1);

        outObjects.append(new CT_HoughTransform::HoughObject(coords, line));
    }
}

void CT_HoughTransform::createHoughObjectsFromHorizontalCircles(QList<CT_Circle2DData *> &circles, double radiusWeight, QList<CT_HoughTransform::HoughObject *> &outObjects)
{
    for (int i = 0 ; i < circles.size() ; i++)
    {
        CT_Circle2DData* circle = circles.at(i);
        const Eigen::Vector2d &center = circle->getCenter();

        QVector<double> coords(3);
        coords[0] = center(0);
        coords[1] = center(1);
        coords[2] = radiusWeight*circle->getRadius();

        outObjects.append(new CT_HoughTransform::HoughObject(coords, circle));
    }
}
