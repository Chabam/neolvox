#ifndef PAINTERINTERFACE_H
#define PAINTERINTERFACE_H

#include <QString>
#include <QtOpenGL/QGL>

#if defined(_WIN32) && defined(_MSC_VER) // Microsoft Visual Studio Compiler
#elif (defined(__linux__) || defined(_WIN32)) && defined(__GNUC__) // GNU Compiler (gcc,g++) for Linux, Unix, and MinGW (Windows)
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wdeprecated-copy"
#pragma GCC diagnostic ignored "-Wint-in-bool-context"
#elif defined(__APPLE__) // Clang Compiler (Apple)
#endif
#include "Eigen/Core"
#if defined(_WIN32) && defined(_MSC_VER)
#elif (defined(__linux__) || defined(_WIN32)) && defined(__GNUC__)
#pragma GCC diagnostic warning "-Wall"
#pragma GCC diagnostic warning "-Wextra"
#pragma GCC diagnostic warning "-Wdeprecated-copy"
#pragma GCC diagnostic warning "-Wint-in-bool-context"
#elif defined(__APPLE__)
#endif

class CT_AbstractCloudIndex;
class CT_AbstractMeshModel;

/*!
 *  \brief Représente un objet qui doit être utilisé par les ItemDrawable
 *         pour dessiner leur élements 3D/2D.
 */
class PainterInterface
{
public:
    virtual ~PainterInterface() {}

    /**
     * @brief Save the current color, pen, etc...
     */
    virtual void save() = 0;

    /**
     * @brief Restore the last color, pen, etc... that was saved
     */
    virtual void restore() = 0;

    /**
     * @brief Set to false if you want to disable all next call to "setPointSize" method will be ignored.
     * @warning Don't forget to call enableSetPointSize(true) at the end of your method where you have called enableSetPointSize(false) !
     */
    virtual void enableSetPointSize(bool enable) = 0;

    /**
     * @brief Change the point size for futur draw
     */
    virtual void setPointSize(float size) = 0;

    /**
     * @brief Restore the default point size
     */
    virtual void restoreDefaultPointSize() = 0;

    /**
     * @brief Modify the pen for futur draw
     */
    virtual void setPen(const QPen &pen) = 0;

    /**
     * @brief Restore the default pen
     */
    virtual void restoreDefaultPen() = 0;

    /**
     * @brief Set to false if you want to disable all next call to "setColor" method will be ignored.
     * @warning Don't forget to call enableSetColor(true) at the end of your method where you have called enableSetColor(false) !
     */
    virtual void enableSetColor(bool enable) = 0;

    /**
     * @brief Change the color for futur draw
     */
    virtual void setColor(int r, int g, int b) = 0;
    virtual void setColor(QColor color) = 0;

    /**
     * @brief Returns the current color used
     */
    virtual QColor getColor() = 0;

    /**
     * @brief Draw a point that was not in the global points cloud (otherwise use drawPoint(globalIndex) method)
     */
    virtual void drawPoint(const double &x, const double &y, const double &z) = 0;

    /**
     * @brief Draw a point that was not in the global points cloud (otherwise use drawPoint(globalIndex) method)
     */
    virtual void drawPoint(double *p) = 0;

    /**
     * @brief Draw a point that was in the global points cloud
     * @param globalIndex: index of the point in the global points cloud
     */
    virtual void drawPoint(const size_t &globalIndex) = 0;

    /**
     * @brief Draw all points that was pointed by indexes in "pci" in the global points cloud
     * @param pci : indexes of points to draw
     */
    virtual void drawPointCloud(const CT_AbstractCloudIndex *pci) = 0;

    /**
     * @brief Draw a 3D cube defined by its bounding box (bottom left and top right corner). (NOT FILLED by default)
     */
    virtual void drawCube(const double &x1, const double &y1, const double &z1, const double &x2, const double &y2, const double &z2) = 0;

    /**
     * @brief Draw a 3D cube defined by its bounding box (bottom left and top right corner).
     * @param faces : Specifies the polygons that mode applies to. Must be GL_FRONT_AND_BACK for front- and back-facing polygons. Show glPolygonMode(...) for more details.
     * @param mode : Specifies how polygons will be rasterized. Accepted values are GL_POINT, GL_LINE, and GL_FILL. Show glPolygonMode(...) for more details.
     */
    virtual void drawCube(const double &x1, const double &y1, const double &z1, const double &x2, const double &y2, const double &z2, GLenum faces, GLenum mode) = 0;

    /**
     * @brief Draw a not filled quad face in 3D
     */
    virtual void drawQuadFace( const double &x1, const double &y1, const double &z1,
                               const double &x2, const double &y2, const double &z2,
                               const double &x3, const double &y3, const double &z3,
                               const double &x4, const double &y4, const double &z4 ) = 0;

    /**
     * @brief Draw a filled quad face in 3D
     */
    virtual void fillQuadFace( const double &x1, const double &y1, const double &z1,
                               const double &x2, const double &y2, const double &z2,
                               const double &x3, const double &y3, const double &z3,
                               const double &x4, const double &y4, const double &z4 ) = 0;

    /**
     * @brief Draw a not filled quad face in 3D with associated colors
     */
    virtual void drawQuadFace( const double &x1, const double &y1, const double &z1, int r1, int g1, int b1,
                               const double &x2, const double &y2, const double &z2, int r2, int g2, int b2,
                               const double &x3, const double &y3, const double &z3, int r3, int g3, int b3,
                               const double &x4, const double &y4, const double &z4, int r4, int g4, int b4 ) = 0;

    /**
     * @brief Draw a filled quad face in 3D with associated colors
     */
    virtual void fillQuadFace( const double &x1, const double &y1, const double &z1, int r1, int g1, int b1,
                               const double &x2, const double &y2, const double &z2, int r2, int g2, int b2,
                               const double &x3, const double &y3, const double &z3, int r3, int g3, int b3,
                               const double &x4, const double &y4, const double &z4, int r4, int g4, int b4 ) = 0;

    /**
     * @brief Draw a rectangle in XY plane at level Z
     */
    virtual void drawRectXY(const Eigen::Vector2d &topLeft, const Eigen::Vector2d &bottomRight, const double &z) = 0;

    /**
     * @brief Draw a filled rectangle in XY plane at level Z
     */
    virtual void fillRectXY(const Eigen::Vector2d &topLeft, const Eigen::Vector2d &bottomRight, const double &z) = 0;


    /**
     * @brief Draw a rectangle in XZ plane at level Y
     */
    virtual void drawRectXZ(const Eigen::Vector2d &topLeft, const Eigen::Vector2d &bottomRight, const double &y) =0;

    /**
     * @brief Draw a filled rectangle in XZ plane at level Y
     */
    virtual void fillRectXZ(const Eigen::Vector2d &topLeft, const Eigen::Vector2d &bottomRight, const double &y) = 0;


    /**
     * @brief Draw a rectangle in YZ plane at level X
     */
    virtual void drawRectYZ(const Eigen::Vector2d &topLeft, const Eigen::Vector2d &bottomRight, const double &x) = 0;

    /**
     * @brief Draw a filled rectangle in YZ plane at level X
     */
    virtual void fillRectYZ(const Eigen::Vector2d &topLeft, const Eigen::Vector2d &bottomRight, const double &x) = 0;

    /**
     * @brief Draw a line in 3D
     */
    virtual void drawLine(const double &x1, const double &y1, const double &z1, const double &x2, const double &y2, const double &z2) = 0;

    /**
     * @brief Draw a line in 3D from points in the global cloud
     */
    virtual void drawLine(const size_t& globalEdgeIndex,
                          const size_t &p1GlobalIndex,
                          const size_t &p2GlobalIndex) = 0;

    /**
     * @brief Draw a triangle in 3D
     */
    virtual void drawTriangle(const double &x1, const double &y1, const double &z1,
                              const double &x2, const double &y2, const double &z2,
                              const double &x3, const double &y3, const double &z3) = 0;

    /**
     * @brief Draw a triangle in 3D from points in the global cloud
     */
    virtual void drawTriangle(const size_t& globalFaceIndex,
                              const size_t &p1GlobalIndex,
                              const size_t &p2GlobalIndex,
                              const size_t &p3GlobalIndex) = 0;

    /**
     * @brief Draw a 2D circle (XY)
     */
    virtual void drawCircle(const double &x, const double &y, const double &z, const double &radius) = 0;

    /**
     * @brief Draw a 3D circle
     */
    virtual void drawCircle3D(const Eigen::Vector3d &center, const Eigen::Vector3d &direction, const double &radius) = 0;

    /**
     * @brief Draw a 2D ellipse (XY)
     */
    virtual void drawEllipse(const double &x, const double &y, const double &z, const double &radiusA, const double &radiusB) = 0;

    /**
     * @brief Call this method when you want to start to draw a polygon
     */
    virtual void beginPolygon() = 0;

    /**
     * @brief Call this method to add a point to the polygon
     */
    virtual void addPointToPolygon(const double &x, const double &y, const double &z) = 0;

    /**
     * @brief Call this method to finish the draw of the polygon
     */
    virtual void endPolygon() = 0;

    /**
     * @brief Draw a 2D cylinder (height is in Z axis)
     */
    virtual void drawCylinder(const double & x, const double & y, const double & z, const double & radius, const double & height) = 0;

    /**
     * @brief Draw a 3D cylinder
     */
    virtual void drawCylinder3D(const Eigen::Vector3d &center, const Eigen::Vector3d &direction, const double & radius, const double & height) = 0;

    /**
     * @brief Draw a pyramid given its top point and its base
     */
    virtual void drawPyramid(const double &topX, const double &topY, const double &topZ,
                             const double &base1X, const double &base1Y, const double &base1Z,
                             const double &base2X, const double &base2Y, const double &base2Z,
                             const double &base3X, const double &base3Y, const double &base3Z,
                             const double &base4X, const double &base4Y, const double &base4Z) = 0;
    /**
     * @brief Draw a part of a sphere given the angles bounds
     */
    virtual void drawPartOfSphere (const double &centerX, const double &centerY, const double &centerZ,
                                   const double &radius,
                                   const double &initTheta, const double &endTheta,
                                   const double &initPhi, const double &endPhi,
                                   bool radians = true) = 0;

    /**
     * @brief Draw faces of a mesh
     */
    virtual void drawMesh(const CT_AbstractMeshModel *mesh) = 0;

    /**
     * @brief Draw faces of a mesh
     */
    virtual void drawFaces(const CT_AbstractMeshModel *mesh) = 0;

    /**
     * @brief Draw edges of a mesh
     */
    virtual void drawEdges(const CT_AbstractMeshModel *mesh) = 0;

    /**
     * @brief Draw points of a mesh
     */
    virtual void drawPoints(const CT_AbstractMeshModel *mesh) = 0;

};

#endif // PAINTERINTERFACE_H
