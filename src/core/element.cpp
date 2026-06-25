#include "element.h"
#include <QPointF>
#include <qmath.h> // برای استفاده از توابع گرد کردن ریاضی

QVariant Element::itemChange(GraphicsItemChange change, const QVariant &value) {
    // بررسی می‌کنیم که آیا تغییر از نوع "جابجایی موقعیت قطعه" است و آیا قطعه روی بوم قرار دارد؟
    if (change == ItemPositionChange && scene()) {

        // مختصات جدیدی که موس می‌خواهد قطعه را به آنجا ببرد را می‌خوانیم
        QPointF newPos = value.toPointF();

        // اندازه شبکه‌های شطرنجی که در گام دوم روی 20 تنظیم کرده بودیم
        int gridSize = 20;

        // الگوریتم Snap: عدد را بر 20 تقسیم کرده، گرد می‌کنیم و دوباره در 20 ضرب می‌کنیم
        // مثال: عدد 27 تقسیم بر 20 می‌شود 1.35 -> گرد می‌شود به 1 -> ضربدر 20 می‌شود 20
        qreal snappedX = qRound(newPos.x() / gridSize) * gridSize;
        qreal snappedY = qRound(newPos.y() / gridSize) * gridSize;

        // مختصات اصلاح‌شده و قفل‌شده روی تقاطع‌ها را به موتور گرافیکی برمی‌گردانیم
        return QPointF(snappedX, snappedY);
    }

    // اگر تغییرات دیگری بود (مثلاً انتخاب شدن یا تغییر رنگ)، اجازه می‌دهیم روال عادی Qt انجام شود
    return QGraphicsItem::itemChange(change, value);
}