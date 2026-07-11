//terminal.h
#pragma once
#include <QGraphicsItem>
#include <QList>
class Wire;
// کلاس ترمینال (پایه اتصال) که روی قطعات قرار می‌گیرد
class Terminal : public QGraphicsItem {
public:
    // سازنده: قطعه‌ای که این پایه متعلق به آن است (parent) را دریافت می‌کند
    explicit Terminal(QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
//
// اضافه کردن این ۳ خط برای مدیریت سیم‌های متصل
    void addWire(Wire *wire);
    void removeWire(Wire *wire);
    QList<Wire*> getConnectedWires() const;
    QString voltageLevel = "Undefined";
    ~Terminal() override;
protected:
    // بازنویسی رویدادهای موس برای تغییر رنگ هنگام نزدیک شدن نشانگر (Hover)
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;


    // رویداد کلیک برای شروع سیم‌کشی
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
   // void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
private:
    bool isHovered; // متغیری برای تشخیص اینکه آیا موس روی پایه است یا خیر
    QList<Wire*> connectedWires;
};