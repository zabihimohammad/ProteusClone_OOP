#include "wire.h"
#include "terminal.h"
#include <QPainter>

Wire::Wire(Terminal *startTerm, QPointF startPos) {
    startTerminal = startTerm;
    endTerminal = nullptr;

    // نقطه شروع را دو بار به لیست اضافه می‌کنیم (دومی برای انتهای متحرک سیم است)
    points.append(startPos);
    points.append(startPos);
    setZValue(-1);
}

void Wire::setEndPoint(QPointF endPos) {
    if (!points.isEmpty()) {
        points.last() = endPos; // همیشه آخرین نقطه به دنبال موس حرکت می‌کند
        prepareGeometryChange();
    }
}

void Wire::addWaypoint(QPointF point) {
    if (points.size() >= 2) {
        // نقطه جدید را درست قبل از نقطه متحرک پایانی وارد می‌کنیم
        points.insert(points.size() - 1, point);
    }
}

void Wire::setFullRoute(const QVector<QPointF> &route) {
    points.clear();
    points.append(startTerminal->sceneBoundingRect().center());
    points.append(route);
    prepareGeometryChange();
}

void Wire::confirmConnection(Terminal *endTerm) {
    endTerminal = endTerm;
}

QRectF Wire::boundingRect() const {
    // پیدا کردن کادری که تمام گره‌های سیم را در بر بگیرد
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
    QPen pen(Qt::blue, 2);
    painter->setPen(pen);

    if (points.size() < 2) return;

    // اگر سیم در حال کشیده شدن با موس است (نقاط کمی دارد)
    if (points.size() <= 3) {
        for (int i = 0; i < points.size() - 1; ++i) {
            QPointF pA = points[i];
            QPointF pB = points[i+1];
            qreal midX = (pA.x() + pB.x()) / 2.0;

            painter->drawLine(pA, QPointF(midX, pA.y()));
            painter->drawLine(QPointF(midX, pA.y()), QPointF(midX, pB.y()));
            painter->drawLine(QPointF(midX, pB.y()), pB);
        }
    } else {
        // اگر سیم توسط هوش مصنوعی مسیردهی شده (صدها نقطه 10 پیکسلی دارد)
        // خطوط مستقیماً به هم وصل می‌شوند تا زیگ‌زاگ نشود
        for (int i = 0; i < points.size() - 1; ++i) {
            painter->drawLine(points[i], points[i+1]);
        }
    }
}