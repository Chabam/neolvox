#ifndef CT_INDEXABLEPOINTSREADER_H
#define CT_INDEXABLEPOINTSREADER_H

#include "ctlibreader_global.h"
#include "ct_reader/abstract/ct_abstractreader.h"
#include "ct_shape2ddata/ct_areashape2ddata.h"

/**
 * Class that offer the points filtering extension to your reader
 */
class CTLIBREADER_EXPORT CT_IndexablePointsReader
{

public:

    class CandidateShape2D {
    public:

        CandidateShape2D (const CT_AreaShape2DData* area2D)
        {
            _area2D = area2D;
            _all = true;
            _asPointsInside = true;
            _lastIncludedIndex = -1;
        }

        const CT_AreaShape2DData*   _area2D;
        bool                        _asPointsInside;
        bool                        _all;
        qint64                      _lastIncludedIndex;
        std::list<qint64>         _indicesAfterLastIncludedIndex;
    };

    virtual void getPointIndicesInside2DShape(QList<CandidateShape2D> &candidateShapes) const = 0;

    virtual QString getFormatCode() const = 0;



};

#endif // CT_INDEXABLEPOINTSREADER_H
