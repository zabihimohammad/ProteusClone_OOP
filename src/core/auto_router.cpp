#include "auto_router.h"
#include "terminal.h"

QVector<QPointF> AutoRouter::findPath(QGraphicsScene *scene, QPointF startPos, QPointF endPos, Terminal *startTerm, Terminal *endTerm, Wire *currentWire) {
    // 🛠️ فیکس کرش هنگام حذف سیم: بررسی Null Pointers
    if (!startTerm || !endTerm) return {startPos, endPos};

    QVector<QPointF> path;
    path.append(startPos);

    // 🛠️ ایجاد مسیر کاملاً متعامد (۹۰ درجه) و استاندارد مشابه پروتئوس
    if (qAbs(startPos.x() - endPos.x()) > 1.0 && qAbs(startPos.y() - endPos.y()) > 1.0) {

        bool isHorizontalStart = true;
        if (startTerm && startTerm->parentItem()) {
            QPointF local = startTerm->pos();
            QRectF bounds = startTerm->parentItem()->boundingRect();
            double distTop = qAbs(local.y() - bounds.top());
            double distBottom = qAbs(local.y() - bounds.bottom());
            if (distTop < 5.0 || distBottom < 5.0) isHorizontalStart = false; // خروج عمودی
        }

        if (isHorizontalStart) {
            path.append(QPointF(endPos.x(), startPos.y())); // اول افقی، بعد عمودی
        } else {
            path.append(QPointF(startPos.x(), endPos.y())); // اول عمودی، بعد افقی
        }
    }

    path.append(endPos);
    return path;
}