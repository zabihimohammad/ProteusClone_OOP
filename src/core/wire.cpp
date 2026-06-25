#include "wire.h"
#include "terminal.h"
#include <QPainter>

Wire::Wire(Terminal *startTerm, QPointF startPos) {
    startTerminal = startTerm;
    endTerminal = nullptr;
    p1 = startPos;
    p2 = startPos;
    // با منفی کردن این مقدار، سیم‌ها همیشه زیر قطعات مدار قرار می‌گیرند و روی آن‌ها را نمی‌پوشانند
    setZValue(-1);
}

void Wire::setEndPoint(QPointF endPos) {
    p2 = endPos;
    prepareGeometryChange(); // دستور به موتور گرافیکی Qt برای رسم مجدد
}

void Wire::confirmConnection(Terminal *endTerm) {
    endTerminal = endTerm;
}

QRectF Wire::boundingRect() const {
    // ایجاد یک مستطیل فرضی که خط سیم دقیقا در قطر آن قرار می‌گیرد
    return QRectF(p1, p2).normalized().adjusted(-5, -5, 5, 5);
}

void Wire::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::blue, 2); // رنگ پیش‌فرض سیم
    painter->setPen(pen);

    // ==========================================
    // الگوریتم سیم‌کشی اورتوگونال (Manhattan Routing)
    // ==========================================

    // ۱. پیدا کردن نقطه میانی در محور X
    qreal midX = (p1.x() + p2.x()) / 2.0;

    // ۲. تعریف دو نقطه شکست (گوشه‌های ۹۰ درجه)
    QPointF corner1(midX, p1.y());
    QPointF corner2(midX, p2.y());

    // ۳. رسم سیم در ۳ قطعه متصل به هم
    painter->drawLine(p1, corner1);       // قطعه اول: حرکت افقی از مبدا تا وسط
    painter->drawLine(corner1, corner2);  // قطعه دوم: حرکت عمودی تا ارتفاع مقصد
    painter->drawLine(corner2, p2);       // قطعه سوم: حرکت افقی تا نقطه مقصد
}