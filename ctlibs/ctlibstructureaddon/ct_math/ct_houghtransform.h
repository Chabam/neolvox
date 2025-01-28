#ifndef CT_HOUGHTRANSFORM_H
#define CT_HOUGHTRANSFORM_H

#include "ct_shapedata/ct_linedata.h"
#include "ct_shape2ddata/ct_circle2ddata.h"

#include <Eigen/Core>

#include <QMap>
#include <QVector>

class CTLIBSTRUCTUREADDON_EXPORT CT_HoughTransform
{
public:

    struct HoughObject
    {
        HoughObject(const QVector<double>& coords, void* object) :
            _houghCoords(coords),
            _object(object)
        {
        }

        double operator()(int idx) const
        {
            if (idx < 0 || idx >= _houghCoords.size()) {return 0;}
            return _houghCoords[idx];
        }

        double& operator[](int idx)
        {
            return _houghCoords[idx];
        }

        int size() const {return _houghCoords.size();}

        QVector<double> _houghCoords;
        void*           _object;
    };


    static void sortHoughtObjects(QList<CT_HoughTransform::HoughObject*> &objects, double minDist, QMultiMap<double, CT_HoughTransform::HoughObject*> &scoreMap);

    static void getHoughtDistances(QList<CT_HoughTransform::HoughObject *> &objects, double maxDist, QMultiMap<double, QPair<CT_HoughTransform::HoughObject*, CT_HoughTransform::HoughObject*>* > &outDistMap);

    static double findBestHoughtObjects(QList<CT_HoughTransform::HoughObject *> &objects, double minDist, CT_HoughTransform::HoughObject* bestObject);

    static void createHoughObjectsFromVerticalLines(QList<CT_LineData*> &lines, double referenceHeight, QList<CT_HoughTransform::HoughObject*> &outObjects);

    static void createHoughObjectsFromHorizontalCircles(QList<CT_Circle2DData*> &circles, double radiusWeight, QList<CT_HoughTransform::HoughObject*> &outObjects);

};

#endif // CT_HOUGHTRANSFORM_H
