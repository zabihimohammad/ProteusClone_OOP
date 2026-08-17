#include "probe_item.h"
#include <QFont>

ProbeItem::ProbeItem(QGraphicsItem *parent) : QGraphicsItem(parent) {
    currentVoltage = "Undefined";
    setZValue(1000);
    hide();
}

void ProbeItem::updateProbe(const QString &voltage, const QPointF &pos) {
    currentVoltage = voltage; // متن ولتاژ را نگه دار.
    setPos(pos + QPointF(15, 15));
    show();
    update();
}

QRectF ProbeItem::boundingRect() const {
    return QRectF(0, 0, 80, 30);
}

void ProbeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    painter->setBrush(QColor(30, 30, 40, 230));
    painter->setPen(QPen(QColor(100, 200, 255), 1.5));
    painter->drawRoundedRect(boundingRect(), 6, 6);

    QFont font("Consolas", 10, QFont::Bold);
    painter->setFont(font);

    if (currentVoltage == "5.0V") {
        painter->setPen(QColor(255, 80, 80));
    } else if (currentVoltage == "0.0V") {
        painter->setPen(QColor(80, 150, 255));
    } else {
        painter->setPen(Qt::yellow);
    }

    painter->drawText(boundingRect(), Qt::AlignCenter, currentVoltage);
}