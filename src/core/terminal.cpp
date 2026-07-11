#include "terminal.h"
#include "wire.h"
#include <QPainter>
#include <QCursor>
#include <QGraphicsSceneHoverEvent>
#include <QToolTip>
#include "../canvas/circuit_scene.h"
#include <QGraphicsSceneMouseEvent>


Terminal::Terminal(QGraphicsItem *parent) : QGraphicsItem(parent), isHovered(false) {
    // این دو فلگ بسیار مهم هستند:
    // اولی باعث می‌شود پایه بتواند رویدادهای نزدیک شدن موس را تشخیص دهد
    // دومی باعث می‌شود پایه از قطعه مادر خود (مثلا مقاومت) بیرون نزند
    setAcceptHoverEvents(true);
    setFlag(ItemIgnoresTransformations, false);
}

QRectF Terminal::boundingRect() const {
    // یک دایره فرضی کوچک به شعاع 6 پیکسل به عنوان نقطه اتصال
    return QRectF(-6, -6, 12, 12);
}

void Terminal::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    // در حالت عادی نامرئی است، اما برای دیباگ می‌توانیم یک رنگ ملایم به آن بدهیم
    QPen pen(Qt::NoPen);
    QBrush brush(Qt::transparent);

    // وقتی موس روی آن می‌آید، یک دایره توپر آبی رنگ برای راهنمایی کاربر روشن می‌شود
    if (isHovered) {
        pen.setStyle(Qt::SolidLine);
        pen.setColor(QColor(0, 120, 255)); // آبی روشن
        pen.setWidth(2);
        brush.setColor(QColor(0, 120, 255, 100)); // آبی نیمه‌شفاف برای داخل دایره
        brush.setStyle(Qt::SolidPattern);
    }

    painter->setPen(pen);
    painter->setBrush(brush);
    painter->drawEllipse(-4, -4, 8, 8); // رسم نقطه اتصال
}

void Terminal::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    // فعلاً فقط رویداد را ثبت می‌کنیم. منطق رسم سیم را در مراحل بعدی اینجا اضافه خواهیم کرد
    event->accept();
}

void Terminal::addWire(Wire *wire) {
    if (!connectedWires.contains(wire)) {
        connectedWires.append(wire);
    }
}

void Terminal::removeWire(Wire *wire) {
    connectedWires.removeAll(wire);
}

QList<Wire*> Terminal::getConnectedWires() const {
    return connectedWires;
}

// ==========================================
// رویدادهای Hover (ورود، حرکت و خروج موس)
// ==========================================

void Terminal::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    isHovered = true;
    setCursor(Qt::CrossCursor);
    update(); // روشن شدن نقطه آبی
}

void Terminal::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    isHovered = false; // خاموش شدن نقطه آبی
    setCursor(Qt::ArrowCursor);
    update();
    QGraphicsItem::hoverLeaveEvent(event);
}
Terminal::~Terminal() {
    // قبل از اینکه ترمینال از بین برود، به تمام سیم‌های متصل می‌گوید ارتباط را قطع کنند
    for (Wire *wire : connectedWires) {
        if (wire) {
            wire->disconnectTerminal(this);
        }
    }
    // در نهایت لیست خودش را پاک می‌کند
    connectedWires.clear();
}
