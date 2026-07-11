#include "terminal.h"
#include <QPainter>
#include <QCursor>
#include <QGraphicsSceneHoverEvent>
#include <QToolTip>
#include "../canvas/circuit_scene.h"
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
Terminal::Terminal(QGraphicsItem *parent) : QGraphicsItem(parent), isHovered(false) {
    // این دو فلگ بسیار مهم هستند:
    // اولی باعث می‌شود پایه بتواند رویدادهای نزدیک شدن موس را تشخیص دهد
    // دومی باعث می‌شود پایه از قطعه مادر خود (مثلا مقاومت) بیرون نزند
    setAcceptHoverEvents(true);
    setFlag(ItemIgnoresTransformations, false);
    setAcceptHoverEvents(true);
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

void Terminal::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    isHovered = true;
    setCursor(Qt::CrossCursor); // تغییر شکل موس به حالت بعلاوه (آماده سیم‌کشی)
    update(); // دستور به Qt برای رسم مجدد پایه (تا آبی شود)
}

/*void Terminal::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    isHovered = false;
    setCursor(Qt::ArrowCursor); // بازگشت موس به حالت عادی
    update(); // رسم مجدد (تا دوباره نامرئی شود)
}*/

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
void Terminal::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
    CircuitScene *scene = dynamic_cast<CircuitScene*>(this->scene());
    if (scene && scene->voltageProbe) {
        // ارسال ولتاژ ترمینال به پروب گرافیکی
        scene->voltageProbe->updateProbe(voltageLevel, event->scenePos());
        if(scene->isProbeEnabled){
            scene->voltageProbe->updateProbe(voltageLevel, event->scenePos());
        }
        else {
            scene->voltageProbe->hide();
        }
    }
    QGraphicsItem::hoverMoveEvent(event);
}

void Terminal::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    isHovered = false;
    setCursor(Qt::ArrowCursor); // بازگشت موس به حالت عادی
    update(); // رسم مجدد (تا دوباره نامرئی شود)
    CircuitScene *scene = dynamic_cast<CircuitScene*>(this->scene());
    if (scene && scene->voltageProbe) {
        // پنهان کردن پروب با کنار رفتن موس
        scene->voltageProbe->hide();
    }
    QGraphicsItem::hoverLeaveEvent(event);
}