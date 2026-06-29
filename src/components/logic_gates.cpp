#include "logic_gates.h"
#include <QPainter>
#include <QPainterPath>
#include <QFont> // اضافه شده برای تنظیم فونت نوشته‌ها
#include "../core/terminal.h"

// ==========================================
// ۱. گیت AND
// ==========================================
AndGate::AndGate() {
    (new Terminal(this))->setPos(-30, -10); // ورودی A
    (new Terminal(this))->setPos(-30, 10);  // ورودی B
    (new Terminal(this))->setPos(30, 0);    // خروجی Y
}

QRectF AndGate::boundingRect() const {
    // ارتفاع کادر افزایش یافته تا متنِ پایین گیت بریده نشود
    return QRectF(-30, -25, 60, 65);
}

void AndGate::process() {}

void AndGate::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    // رسم پایه‌های ورودی و خروجی
    painter->drawLine(-30, -10, -15, -10);
    painter->drawLine(-30, 10, -15, 10);
    painter->drawLine(20, 0, 30, 0);

    // رسم بدنه گیت AND (استاندارد IEEE)
    painter->drawLine(-15, -20, -15, 20); // خط صاف پشت
    painter->drawLine(-15, -20, 0, -20);  // سقف
    painter->drawLine(-15, 20, 0, 20);    // کف
    painter->drawArc(-20, -20, 40, 40, -90 * 16, 180 * 16); // انحنای جلو

    // === نمایش داینامیک میزان تأخیر ===
    painter->setFont(QFont("Consolas", 7, QFont::Bold));
    painter->setPen(Qt::darkBlue);
    painter->drawText(QRectF(-30, 25, 60, 15), Qt::AlignCenter, propagationDelay);
}

// ==========================================
// ۲. گیت OR
// ==========================================
OrGate::OrGate() {
    (new Terminal(this))->setPos(-30, -10);
    (new Terminal(this))->setPos(-30, 10);
    (new Terminal(this))->setPos(30, 0);
}

QRectF OrGate::boundingRect() const { return QRectF(-30, -25, 60, 65); }

void OrGate::process() {}

void OrGate::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    painter->drawLine(-30, -10, -10, -10);
    painter->drawLine(-30, 10, -10, 10);
    painter->drawLine(20, 0, 30, 0);

    // رسم بدنه منحنی گیت OR با استفاده از مسیرها (Paths)
    QPainterPath path;
    path.moveTo(-15, -20);
    path.cubicTo(0, -20, 10, -10, 20, 0);
    path.cubicTo(10, 10, 0, 20, -15, 20);
    path.quadTo(-5, 0, -15, -20);
    painter->drawPath(path);

    // === نمایش داینامیک میزان تأخیر ===
    painter->setFont(QFont("Consolas", 7, QFont::Bold));
    painter->setPen(Qt::darkBlue);
    painter->drawText(QRectF(-30, 25, 60, 15), Qt::AlignCenter, propagationDelay);
}

// ==========================================
// ۳. گیت NOT
// ==========================================
NotGate::NotGate() {
    (new Terminal(this))->setPos(-30, 0);
    (new Terminal(this))->setPos(30, 0);
}

QRectF NotGate::boundingRect() const { return QRectF(-30, -25, 60, 65); }

void NotGate::process() {}

void NotGate::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    painter->drawLine(-30, 0, -15, 0);
    painter->drawLine(10, 0, 30, 0);

    // رسم مثلث و دایره نات
    QPolygonF triangle;
    triangle << QPointF(-15, -15) << QPointF(-15, 15) << QPointF(5, 0);
    painter->drawPolygon(triangle);
    painter->drawEllipse(5, -2.5, 5, 5);

    // === نمایش داینامیک میزان تأخیر ===
    painter->setFont(QFont("Consolas", 7, QFont::Bold));
    painter->setPen(Qt::darkBlue);
    painter->drawText(QRectF(-30, 20, 60, 15), Qt::AlignCenter, propagationDelay);
}

// ==========================================
// ۴. گیت XOR
// ==========================================
XorGate::XorGate() {
    (new Terminal(this))->setPos(-30, -10);
    (new Terminal(this))->setPos(-30, 10);
    (new Terminal(this))->setPos(30, 0);
}

QRectF XorGate::boundingRect() const { return QRectF(-30, -25, 60, 65); }

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

    // === نمایش داینامیک میزان تأخیر ===
    painter->setFont(QFont("Consolas", 7, QFont::Bold));
    painter->setPen(Qt::darkBlue);
    painter->drawText(QRectF(-30, 25, 60, 15), Qt::AlignCenter, propagationDelay);
}

// ==========================================
// ۵. گیت NAND
// ==========================================
NandGate::NandGate() {
    (new Terminal(this))->setPos(-30, -10);
    (new Terminal(this))->setPos(-30, 10);
    (new Terminal(this))->setPos(35, 0);
}

QRectF NandGate::boundingRect() const { return QRectF(-30, -25, 70, 65); }

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

    painter->drawEllipse(20, -2.5, 5, 5); // دایره NOT
    painter->drawLine(25, 0, 35, 0);      // پایه خروجی

    // === نمایش داینامیک میزان تأخیر ===
    painter->setFont(QFont("Consolas", 7, QFont::Bold));
    painter->setPen(Qt::darkBlue);
    painter->drawText(QRectF(-30, 25, 70, 15), Qt::AlignCenter, propagationDelay);
}

// ==========================================
// ۶. فلیپ‌فلاپ (D-FlipFlop)
// ==========================================
DFlipFlop::DFlipFlop() {
    (new Terminal(this))->setPos(-40, -15); // D
    (new Terminal(this))->setPos(-40, 15);  // CLK
    (new Terminal(this))->setPos(40, -15);  // Q
    (new Terminal(this))->setPos(45, 15);   // Q'
}

QRectF DFlipFlop::boundingRect() const {
    // گسترش کادر بالا و پایین برای نمایش وضعیت اولیه و تأخیر
    return QRectF(-45, -50, 100, 100);
}

void DFlipFlop::process() {}

void DFlipFlop::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    painter->drawRect(-30, -30, 60, 60); // بدنه فلیپ‌فلاپ

    // پایه‌های ورودی
    painter->drawLine(-40, -15, -30, -15);
    painter->drawText(-25, -10, "D");

    painter->drawLine(-40, 15, -30, 15);
    painter->drawLine(-30, 10, -25, 15); // مثلث کلاک
    painter->drawLine(-25, 15, -30, 20);

    // پایه‌های خروجی
    painter->drawLine(30, -15, 40, -15);
    painter->drawText(15, -10, "Q");

    painter->drawLine(30, 15, 40, 15);
    painter->drawEllipse(30, 12.5, 5, 5);  // دایره نات برای Q'
    painter->drawLine(35, 15, 45, 15);
    painter->drawText(15, 20, "Q'");

    // === نمایش داینامیک وضعیت اولیه و تأخیر ===
    painter->setFont(QFont("Consolas", 7, QFont::Bold));
    painter->setPen(Qt::darkBlue);

    // چاپ وضعیت اولیه در بالای قطعه
    QString stateText = "Init Q=" + initialQState;
    painter->drawText(QRectF(-40, -45, 80, 15), Qt::AlignCenter, stateText);

    // چاپ میزان تأخیر در پایین قطعه
    painter->drawText(QRectF(-40, 32, 80, 15), Qt::AlignCenter, propagationDelay);
}