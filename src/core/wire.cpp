#include "wire.h"
#include "terminal.h"
#include <QPainter>
#include "auto_router.h"
#include <QToolTip>
#include <QGraphicsSceneHoverEvent>
#include "probe_item.h"
#include "../canvas/circuit_scene.h"
#include "../components/basic_components.h"
#include <QPainterPath>
Wire::Wire(Terminal *startTerm, QPointF startPos) {
    startTerminal = startTerm;
    endTerminal = nullptr;

    // نقطه دوم همراه موس حرکت می‌کند.
    points.append(startPos);
    points.append(startPos);
    setZValue(-1);
    // سیم با موس قابل انتخاب است.
    setFlag(QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(true);
}

void Wire::setEndPoint(QPointF endPos) {
    if (!points.isEmpty()) {
        points.last() = endPos;
        prepareGeometryChange();
    }
}

void Wire::addWaypoint(QPointF point) {
    if (points.size() >= 2) {
        // نقطه را پیش از انتهای متحرک بگذار.
        points.insert(points.size() - 1, point);
    }
}

void Wire::setFullRoute(const QVector<QPointF> &route) {
    // مسیر کامل را جایگزین کن.
    points = route;
    prepareGeometryChange();
}

void Wire::confirmConnection(Terminal *endTerm) {
    endTerminal = endTerm;
    // اتصال را در هر دو پایه ثبت کن.
    if (startTerminal) startTerminal->addWire(this);
    if (endTerminal) endTerminal->addWire(this);
}

QRectF Wire::boundingRect() const {
    // کادر همه بخش‌های سیم
    if (points.isEmpty()) return QRectF();

    qreal minX = points[0].x(), maxX = points[0].x();
    qreal minY = points[0].y(), maxY = points[0].y();

    for (const QPointF &p : points) {
        if (p.x() < minX) minX = p.x();
        if (p.x() > maxX) maxX = p.x();
        if (p.y() < minY) minY = p.y();
        if (p.y() > maxY) maxY = p.y();
    }

    return QRectF(minX, minY, maxX - minX, maxY - minY).adjusted(-5, -5, 5, 5);
}


void Wire::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    // رنگ سیم از ولتاژ آن می‌آید.
    QColor wireColor = Qt::blue;

    if (isSelected()) {
        wireColor = Qt::red;
    } else {
        // رنگ حالت منطقی
        if (voltageLevel == "5.0V") {
            wireColor = QColor("#D64545"); // HIGH
        } else if (voltageLevel == "0.0V") {
            wireColor = QColor("#1473E6"); // LOW
        } else {
            wireColor = QColor("#253143"); // شناور
        }
    }

    QPen pen(wireColor, 2);
    painter->setPen(pen);

    if (points.size() < 2) return;

    // مسیر موقت هنگام سیم‌کشی
    if (!endTerminal) {
        // مسیر موقت عمودی و افقی است.
        for (int i = 0; i < points.size() - 1; ++i) {
            QPointF pA = points[i];
            QPointF pB = points[i+1];
            qreal midX = (pA.x() + pB.x()) / 2.0;

            painter->drawLine(pA, QPointF(midX, pA.y()));
            painter->drawLine(QPointF(midX, pA.y()), QPointF(midX, pB.y()));
            painter->drawLine(QPointF(midX, pB.y()), pB);
        }
    } else {
        // مسیر نهایی را رسم کن.
        for (int i = 0; i < points.size() - 1; ++i) {
            painter->drawLine(points[i], points[i+1]);
        }
    }
}
void Wire::updateRoute() {
    // فقط سیم کامل دوباره مسیریابی می‌شود.
    if (!startTerminal || !endTerminal || !scene()) return;

    // مسیر را از جای جدید پایه‌ها بساز.
    QVector<QPointF> newRoute = AutoRouter::findPath(
            scene(),
            startTerminal->scenePos(),
            endTerminal->scenePos(),
            startTerminal,
            endTerminal,
            this
    );

    // مسیر تازه را نمایش بده.
    setFullRoute(newRoute);
}
Wire::~Wire() {
    if (startTerminal) startTerminal->removeWire(this);
    if (endTerminal) endTerminal->removeWire(this);
}
void Wire::disconnectTerminal(Terminal *term) {
    // اشاره‌گر پایه حذف‌شده را پاک کن.
    if (startTerminal == term) {
        startTerminal = nullptr;
    }
    if (endTerminal == term) {
        endTerminal = nullptr;
    }
}
// محدوده قابل کلیک سیم
QPainterPath Wire::shape() const {
    QPainterPath path;
    if (points.size() < 2) return path;

    if (!endTerminal) {
        for (int i = 0; i < points.size() - 1; ++i) {
            QPointF pA = points[i];
            QPointF pB = points[i+1];
            qreal midX = (pA.x() + pB.x()) / 2.0;

            if (i == 0) path.moveTo(pA);
            path.lineTo(QPointF(midX, pA.y()));
            path.lineTo(QPointF(midX, pB.y()));
            path.lineTo(pB);
        }
    } else {
        path.moveTo(points[0]);
        for (int i = 1; i < points.size(); ++i) {
            path.lineTo(points[i]);
        }
    }

    QPainterPathStroker stroker;
    stroker.setWidth(10);
    return stroker.createStroke(path);
}
// انتخاب بخش سیم
void Wire::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton && points.size() >= 2) {
        QPointF clickPos = event->scenePos();
        m_draggedSegmentIndex = -1;
        m_isDraggingHorizontal = false;
        m_isDraggingVertical = false;

        double minDistance = 10.0; // شعاع انتخاب

        for (int i = 0; i < points.size() - 1; ++i) {
            QPointF a = points[i];
            QPointF b = points[i+1];

            double dx = b.x() - a.x();
            double dy = b.y() - a.y();
            double l2 = dx * dx + dy * dy;
            if (l2 < 0.1) continue;

            double t = ((clickPos.x() - a.x()) * dx + (clickPos.y() - a.y()) * dy) / l2;
            t = qBound(0.0, t, 1.0);
            QPointF proj(a.x() + t * dx, a.y() + t * dy);

            double dist = QLineF(clickPos, proj).length();
            if (dist < minDistance) {
                minDistance = dist;
                m_draggedSegmentIndex = i;
            }
        }

        if (m_draggedSegmentIndex != -1) {
            m_lastDragPos = clickPos;
            QPointF p1 = points[m_draggedSegmentIndex];
            QPointF p2 = points[m_draggedSegmentIndex + 1];

            if (qAbs(p1.y() - p2.y()) < 1.0) m_isDraggingHorizontal = true;
            else m_isDraggingVertical = true;

            if (m_draggedSegmentIndex == 0) {
                points.insert(1, points[0]);
                m_draggedSegmentIndex = 1;
            }
            if (m_draggedSegmentIndex == points.size() - 2) {
                points.insert(points.size() - 1, points.last());
            }

            // انتخاب را به کلاس پایه هم بده.
            QGraphicsItem::mousePressEvent(event);
            return;
        }
    }
    QGraphicsItem::mousePressEvent(event);
}

// جابه‌جایی بخش سیم
void Wire::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (m_draggedSegmentIndex != -1) {
        QPointF currentPos = event->scenePos();
        QPointF delta = currentPos - m_lastDragPos;
        m_lastDragPos = currentPos;

        prepareGeometryChange();

        if (m_isDraggingHorizontal) {
            points[m_draggedSegmentIndex].setY(points[m_draggedSegmentIndex].y() + delta.y());
            points[m_draggedSegmentIndex + 1].setY(points[m_draggedSegmentIndex + 1].y() + delta.y());
        } else if (m_isDraggingVertical) {
            points[m_draggedSegmentIndex].setX(points[m_draggedSegmentIndex].x() + delta.x());
            points[m_draggedSegmentIndex + 1].setX(points[m_draggedSegmentIndex + 1].x() + delta.x());
        }

        update();
        return;
    }
    QGraphicsItem::mouseMoveEvent(event);
}

// تراز بخش سیم با شبکه
void Wire::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if (m_draggedSegmentIndex != -1) {
        int snapSize = 20;

        prepareGeometryChange();
        if (m_isDraggingHorizontal) {
            double snappedY = qRound(points[m_draggedSegmentIndex].y() / snapSize) * snapSize;
            points[m_draggedSegmentIndex].setY(snappedY);
            points[m_draggedSegmentIndex + 1].setY(snappedY);
        } else if (m_isDraggingVertical) {
            double snappedX = qRound(points[m_draggedSegmentIndex].x() / snapSize) * snapSize;
            points[m_draggedSegmentIndex].setX(snappedX);
            points[m_draggedSegmentIndex + 1].setX(snappedX);
        }

        m_draggedSegmentIndex = -1;
        m_isDraggingHorizontal = false;
        m_isDraggingVertical = false;
        update();

        event->accept();
        return;
    }
    QGraphicsItem::mouseReleaseEvent(event);
}
