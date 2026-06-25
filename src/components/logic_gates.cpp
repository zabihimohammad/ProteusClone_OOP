#include "logic_gates.h"
#include <QPainter>
#include <QPainterPath>
AndGate::AndGate() {}
QRectF AndGate::boundingRect() const { return QRectF(-30, -25, 60, 50); }
void AndGate::process() {}

void AndGate::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    // پایه‌های ورودی (سمت چپ)
    painter->drawLine(-30, -10, -15, -10); // ورودی A
    painter->drawLine(-30, 10, -15, 10);   // ورودی B

    // بدنه گیت AND (خط صاف در چپ، نیم‌دایره در راست)
    painter->drawLine(-15, -20, -15, 20); // خط صاف پشت گیت
    painter->drawLine(-15, -20, 0, -20);  // سقف
    painter->drawLine(-15, 20, 0, 20);    // کف
    painter->drawArc(-20, -20, 40, 40, -90 * 16, 180 * 16); // نیم‌دایره شکم گیت

    // پایه خروجی (سمت راست)
    painter->drawLine(20, 0, 30, 0);       // خروجی Y
}


// --- گیت OR ---
OrGate::OrGate() {}
QRectF OrGate::boundingRect() const { return QRectF(-30, -25, 60, 50); }
void OrGate::process() {}
void OrGate::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);
    painter->drawLine(-30, -10, -10, -10); // ورودی بالا
    painter->drawLine(-30, 10, -10, 10);   // ورودی پایین
    painter->drawLine(20, 0, 30, 0);       // خروجی

    // رسم بدنه منحنی گیت OR با استفاده از مسیرها (Paths)
    QPainterPath path;
    path.moveTo(-15, -20);
    path.cubicTo(0, -20, 10, -10, 20, 0); // انحنای بالا تا نوک
    path.cubicTo(10, 10, 0, 20, -15, 20); // انحنای پایین تا نوک
    path.quadTo(-5, 0, -15, -20);         // انحنای فرو رفته پشتی
    painter->drawPath(path);
}

// --- گیت NOT ---
NotGate::NotGate() {}
QRectF NotGate::boundingRect() const { return QRectF(-30, -25, 60, 50); }
void NotGate::process() {}
void NotGate::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);
    painter->drawLine(-30, 0, -15, 0); // ورودی
    painter->drawLine(10, 0, 30, 0);   // خروجی

    QPolygonF triangle;
    triangle << QPointF(-15, -15) << QPointF(-15, 15) << QPointF(5, 0);
    painter->drawPolygon(triangle);
    painter->drawEllipse(5, -2.5, 5, 5); // دایره کوچک (علامت نات)
}

// --- گیت XOR ---
XorGate::XorGate() {}
QRectF XorGate::boundingRect() const { return QRectF(-30, -25, 60, 50); }
void XorGate::process() {}
void XorGate::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);
    painter->drawLine(-30, -10, -10, -10);
    painter->drawLine(-30, 10, -10, 10);
    painter->drawLine(20, 0, 30, 0);

    QPainterPath path;
    path.moveTo(-15, -20);
    path.cubicTo(0, -20, 10, -10, 20, 0);
    path.cubicTo(10, 10, 0, 20, -15, 20);
    path.quadTo(-5, 0, -15, -20);
    painter->drawPath(path);

    // خط منحنی اضافی پشت گیت برای XOR
    QPainterPath extraArc;
    extraArc.moveTo(-20, -20);
    extraArc.quadTo(-10, 0, -20, 20);
    painter->drawPath(extraArc);
}

// --- گیت NAND ---
NandGate::NandGate() {}
QRectF NandGate::boundingRect() const { return QRectF(-30, -25, 70, 50); }
void NandGate::process() {}
void NandGate::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);
    painter->drawLine(-30, -10, -15, -10);
    painter->drawLine(-30, 10, -15, 10);
    painter->drawLine(-15, -20, -15, 20);
    painter->drawLine(-15, -20, 0, -20);
    painter->drawLine(-15, 20, 0, 20);
    painter->drawArc(-20, -20, 40, 40, -90 * 16, 180 * 16);

    // دایره NOT و پایه خروجی
    painter->drawEllipse(20, -2.5, 5, 5);
    painter->drawLine(25, 0, 35, 0);
}

// --- فلیپ‌فلاپ (D-FlipFlop) ---
DFlipFlop::DFlipFlop() {}
QRectF DFlipFlop::boundingRect() const { return QRectF(-40, -40, 90, 80); }
void DFlipFlop::process() {}
void DFlipFlop::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    painter->drawRect(-30, -30, 60, 60); // بدنه فلیپ‌فلاپ

    // پایه‌های ورودی
    painter->drawLine(-40, -15, -30, -15); // پایه D
    painter->drawText(-25, -10, "D");

    painter->drawLine(-40, 15, -30, 15);   // پایه کلاک (CLK)
    // مثلث کوچک ورودی کلاک
    painter->drawLine(-30, 10, -25, 15);
    painter->drawLine(-25, 15, -30, 20);

    // پایه‌های خروجی
    painter->drawLine(30, -15, 40, -15);   // پایه Q
    painter->drawText(15, -10, "Q");

    painter->drawLine(30, 15, 40, 15);     // پایه Q' (Not Q)
    painter->drawEllipse(30, 12.5, 5, 5);  // دایره نات برای Q'
    painter->drawLine(35, 15, 45, 15);
    painter->drawText(15, 20, "Q'");
}