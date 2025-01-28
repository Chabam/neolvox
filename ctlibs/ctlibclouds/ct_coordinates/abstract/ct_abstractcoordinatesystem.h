#ifndef CT_ABSTRACTCOORDINATESYSTEM_H
#define CT_ABSTRACTCOORDINATESYSTEM_H

#include "ctlibclouds_global.h"
#include <QtOpenGL/QGL>

#include <Eigen/Dense>

/**
 * @brief Abstract class that define the interface of a Coordinate System class. A
 *        Coordinate System class can be a projection library per example.
 *
 *        A Coordinate System class must offer the possibility of :
 *
 *        - convert coordinates of type realEx to importable coordinates (realIm)
 *        - convert coordinates of type realIm to exportable coordinates (realEx)
 */
class CTLIBCLOUDS_EXPORT CT_AbstractCoordinateSystem
{
public:

    /**
     * @brief Type of real importable
     */
    typedef float           realIm;

    /**
     * @brief Type of real exportable
     */
    typedef double          realEx;

    /**
     * @brief Data to convert
     */
    enum DataToConvert {
        CONVERT_X = 1,
        CONVERT_Y = 2,
        CONVERT_Z = 4,
        CONVERT_ALL = 7
    };

    Q_DECLARE_FLAGS(DatasToConvert, DataToConvert)

    CT_AbstractCoordinateSystem()
    {
        m_used = false;
        m_uniqueKey = 0;
    }

    virtual ~CT_AbstractCoordinateSystem() {}

    /**
     * @brief Returns true if coordinates passed in parameter can be converted to realIm (import). False
     *        if the capacity of a realIm is not sufficient.
     */
    virtual bool canConvertImport(CT_AbstractCoordinateSystem::realEx x,
                                  CT_AbstractCoordinateSystem::realEx y,
                                  CT_AbstractCoordinateSystem::realEx z) const = 0;

    /**
     * @brief Convert a coordinate of type realEx to realIm. This method don't test if the realIm
     *        can accept the capacity of the converted value (if you want to know if its possible call
     *        the method "canConvertedForImport")
     */
    virtual void convertImport(CT_AbstractCoordinateSystem::realEx x,
                               CT_AbstractCoordinateSystem::realEx y,
                               CT_AbstractCoordinateSystem::realEx z,
                               CT_AbstractCoordinateSystem::realIm &xOut,
                               CT_AbstractCoordinateSystem::realIm &yOut,
                               CT_AbstractCoordinateSystem::realIm &zOut,
                               CT_AbstractCoordinateSystem::DatasToConvert convert = CT_AbstractCoordinateSystem::CONVERT_ALL) const = 0;

    /**
     * @brief Convert a coordinate of type realIm to realEx.
     */
    virtual void convertExport(CT_AbstractCoordinateSystem::realIm x,
                               CT_AbstractCoordinateSystem::realIm y,
                               CT_AbstractCoordinateSystem::realIm z,
                               CT_AbstractCoordinateSystem::realEx &xOut,
                               CT_AbstractCoordinateSystem::realEx &yOut,
                               CT_AbstractCoordinateSystem::realEx &zOut,
                               CT_AbstractCoordinateSystem::DatasToConvert convert = CT_AbstractCoordinateSystem::CONVERT_ALL) const = 0;

    /**
     * @brief Returns the offset of this coordinate system
     */
    virtual void offset(CT_AbstractCoordinateSystem::realEx &x, CT_AbstractCoordinateSystem::realEx &y, CT_AbstractCoordinateSystem::realEx &z) const = 0;
    virtual void offset(Eigen::Vector3d &v) const = 0;

    /**
     * @brief Convert the coordinate system to a matrix 4x4 (used per example in opengl rendering)
     */
    virtual Eigen::Matrix4d toMatrix4x4() const = 0;

    /**
     * @brief Return the index of this coordinate system in the manager
     */
    virtual GLuint indexInManager() const = 0;

    /**
     * @brief Return the unique key of this coordinate system
     */
    uint uniqueKey() const { return m_uniqueKey; }

private:
    mutable bool        m_used;
    uint                m_uniqueKey;

protected:
    friend class CT_AbstractReader;
    friend class CT_AbstractExporter;
    friend class CT_CoordinateSystemManager;

    /**
     * @brief Called by CT_AbstractReader and CT_AbstractExporter to set m_used to false
     */
    void initUsed() { m_used = false; }

    /**
     * @brief Must be called from derived class in method "convertImport" and "convertExport" to inform that this
     *        coordinate system has been used. Set m_used to true.
     */
    void informThatUsed() const { m_used = true; }

    /**
     * @brief Returns true if the coordinate system was used
     */
    bool wasUsed() const { return m_used; }

    /**
     * @brief Set the unique key of this coordinate system
     */
    void setUniqueKey(uint key) { m_uniqueKey = key; }
};

Q_DECLARE_OPERATORS_FOR_FLAGS(CT_AbstractCoordinateSystem::DatasToConvert)

#endif // CT_ABSTRACTCOORDINATESYSTEM_H
