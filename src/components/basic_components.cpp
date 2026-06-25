#include "basic_components.h"
#include <QPainter>

Resistor::Resistor() {
    // تنظیمات اولیه می‌تواند اینجا قرار بگیرد
}

QRectF Resistor::boundingRect() const {
    // یک مستطیل فرضی با عرض ۷۰ و ارتفاع ۳۰ که مرکز آن نقطه (0,0) است
    return QRectF(-35, -15, 70, 30);
}

void Resistor::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    // تنظیم ضخامت قلم و رنگ پیش‌فرض (مشکی)
    QPen pen(Qt::black, 2);

    // اگر کاربر روی مقاومت کلیک کرد، رنگ آن قرمز شود تا مشخص شود انتخاب شده است
    if (isSelected()) {
        pen.setColor(Qt::red);
    }
    painter->setPen(pen);

    // رسم پایه‌ها و خطوط زیگزاگ مقاومت با استفاده از مختصات هندسی دقیق
    painter->drawLine(-30, 0, -20, 0);       // پایه چپ
    painter->drawLine(-20, 0, -15, -10);     // شروع زیگزاگ به بالا
    painter->drawLine(-15, -10, -5, 10);     // پایین
    painter->drawLine(-5, 10, 5, -10);       // بالا
    painter->drawLine(5, -10, 15, 10);       // پایین
    painter->drawLine(15, 10, 20, 0);        // پایان زیگزاگ
    painter->drawLine(20, 0, 30, 0);         // پایه راست
}

void Resistor::process() {
    // منطق جریان و ولتاژ که بعداً نوشته می‌شود
}