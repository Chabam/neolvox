/****************************************************************************

 Copyright (C) 2010-2012 AMVALOR, France. All rights reserved.

 Contact : micha.krebs@gmail.com

 Developers : Michaël KREBS (AMVALOR)

 This file is part of AMKgl

 Computree is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Computree is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Amkgl. If not, see <http://www.gnu.org/licenses/>.

*****************************************************************************/

#ifndef POLYGONFORPICKING_H
#define POLYGONFORPICKING_H

#include <QMouseEvent>
#include <QPolygon>

namespace AMKgl {
    class PolygonForPicking : public QObject
    {
        Q_OBJECT

    public:
        PolygonForPicking();

        /**
         * @brief Clear the current polygon
         */
        void clear();

        /**
         * @brief Returns true if polygon is closed
         */
        bool isPolygonClosed() const;

        /**
         * @brief Close the polygon
         * @return false if polygon can't be closed
         */
        bool closePolygon();

        /**
         * @brief Remove the last point added
         * @return false if polygon was empty
         */
        bool removeLastPoint();

        /**
         * @brief Set the polygon.
         */
        void setPolygon(const QPolygon& p);

        /**
         * @brief Returns the polygon
         */
        const QPolygon& getPolygon() const;

        /**
         * @brief Called from graphics view when a mouse press event occured
         * @return Returns true if the graphics view must not use the mouse event
         */
        bool mousePressEvent(QMouseEvent *e);

        /**
         * @brief Called from graphics view when a mouse move event occured
         * @return Returns true if the graphics view must not use the mouse event
         */
        bool mouseMoveEvent(QMouseEvent *e);

        /**
         * @brief Called from graphics view when a mouse release event occured
         * @return Returns true if the graphics view must not use the mouse event
         */
        bool mouseReleaseEvent(QMouseEvent *e);

        /**
         * @brief Called from graphics view when a mouse double click event occured
         * @return Returns true if the graphics view must not use the mouse event
         */
        bool mouseDoubleClickEvent(QMouseEvent *e);

        /**
         * @brief Called from graphics view when a mouse wheel event occured
         * @return Returns true if the graphics view must not use the mouse event
         */
        bool wheelEvent(QWheelEvent *e);

        /**
         * @brief Called from graphics view when a key press event occured
         * @return Returns true if the graphics view must not use the key event
         */
        bool keyPressEvent(QKeyEvent *e);

        /**
         * @brief Called from graphics view when a key release event occured
         * @return Returns true if the graphics view must not use the key event
         */
        bool keyReleaseEvent(QKeyEvent *e);

        /**
         * @brief Called from graphics view when we must draw it
         */
        void drawOverlay(QPainter &painter);

    private:
        enum MoveWhat {
            MoveNone,
            MovePointOfPolygon,
            MovePolygon
        };

        struct ClosestResult {
            QPoint  point;
            double  distance;
            int     index;
        };

        QPolygon        m_polygon;
        QPoint          m_mousePos;

        QPoint          m_originPointMove;
        QPolygon        m_originPolygon;

        MoveWhat        m_moveWhat;
        int             m_pointToMoveIndex;

        bool            m_polygonClosed;

        /**
         * @brief Draw the polygon
         */
        void drawPolygon(const QPolygon &pol, QPainter &painter);

        /**
         * @brief Draw interested points of the polygon
         */
        void drawInterestPoint(QPainter &painter);

        /**
         * @brief Draw the preview polygon
         */
        void drawPreview(QPainter &painter);

        /**
         * @brief Get the closest points to mouse pos
         */
        QVector<ClosestResult> getClosestPointsToMousePos() const;

        /**
         * @brief Returns true if the mouse pos is in the polygon
         */
        bool isMousePosInPolygon() const;

        /**
         * @brief Call to redraw only overlay
         */
        void redrawOverlay();

        /**
         * @brief Returns the index in the polygon where to insert the point
         */
        int getWhereToInsertPoint() const;

        /**
         * @brief Returns true if point under mouse position can be added
         */
        bool newPointCanBeAdded() const;

        /**
         * @brief Returns true if point under mouse position can be moved
         */
        bool currentPointCanBeMoved() const;

        /**
         * @brief Returns true if line intersect with the polygon
         */
        bool isLineIntersectPolygon(const QPolygon &pol, const QLineF &line, QPointF* intersectionPoint = NULL) const;

        /**
         * @brief Returns true if line intersect with another line
         */
        bool isLineIntersectLine(const QLineF &line1, const QLineF &line2, QPointF* intersectionPoint = NULL) const;

        /**
         * @brief Order points of the polygon in clockwise order
         */
        void reorderPolygonInClockWiseOrder();

        /**
         * @brief Returns the pen to use
         */
        QPen createPenToUse(const QColor &color);

    signals:
        /**
         * @brief Emitted when you must redraw this object
         */
        void mustBeRedraw();

        /**
         * @brief Emitted when the polygon changed
         */
        void polygonChanged();
    };
}

#endif // POLYGONFORPICKING_H
