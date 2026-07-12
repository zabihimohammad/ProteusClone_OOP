#include "terminal.h"
#include "wire.h"
#include <QPainter>
#include <QCursor>
#include <QGraphicsSceneHoverEvent>
#include <QToolTip>
#include "../canvas/circuit_scene.h"
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <qmath.h>

Terminal::Terminal(QGraphicsItem *parent) : QGraphicsItem(parent), isHovered(false) {
    setAcceptHoverEvents(true);
    setFlag(ItemIgnoresTransformations, false);
    resetState(); // مقداردهی اولیه متغیرهای الکتریکی
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

QRectF Terminal::boundingRect() const {
    return QRectF(-6, -6, 12, 12);
}

void Terminal::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::NoPen);
    QBrush brush(Qt::transparent);

    // تغییر رنگ پایه در زمان قرار گرفتن موس
    if (isHovered) {
        pen.setStyle(Qt::SolidLine);
        pen.setColor(QColor(0, 120, 255));
        pen.setWidth(2);
        brush.setColor(QColor(0, 120, 255, 100));
        brush.setStyle(Qt::SolidPattern);
    }

    // بازخورد گرافیکی DRC: قرمز شدن پایه در صورت اتصال کوتاه یا وضعیت نامشخص
    if (_isUndefined) {
        pen.setStyle(Qt::SolidLine);
        pen.setColor(Qt::red);
        pen.setWidth(2);
        brush.setColor(QColor(255, 0, 0, 150));
        brush.setStyle(Qt::SolidPattern);
    }

    painter->setPen(pen);
    painter->setBrush(brush);
    painter->drawEllipse(-4, -4, 8, 8);
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

void Terminal::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    // فعلاً فقط رویداد را ثبت می‌کنیم. منطق رسم سیم را در مراحل بعدی اینجا اضافه خواهیم کرد
    event->accept();
    QGraphicsItem::mousePressEvent(event);
}

// ============================================================================
// پیاده‌سازی منطق الکتریکی و DRC (Design Rule Check)
// ============================================================================

void Terminal::resetState() {
    _voltage = 0.0;
    _isDriven = false;
    _isUndefined = false;
    update();
}

void Terminal::setVoltage(double v) {
    // پیاده‌سازی بخش 11.1 (DRC): تشخیص اتصال کوتاه
    // اگر پایه‌ای قبلاً در همین سیکل درایو شده باشد و اکنون ولتاژ متفاوتی به آن اعمال شود:
    if (_isDriven && qAbs(_voltage - v) > 0.1) {
        _isUndefined = true;
        qWarning() << "DRC ERROR: Short Circuit Detected! Conflicting voltages at terminal.";
        update(); // قرمز کردن پایه
        return;
    }

    _voltage = v;
    _isDriven = true;
    _isUndefined = false;
}

void Terminal::setUndefined() {
    _isUndefined = true;
    update();
}

double Terminal::getVoltage() const {
    return _voltage;
}

bool Terminal::isFloating() const {
    // پیاده‌سازی بخش 11.2 (DRC): پایه رها شده
    // پایه رهاست اگر هیچ سیمی به آن وصل نباشد و از داخل قطعه هم مقداردهی (درایو) نشده باشد
    return connectedWires.isEmpty() && !_isDriven;
}

bool Terminal::getLogicState() const {
    // وضعیت نامشخص یا فلوتینگ نمی‌تواند لاجیک درستی بدهد
    if (_isUndefined || isFloating()) return false;

    // تبدیل ولتاژ به منطق: فرض می‌کنیم در این مدار ولتاژ بالای 2.5 ولت معادل HIGH است
    return _voltage >= 2.5;
}

bool Terminal::isUndefinedState() const {
    return _isUndefined;
}