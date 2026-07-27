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
    resetState();
}

Terminal::~Terminal() {
    // 🛠️ فیکس حیاتی کرش: وقتی پایه قطعه‌ای پاک می‌شود، تمام سیم‌های متصل به آن هم باید نابود شوند تا در صفحه معلق (یتیم) نمانند.
    QList<Wire*> wiresToDelete = connectedWires;
    connectedWires.clear();
    for (Wire *wire : wiresToDelete) {
        if (wire) {
            if (wire->scene()) wire->scene()->removeItem(wire);
            delete wire;
        }
    }
}

QRectF Terminal::boundingRect() const {
    return QRectF(-6, -6, 12, 12);
}

void Terminal::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::NoPen);
    QBrush brush(Qt::transparent);

    if (isHovered) {
        pen.setStyle(Qt::SolidLine);
        pen.setColor(QColor(0, 120, 255));
        pen.setWidth(2);
        brush.setColor(QColor(0, 120, 255, 100));
        brush.setStyle(Qt::SolidPattern);
    }

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

void Terminal::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    isHovered = true;
    setCursor(Qt::CrossCursor);
    update();
}

void Terminal::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    isHovered = false;
    setCursor(Qt::ArrowCursor);
    update();
    QGraphicsItem::hoverLeaveEvent(event);
}

void Terminal::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    event->accept();
    QGraphicsItem::mousePressEvent(event);
}

void Terminal::resetState() {
    _voltage = 0.0;
    _isDriven = false;
    _isUndefined = false;
}

void Terminal::setVoltage(double v) {
    if (_isDriven && qAbs(_voltage - v) > 0.1) {
        _isUndefined = true;
        qWarning() << "DRC ERROR: Short Circuit Detected! Conflicting voltages at terminal.";
        return;
    }
    _voltage = v;
    exactVoltage = v;
    voltageLevel = QString::number(v, 'f', 1) + "V";
    _isDriven = true;
    _isUndefined = false;
}

void Terminal::setUndefined() {
    _isUndefined = true;
    update();
}

double Terminal::getVoltage() const {
    return exactVoltage;
}

bool Terminal::isFloating() const {
    return connectedWires.isEmpty() && !_isDriven;
}

bool Terminal::getLogicState() const {
    if (_isUndefined || isFloating()) return false;
    return exactVoltage >= 2.5;
}

bool Terminal::isUndefinedState() const {
    return _isUndefined;
}