#include "ct_transformationmatrix.h"

#include <Eigen/Geometry>

CT_DEFAULT_IA_INIT(CT_TransformationMatrix)

CT_TransformationMatrix::CT_TransformationMatrix() : SuperClass()
{
    _transformationMatrix = Eigen::Matrix4d::Identity(4,4);
}

CT_TransformationMatrix::CT_TransformationMatrix(const Eigen::Matrix4d &trMat) : SuperClass()
{
    _transformationMatrix = trMat;
}

QString CT_TransformationMatrix::getTransformationMatrixAsString() const
{
    return QString("%1\t%2\t%3\t%4\n%5\t%6\t%7\t%8\n%9\t%10\t%11\t%12\n%13\t%14\t%15\t%16\n")
                                                            .arg(QString::number(_transformationMatrix(0,0), 'f', 10))
                                                            .arg(QString::number(_transformationMatrix(0,1), 'f', 10))
                                                            .arg(QString::number(_transformationMatrix(0,2), 'f', 10))
                                                            .arg(QString::number(_transformationMatrix(0,3), 'f', 10))
                                                            .arg(QString::number(_transformationMatrix(1,0), 'f', 10))
                                                            .arg(QString::number(_transformationMatrix(1,1), 'f', 10))
                                                            .arg(QString::number(_transformationMatrix(1,2), 'f', 10))
                                                            .arg(QString::number(_transformationMatrix(1,3), 'f', 10))
                                                            .arg(QString::number(_transformationMatrix(2,0), 'f', 10))
                                                            .arg(QString::number(_transformationMatrix(2,1), 'f', 10))
                                                            .arg(QString::number(_transformationMatrix(2,2), 'f', 10))
                                                            .arg(QString::number(_transformationMatrix(2,3), 'f', 10))
                                                            .arg(QString::number(_transformationMatrix(3,0), 'f', 10))
                                                            .arg(QString::number(_transformationMatrix(3,1), 'f', 10))
                                                            .arg(QString::number(_transformationMatrix(3,2), 'f', 10))
                                                            .arg(QString::number(_transformationMatrix(3,3), 'f', 10));
}

void CT_TransformationMatrix::transform(Eigen::Vector3d &vec) const
{
    Eigen::Vector4d tmp = _transformationMatrix*(vec.homogeneous());
    vec = tmp.hnormalized();
}

Eigen::Vector3d CT_TransformationMatrix::getTransformed(const Eigen::Vector3d &vec) const
{
    Eigen::Vector4d tmp = _transformationMatrix*(vec.homogeneous());
    return tmp.hnormalized();
}

Eigen::Matrix4d CT_TransformationMatrix::getTransformed(Eigen::Matrix4d &mat) const
{
    return _transformationMatrix*mat;
}
