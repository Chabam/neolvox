#ifndef MATH_H
#define MATH_H

#include <QLineF>
#include <QPointF>
#include <QPolygon>

#include <Eigen/Core>

namespace AMKgl {
    class Math
    {
    public:
        Math();

        template<typename T>
        static double distancePoints(const T &p1, const T &p2) {
            return std::sqrt(std::pow(p1.x()-p2.x(),2) + std::pow(p1.y()-p2.y(), 2));
        }

        static double distancePointSegment(const QLineF &segment, const QPointF &point)
        {
            double x1 = segment.x1();
            double y1 = segment.y1();
            double x2 = segment.x2();
            double y2 = segment.y2();

            double pointX = point.x();
            double pointY = point.y();

            double diffX = x2 - x1;
            float diffY = y2 - y1;
            if ((diffX == 0) && (diffY == 0))
            {
                diffX = pointX - x1;
                diffY = pointY - y1;
                return sqrt(diffX * diffX + diffY * diffY);
            }

            float t = ((pointX - x1) * diffX + (pointY - y1) * diffY) / (diffX * diffX + diffY * diffY);

            if (t < 0)
            {
                //point is nearest to the first point i.e x1 and y1
                diffX = pointX - x1;
                diffY = pointY - y1;
            }
            else if (t > 1)
            {
                //point is nearest to the end point i.e x2 and y2
                diffX = pointX - x2;
                diffY = pointY - y2;
            }
            else
            {
                //if perpendicular line intersect the line segment.
                diffX = pointX - (x1 + t * diffX);
                diffY = pointY - (y1 + t * diffY);
            }

            //returning shortest distance
            return sqrt(diffX * diffX + diffY * diffY);
        }

        /**
         * @brief Get the closest point (closest distance) from point to a line
         */
        static void getClosestPointBetweenPointAndLine(const Eigen::Vector3d &p, const Eigen::Vector3d &LP0, const Eigen::Vector3d &dir, Eigen::Vector3d& pOnLine);

        /**
         * @brief Get the closest point (closest distance) from point to a segment
         */
        static void getClosestPointBetweenPointAndSegment(const Eigen::Vector3d &p, const Eigen::Vector3d &seg_a, const Eigen::Vector3d &seg_b, Eigen::Vector3d& pOnLine);

        /**
         * @brief Returns true if the point is between a and b
         */
        static bool isPointOnSegment(const Eigen::Vector3d &p, const Eigen::Vector3d &a, const Eigen::Vector3d &b);

        /**
         * @brief Returns true if the point is in ellipse
         */
        static bool isPointInEllipse(const Eigen::Vector3d &p, const Eigen::Matrix4d &inversedMatrix, Eigen::Vector3d *inversedP = nullptr);

        /**
         * @brief Returns true if the point is in cylinder
         */
        static bool isPointInCylinder(const Eigen::Vector3d &p, const Eigen::Matrix4d &inversedMatrix, Eigen::Vector3d *inversedP = nullptr);

        /**
         * @brief Returns true if the point is in sphere
         */
        static bool isPointInSphere(const Eigen::Vector3d &p, const Eigen::Matrix4d &inversedMatrix, Eigen::Vector3d *inversedP = nullptr);

        /**
         * @brief Get the closest point to mouse pos
         */
        static QPoint getClosestPolygonPointToPoint(const QPolygon &pol, const QPoint &p, double &distance, int* index = nullptr);

        /**
         * @brief Get the closest point between a point and a triangle
         */
        static void getClosestPointBetweenPointAndTriangle(const Eigen::Vector3d &p1,
                                                           const Eigen::Vector3d &tr1,
                                                           const Eigen::Vector3d &tr2,
                                                           const Eigen::Vector3d &tr3,
                                                           Eigen::Vector3d& closestPoint);

        /**
         * @brief Returns true if the point is in the polygon
         */
        static bool isPointInPolygon(const QPolygon &pol, const QPoint &p);
    };
}

#endif // MATH_H
