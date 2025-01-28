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

#include "polygonforpicking.h"

#include <QPointF>
#include <QLineF>
#include <QPainter>

#include <limits>

#include "actions/tools/math.h"

#define MIN_DISTANCE 5

namespace AMKgl {
    PolygonForPicking::PolygonForPicking()
    {
        m_moveWhat = MoveNone;
        m_pointToMoveIndex = -1;
        m_polygonClosed = false;
    }

    void PolygonForPicking::clear()
    {
        m_polygon.clear();
        m_pointToMoveIndex = -1;
        m_moveWhat = MoveNone;
        m_polygonClosed = false;
        emit polygonChanged();
        redrawOverlay();
    }

    bool PolygonForPicking::isPolygonClosed() const
    {
        return m_polygonClosed;
    }

    bool PolygonForPicking::closePolygon()
    {
        if(!isPolygonClosed() && (m_polygon.size() > 2)) {
            m_polygonClosed = true;
            reorderPolygonInClockWiseOrder();
            emit polygonChanged();
            redrawOverlay();
        }

        return isPolygonClosed();
    }

    bool PolygonForPicking::removeLastPoint()
    {
        if(m_polygon.isEmpty())
            return false;

        m_pointToMoveIndex = -1;
        m_moveWhat = MoveNone;
        m_polygonClosed = false;
        m_polygon.removeLast();
        m_originPolygon = m_polygon;
        emit polygonChanged();
        redrawOverlay();

        return true;
    }

    void PolygonForPicking::setPolygon(const QPolygon &p)
    {
        m_pointToMoveIndex = -1;
        m_moveWhat = MoveNone;
        m_polygon = p;
        m_originPolygon = m_polygon;
        m_polygonClosed = false;
        closePolygon();
        emit polygonChanged();
        redrawOverlay();
    }

    const QPolygon& PolygonForPicking::getPolygon() const
    {
        return m_polygon;
    }

    bool PolygonForPicking::mousePressEvent(QMouseEvent *e)
    {
        if((e->button() == Qt::LeftButton)
                && (m_polygon.size() > 1)) {

            double distance;
            Math::getClosestPolygonPointToPoint(m_polygon, m_mousePos, distance, &m_pointToMoveIndex);

            if((m_pointToMoveIndex != -1) && (distance < MIN_DISTANCE)) {
                m_moveWhat = MovePointOfPolygon;
                m_originPolygon = m_polygon;
            } else if(isMousePosInPolygon()) {
                m_moveWhat = MovePolygon;
                m_originPolygon = m_polygon;
            }
        }

        if(m_moveWhat != MoveNone)
            m_originPointMove = e->pos();

        return true;
    }

    bool PolygonForPicking::mouseMoveEvent(QMouseEvent *e)
    {
        Q_UNUSED(e)

        m_mousePos = e->pos();

        if(m_moveWhat == MovePointOfPolygon) {
            if(currentPointCanBeMoved()) {
                m_polygon[m_pointToMoveIndex] = m_mousePos;
                emit polygonChanged();
            }

        } else if(m_moveWhat == MovePolygon) {
            m_polygon = m_originPolygon;
            m_polygon.translate(e->pos() - m_originPointMove);
            emit polygonChanged();
        }

        redrawOverlay();

        return true;
    }

    bool PolygonForPicking::mouseReleaseEvent(QMouseEvent *e)
    {
        if((e->button() == Qt::LeftButton)
                && (m_moveWhat == MoveNone)) {

            if(newPointCanBeAdded())
            {
                int index = getWhereToInsertPoint();
                m_polygon.insert(index, e->pos());

                emit polygonChanged();
                redrawOverlay();
            }
        }

        m_moveWhat = MoveNone;

        return true;
    }

    bool PolygonForPicking::mouseDoubleClickEvent(QMouseEvent *e)
    {
        Q_UNUSED(e)

        return false;
    }

    bool PolygonForPicking::wheelEvent(QWheelEvent *e)
    {
        Q_UNUSED(e)

        return true;
    }

    bool PolygonForPicking::keyPressEvent(QKeyEvent *e)
    {
        Q_UNUSED(e)

        return false;
    }

    bool PolygonForPicking::keyReleaseEvent(QKeyEvent *e)
    {
        Q_UNUSED(e)

        return false;
    }

    void PolygonForPicking::drawOverlay(QPainter &painter)
    {
        painter.save();

            // draw just lines of the polygon
            painter.setPen(createPenToUse(Qt::blue));
            painter.setBrush(QColor(0, 0, 255, 50));
            drawPolygon(m_polygon, painter);

            // draw the preview
            drawPreview(painter);

            /*painter.save();
                drawInterestPoint(painter, info);
            painter.restore();*/

        painter.restore();
    }

    void PolygonForPicking::drawPolygon(const QPolygon &pol, QPainter &painter)
    {
        if(m_polygonClosed) {
            painter.drawPolygon(pol);
        } else {
            QVectorIterator<QPoint> it(pol);

            if(it.hasNext()) {
                QPoint lastP = it.next();

                while(it.hasNext()) {
                    const QPoint &p = it.next();

                    painter.drawLine(lastP, p);

                    lastP = p;
                }
            }
        }
    }

    void PolygonForPicking::drawInterestPoint(QPainter &painter)
    {
        if(m_polygon.size() > 1) {
            int index = getWhereToInsertPoint();

            if(index >= m_polygon.size())
                --index;

            painter.setPen(createPenToUse(Qt::red));
            painter.setBrush(Qt::red);
            painter.drawEllipse(m_polygon.at(index), 2, 2);

            --index;

            if(index < 0)
                index = m_polygon.size()-1;

            painter.drawEllipse(m_polygon.at(index), 2, 2);

            painter.setPen(createPenToUse(Qt::white));
            int i = 0;
            foreach (const QPoint &p, m_polygon) {
                painter.drawText(p, QString().setNum(i++));
            }
        }
    }

    void PolygonForPicking::drawPreview(QPainter &painter)
    {
        bool drawFilled = false;

        int nPoint = m_polygon.size();

        if(nPoint > 1) {

            double distance;
            int index;
            QPoint p = Math::getClosestPolygonPointToPoint(m_polygon, m_mousePos, distance, &index);

            if(distance < MIN_DISTANCE) {
                painter.setPen(createPenToUse(Qt::yellow));
                painter.setBrush(Qt::yellow);
                painter.drawEllipse(p, 5, 5);

                painter.setPen(createPenToUse(Qt::red));
                painter.setBrush(Qt::red);
                painter.drawEllipse(p, 1, 1);

            } else if(!m_polygonClosed) {
                drawFilled = true;
            } else if(isMousePosInPolygon()){
                painter.setPen(createPenToUse(Qt::yellow));
                painter.setBrush(QBrush());

                drawPolygon(m_polygon, painter);
            }
        } else if (nPoint != 0) {
            drawFilled = !m_polygonClosed;
        }

        if(drawFilled) {

            if(newPointCanBeAdded()) {
                painter.setPen(createPenToUse(Qt::yellow));
                painter.setBrush(Qt::yellow);

                painter.drawLine(m_polygon.last(), m_mousePos);
                painter.drawEllipse(m_mousePos, 5, 5);

                painter.setPen(createPenToUse(Qt::red));
                painter.setBrush(Qt::red);
                painter.drawEllipse(m_mousePos, 1, 1);
            }
        }
    }

    QVector<PolygonForPicking::ClosestResult> PolygonForPicking::getClosestPointsToMousePos() const
    {
        QVector<PolygonForPicking::ClosestResult> res;

        struct {
            QPoint center;

            bool operator()(const QPoint &o1, const QPoint &o2)
            {
                double dist1 = Math::distancePoints(o1, center);
                double dist2 = Math::distancePoints(o2, center);

                return dist2 > dist1;
            }
        } customSort;

        customSort.center = m_mousePos;

        QVector<QPoint> vec = m_polygon;

        qSort(vec.begin(), vec.end(), customSort);

        foreach (const QPoint &p, vec) {
            PolygonForPicking::ClosestResult r;
            r.point = p;
            r.distance = Math::distancePoints(p, m_mousePos);
            r.index = m_polygon.indexOf(p);

            res.append(r);
        }

        return res;
    }

    bool PolygonForPicking::isMousePosInPolygon() const
    {
        return m_polygonClosed && Math::isPointInPolygon(m_polygon, m_mousePos);
    }

    void PolygonForPicking::redrawOverlay()
    {
        emit mustBeRedraw();
    }

    int PolygonForPicking::getWhereToInsertPoint() const
    {
        return m_polygon.size();
    }

    bool PolygonForPicking::newPointCanBeAdded() const
    {
        if(!m_polygon.isEmpty() && m_polygon.contains(m_mousePos))
            return false;

        return (m_polygon.size() < 2) || (!m_polygonClosed && !isLineIntersectPolygon(m_polygon, QLineF(m_polygon.last(), m_mousePos)));
    }

    bool PolygonForPicking::currentPointCanBeMoved() const
    {
        if((m_pointToMoveIndex == -1) || (m_polygon.size() < 4))
            return true;

        QPolygon nextPolygon = m_polygon;
        nextPolygon[m_pointToMoveIndex] = m_mousePos;

        int previous = m_pointToMoveIndex-1;
        int next = m_pointToMoveIndex+1;

        if(previous < 0)
            previous = nextPolygon.size()-1;

        if(next == nextPolygon.size())
            next = 0;

        return !isLineIntersectPolygon(nextPolygon, QLineF(nextPolygon.at(m_pointToMoveIndex), nextPolygon.at(previous)))
                && !isLineIntersectPolygon(nextPolygon, QLineF(nextPolygon.at(m_pointToMoveIndex), nextPolygon.at(next)));
    }

    bool PolygonForPicking::isLineIntersectPolygon(const QPolygon &pol, const QLineF &line, QPointF *intersectionPoint) const
    {
        QVectorIterator<QPoint> it(pol);

        bool error = false;

        if(it.hasNext()) {
            QPoint lastP = it.next();
            QPoint firstP = lastP;

            while(it.hasNext() && !error) {
                const QPoint &p = it.next();

                error = isLineIntersectLine(line, QLineF(lastP, p), intersectionPoint);

                lastP = p;
            }

            if(!error && m_polygonClosed)
                error = isLineIntersectLine(line, QLineF(lastP, firstP), intersectionPoint);
        }

        return error;
    }

    bool PolygonForPicking::isLineIntersectLine(const QLineF &line1, const QLineF &line2, QPointF *intersectionPoint) const
    {
        bool intersect = false;

        QPointF commonPoint;

        if(line1.p1() == line2.p1())
            commonPoint = line1.p1();
        else if(line1.p2() == line2.p2())
            commonPoint = line1.p2();
        else if(line1.p1() == line2.p2())
            commonPoint = line1.p1();
        else if(line1.p2() == line2.p1())
            commonPoint = line1.p2();

        QPointF intersection;

        QLineF::IntersectType type = line1.intersect(line2, &intersection);
        if(type == QLineF::BoundedIntersection) {

            if(commonPoint.isNull() || (intersection != commonPoint)) {

                intersect = true;

                if(intersectionPoint != NULL)
                    *intersectionPoint = intersection;
            }
        } else if(type == QLineF::UnboundedIntersection) {
            if(Math::distancePointSegment(line1, intersection) < 2
                    && Math::distancePointSegment(line2, intersection) < 2) {
                intersect = true;

                if(intersectionPoint != NULL)
                    *intersectionPoint = intersection;
            }
        }

        return intersect;
    }

    void PolygonForPicking::reorderPolygonInClockWiseOrder()
    {
        if(m_polygon.size() > 2) {

            // find the point at min X

            int nPoint = m_polygon.size();

            int minX = m_polygon.boundingRect().right();
            int minPIndex = -1;
            int i = 0;

            foreach (const QPointF &p, m_polygon) {
                if(p.x() < minX) {
                    minX = p.x();
                    minPIndex = i;
                }
                ++i;
            }

            QPolygon newPolygon;

            int next = minPIndex+1;
            int goTo = 1;

            if(next == nPoint)
                next = 0;

            if(m_polygon.at(next).y() > m_polygon.at(minPIndex).y())
                goTo = -1;

            i = minPIndex;

            for(int j=0; j<nPoint; ++j) {
                newPolygon.append(m_polygon.at(i));

                i = i+goTo;

                if(i == nPoint)
                    i = 0;
                else if(i < 0)
                    i = nPoint-1;
            }

            m_polygon = newPolygon;
        }
    }

    QPen PolygonForPicking::createPenToUse(const QColor &color)
    {
        return QPen(QBrush(color), 1);
    }
}
