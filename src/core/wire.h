#pragma once
#include <QGraphicsItem>
#include <QVector>
#include <QPointF>
#include <QString>

class Terminal;

class Wire : public QGraphicsItem {
public:
    QPainterPath shape() const override;
    Wire(Terminal *startTerm, QPointF startPos);

    void setEndPoint(QPointF endPos);
    void addWaypoint(QPointF point);
    void setFullRoute(const QVector<QPointF> &route);
    void confirmConnection(Terminal *endTerm);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void updateRoute();
    ~Wire() override;

    // ولتاژ نمایشی سیم
    QString voltageLevel = "Undefined";

    // پایه‌ها و مسیر سیم
    Terminal* getStartTerminal() const { return startTerminal; }
    Terminal* getEndTerminal() const { return endTerminal; }
    QVector<QPointF> getPoints() const { return points; }
    // قطع یک پایه
    void disconnectTerminal(Terminal *term);

private:
    Terminal *startTerminal;
    Terminal *endTerminal;
    QVector<QPointF> points;
    // جابه‌جایی دستی سیم
    int m_draggedSegmentIndex = -1;
    bool m_isDraggingHorizontal = false;
    bool m_isDraggingVertical = false;
    QPointF m_lastDragPos;
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
};
