#ifndef CT_NORMAL_H
#define CT_NORMAL_H

#include "ctlibclouds_global.h"

#include <Eigen/Core>

class CT_Normal : public Eigen::Vector4f
{
public:

    CT_Normal(void):Eigen::Vector4f() {}
    // This constructor allows you to construct CT_Normal from Eigen expressions
    template<typename OtherDerived>
    CT_Normal(const Eigen::MatrixBase<OtherDerived>& other)
        : Eigen::Vector4f(other)
    { }
    // This method allows you to assign Eigen expressions to CT_Normal
    template<typename OtherDerived>
    CT_Normal& operator=(const Eigen::MatrixBase <OtherDerived>& other)
    {
        this->Eigen::Vector4f::operator=(other);
        return *this;
    }

    inline CT_Normal(const float& x,
                     const float& y,
                     const float& z,
                     const float& curvature) : Eigen::Vector4f(x, y, z, curvature)
    {
    }


    inline void set(const float& x,
                    const float& y,
                    const float& z,
                    const float& curvature = 1) {
        (*this)(0) = x;
        (*this)(1) = y;
        (*this)(2) = z;
        (*this)(3) = curvature;
    }
};

#endif // CT_NORMAL_H
