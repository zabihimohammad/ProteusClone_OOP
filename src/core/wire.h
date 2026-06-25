#pragma once
#include <QGraphicsItem>

class Terminal; // معرفی اولیه کلاس پایه

class Wire : public QGraphicsItem {
public:
    Wire(Terminal *startTerm, QPointF startPos);

    void setEndPoint(QPointF endPos);
    void confirmConnection(Terminal *endTerm);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    Terminal *startTerminal;
    Terminal *endTerminal;
    QPointF p1;
    QPointF p2;
};