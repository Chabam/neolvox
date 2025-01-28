#ifndef CT_GRID3DWOOTRAVERSALALGORITHM_H
#define CT_GRID3DWOOTRAVERSALALGORITHM_H

#include "ct_itemdrawable/ct_beam.h"
#include "ct_abstractgrid3dbeamvisitor.h"
#include "ct_itemdrawable/abstract/ct_abstractgrid3d.h"

/*! \class  CT_Grid3DWooTraversalAlgorithm
    \brief  A ray traversal algorithm developped by Amanatides & Woo in 1987

    The article presenting this algorithm can be found at : http://www.cse.yorku.ca/~amana/research/grid.pdf

    Here is the BibTeX citation of the article :
    @inproceedings{ amanatides1987fast,
                    title={A fast voxel traversal algorithm for ray tracing},
                    author={Amanatides, J. and Woo, A.},
                    booktitle={Proceedings of EUROGRAPHICS},
                    volume={87},
                    pages={3--10},
                    year={1987}
                    }

*/
class CTLIBSTRUCTUREADDON_EXPORT CT_Grid3DWooTraversalAlgorithm
{
public:

    CT_Grid3DWooTraversalAlgorithm();
    /*!
    *  \brief Constructor
    *
    *  Constructor of the class
    *
    */
    CT_Grid3DWooTraversalAlgorithm(const CT_AbstractGrid3D* grid, bool keepFirst, QList<CT_AbstractGrid3DBeamVisitor* > &list);

    CT_Grid3DWooTraversalAlgorithm(const CT_AbstractGrid3D *grid, bool keepFirst); // version sans visitors (ne pas utiliser Compute)


    /*!
     * \brief compute method of the algorithm
     */
    void compute(CT_Beam &data, Eigen::Vector3d* endPoint = nullptr);

    // Pour utilisation pas à pas, sans visitors
    bool init(CT_Beam &data, size_t returnedIndex);
    bool getNextIndex(size_t returnedIndex);

private:
    const CT_AbstractGrid3D*                _calibrationGrid; /*!< Calibration grid */
    Eigen::Vector3d                         _gridBottom;      /*!< bottom left coordinates of the calibration grid*/
    Eigen::Vector3d                         _gridTop;         /*!< upper right coordinates of the calibration grid*/
    double                                   _gridRes;         /*!< Resolution of the calibration grid*/
    bool                                    _keepFirst;       /*! Should be te cell containing the beam origin be kept*/
    QList<CT_AbstractGrid3DBeamVisitor* >   _visitorList;
    int                                     _numberOfVisitors;

    // Working variables
    Eigen::Vector3d     _start;
    Eigen::Vector3d     _end;
    Eigen::Vector3d     _boundary;
    int                 _currentCol;   /*!< current voxel column all along the algorithm (grid coordinate system)*/
    int                 _currentLin;   /*!< current voxel row all along the algorithm (grid coordinate system)*/
    int                 _currentLevz;  /*!< current voxel z level all along the algorithm (grid coordinate system)*/
    Eigen::Vector3d     _stepAxis;     /*!< indicates for each axis wether the ray goes forward (in the same direction than the base vector => 1) or backward (the opposite direction => -1)*/
    Eigen::Vector3d     _tMax;         /*!< "the value of t at which the ray crosses the first voxel boundary (along each direction)"*/
    Eigen::Vector3d     _tDel;         /*!< "how far along the ray we must move (in units of t)" for each component "of such a movement to equal the width of a voxel"*/
    bool        _intersects;   /*!< When creating or resetting the algorithm, this value indicates wether the ray intersects the box or not. If not, the run method won't do anything.*/
    int         _chooseAxis[8];
    int         _nextStepAxis;

    int                 _endPtCol;
    int                 _endPtLin;
    int                 _endPtLevz;




};

#endif // CT_GRID3DWOOTRAVERSALALGORITHM_H
