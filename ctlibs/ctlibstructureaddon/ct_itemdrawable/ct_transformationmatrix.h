#ifndef CT_TRANSFORMATIONMATRIX_H
#define CT_TRANSFORMATIONMATRIX_H

#include "ct_itemdrawable/abstract/ct_abstractsingularitemdrawable.h"
#include "ctlibstructureaddon_global.h"

#include <Eigen/Core>

/**
 * @brief A singular item that contains item attributes
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_TransformationMatrix : public CT_AbstractSingularItemDrawable
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_TransformationMatrix, CT_AbstractSingularItemDrawable, Transformation matrix)

    using SuperClass = CT_AbstractSingularItemDrawable;

public:
    CT_TransformationMatrix();
    CT_TransformationMatrix(const Eigen::Matrix4d& trMat);
    CT_TransformationMatrix(const CT_TransformationMatrix& other) = default;

    inline const Eigen::Matrix4d& getTransformationMatrix() const {return _transformationMatrix;}
    QString getTransformationMatrixAsString() const;

    void transform(Eigen::Vector3d &vec) const;
    Eigen::Vector3d getTransformed(const Eigen::Vector3d &vec) const;
    Eigen::Matrix4d getTransformed(Eigen::Matrix4d &mat) const;

    CT_ITEM_COPY_IMP(CT_TransformationMatrix)

protected:
    Eigen::Matrix4d     _transformationMatrix;

private:

    CT_DEFAULT_IA_BEGIN(CT_TransformationMatrix)
    CT_DEFAULT_IA_V2(CT_TransformationMatrix, CT_AbstractCategory::staticInitDataValue(), &CT_TransformationMatrix::getTransformationMatrixAsString, QObject::tr("Matrice de transformation"))
    CT_DEFAULT_IA_END(CT_TransformationMatrix)

};

#endif // CT_TRANSFORMATIONMATRIX_H
