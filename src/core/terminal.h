#pragma once
#include <QGraphicsItem>
#include <QList>
#include <QString>

class Wire;

class Terminal : public QGraphicsItem {
public:
    explicit Terminal(QGraphicsItem *parent = nullptr);
    ~Terminal() override;

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void addWire(Wire *wire);
    void removeWire(Wire *wire);
    QList<Wire*> getConnectedWires() const;

    void setVoltage(double v);
    double getVoltage() const;
    void setUndefined();

    bool isFloating() const;
    bool getLogicState() const;
    bool isUndefinedState() const;

    bool isDriven() const { return _isDriven; }

    void resetState();

    QString voltageLevel = "Undefined";
    double exactVoltage = 0.0; // ذخیره ولتاژ دقیق و بدون گرد شدن برای فیزیک

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    bool isHovered;
    QList<Wire*> connectedWires;

    double _voltage;
    bool _isDriven;
    bool _isUndefined;
};