#include "wire.h"
#include "terminal.h"
#include <QPainter>
#include "auto_router.h" // برای دسترسی به هوش مصنوعی
#include <QToolTip>
#include <QGraphicsSceneHoverEvent>
#include "probe_item.h"
#include "../canvas/circuit_scene.h"
#include <QPainterPath>
Wire::Wire(Terminal *startTerm, QPointF startPos) {
    startTerminal = startTerm;
    endTerminal = nullptr;

    // نقطه شروع را دو بار به لیست اضافه می‌کنیم (دومی برای انتهای متحرک سیم است)
    points.append(startPos);
    points.append(startPos);
    setZValue(-1);
    // اضافه کردن این خط: اجازه انتخاب شدن با موس
    setFlag(QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(true);
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
    // 🛠️ باگ بزرگ اینجا بود! حالا مسیر هوش مصنوعی دقیقاً و بدون اضافات کپی می‌شود
    points = route;
    prepareGeometryChange();
}

void Wire::confirmConnection(Terminal *endTerm) {
    endTerminal = endTerm;
    // وقتی سیم وصل شد، خودش را در حافظه پایه‌های مبدا و مقصد ثبت می‌کند
    if (startTerminal) startTerminal->addWire(this);
    if (endTerminal) endTerminal->addWire(this);
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
    QPen pen(isSelected() ? Qt::red : Qt::blue, 2);
    painter->setPen(pen);

    if (points.size() < 2) return;

    // 🛠️ اصلاح طلایی: به جای سایز آرایه، چک می‌کنیم که سیم به مقصد رسیده است یا خیر
    if (!endTerminal) {
        // سیم در دست کاربر است (رسم ارتوگونال موقت)
        for (int i = 0; i < points.size() - 1; ++i) {
            QPointF pA = points[i];
            QPointF pB = points[i+1];
            qreal midX = (pA.x() + pB.x()) / 2.0;

            painter->drawLine(pA, QPointF(midX, pA.y()));
            painter->drawLine(QPointF(midX, pA.y()), QPointF(midX, pB.y()));
            painter->drawLine(QPointF(midX, pB.y()), pB);
        }
    } else {
        // سیم نهایی شده است (اتصال مستقیم نقاط هوش مصنوعی بدون کج شدن)
        for (int i = 0; i < points.size() - 1; ++i) {
            painter->drawLine(points[i], points[i+1]);
        }
    }
}
void Wire::updateRoute() {
    // اگر سیم هنوز کامل کشیده نشده (در دست کاربر است) نیازی به آپدیت ندارد
    if (!startTerminal || !endTerminal || !scene()) return;

    // فراخوانی مجدد هوش مصنوعی با مختصات جدید پایه‌ها
    QVector<QPointF> newRoute = AutoRouter::findPath(
            scene(),
            startTerminal->scenePos(),
            endTerminal->scenePos(),
            startTerminal,
            endTerminal,
            this
    );

    // رسم مجدد مسیر
    setFullRoute(newRoute);
}
Wire::~Wire() {
    // وقتی کاربر سیم را با کلید Delete پاک می‌کند، سیم باید نام خودش را از لیست پایه‌ها خط بزند
    if (startTerminal) startTerminal->removeWire(this);
    if (endTerminal) endTerminal->removeWire(this);
}
void Wire::disconnectTerminal(Terminal *term) {
    // اگر ترمینالی که دارد نابود می‌شود پایه شروع من است، آن را فراموش کن
    if (startTerminal == term) {
        startTerminal = nullptr;
    }
    // اگر پایه انتهایی من است، آن را فراموش کن
    if (endTerminal == term) {
        endTerminal = nullptr;
    }
}
// ==========================================================
// محاسبه دقیق خطوط برخورد سیم با موس (جلوگیری از باگ کادر بزرگ)
// ==========================================================
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