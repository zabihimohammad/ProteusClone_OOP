#include "probe_item.h"
#include <QFont>

ProbeItem::ProbeItem(QGraphicsItem *parent) : QGraphicsItem(parent) {
    currentVoltage = "Undefined";
    setZValue(1000); // برای اینکه پروب همیشه روی بالاترین لایه و روی همه قطعات نمایش داده شود
    hide(); // در حالت عادی پنهان است
}

void ProbeItem::updateProbe(const QString &voltage, const QPointF &pos) {
    currentVoltage = voltage;
    setPos(pos + QPointF(15, 15)); // نمایش پروب کمی پایین‌تر و سمت راست موس
    show();
    update(); // دستور به Qt برای رسم مجدد
}

QRectF ProbeItem::boundingRect() const {
    // ابعاد باکس پروب (عرض 80 و ارتفاع 30 پیکسل)
    return QRectF(0, 0, 80, 30);
}

void ProbeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    // ۱. رسم پس‌زمینه شیک (تیره و نیمه‌شفاف) با گوشه‌های گرد
    painter->setBrush(QColor(30, 30, 40, 230));
    painter->setPen(QPen(QColor(100, 200, 255), 1.5)); // حاشیه آبی روشن
    painter->drawRoundedRect(boundingRect(), 6, 6); // انحنای 6 پیکسلی

    // ۲. تنظیم فونت و استایل متن
    QFont font("Consolas", 10, QFont::Bold);
    painter->setFont(font);

    // ۳. تغییر رنگ متن بر اساس ولتاژ (آپشنال برای زیبایی بیشتر)
    if (currentVoltage == "5.0V") {
        painter->setPen(QColor(255, 80, 80)); // قرمز برای ولتاژ بالا
    } else if (currentVoltage == "0.0V") {
        painter->setPen(QColor(80, 150, 255)); // آبی برای ولتاژ پایین
    } else {
        painter->setPen(Qt::yellow); // زرد برای حالت نامشخص
    }

    // ۴. رسم متن وسط باکس
    painter->drawText(boundingRect(), Qt::AlignCenter, currentVoltage);
}