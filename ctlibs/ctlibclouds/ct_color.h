#ifndef CT_COLOR_H
#define CT_COLOR_H

#include <QColor>

#include "ctlibclouds_global.h"

#include <Eigen/Core>

namespace Eigen {
    typedef Eigen::Matrix<uchar, 4, 1>    Vector4ub;
    //typedef std::vector<uchar>   Vector4ub;
}

class CT_Color : public Eigen::Vector4ub
{
public:
    CT_Color(void):Eigen::Vector4ub(255,255,255,255) {}
    // This constructor allows you to construct CT_Color from Eigen expressions
    template<typename OtherDerived>
    CT_Color(const Eigen::MatrixBase<OtherDerived>& other)
        : Eigen::Vector4ub(other)
    { }
    // This method allows you to assign Eigen expressions to CT_Color
    template<typename OtherDerived>
    CT_Color& operator=(const Eigen::MatrixBase <OtherDerived>& other)
    {
        this->Eigen::Vector4ub::operator=(other);
        return *this;
    }

    inline CT_Color(const QColor &qtColor) : Eigen::Vector4ub(qtColor.red(),
                                                              qtColor.green(),
                                                              qtColor.blue(),
                                                              qtColor.alpha())
    {
    }

    inline CT_Color(const uchar& r,
                    const uchar& g,
                    const uchar& b,
                    const uchar& a) : Eigen::Vector4ub(r,g,b,a)
    {
    }

    inline CT_Color& operator =(const QColor &qtColor)
    {
        (*this)(0) = qtColor.red();
        (*this)(1) = qtColor.green();
        (*this)(2) = qtColor.blue();
        (*this)(3) = qtColor.alpha();

        return (*this);
    }

    inline uchar& r() { return (*this)[0]; }
    inline uchar& g() { return (*this)[1]; }
    inline uchar& b() { return (*this)[2]; }
    inline uchar& a() { return (*this)[3]; }

    inline const uchar& r() const { return (*this)[0]; }
    inline const uchar& g() const { return (*this)[1]; }
    inline const uchar& b() const { return (*this)[2]; }
    inline const uchar& a() const { return (*this)[3]; }

    inline void set(const uchar& r,
                    const uchar& g,
                    const uchar& b,
                    const uchar& a = 255) {
        (*this)(0) = r;
        (*this)(1) = g;
        (*this)(2) = b;
        (*this)(3) = a;
    }
};

#endif // CT_COLOR_H
