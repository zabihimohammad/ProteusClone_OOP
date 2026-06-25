#pragma once
#include <QGraphicsItem>

// کلاس ترمینال (پایه اتصال) که روی قطعات قرار می‌گیرد
class Terminal : public QGraphicsItem {
public:
    // سازنده: قطعه‌ای که این پایه متعلق به آن است (parent) را دریافت می‌کند
    explicit Terminal(QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

protected:
    // بازنویسی رویدادهای موس برای تغییر رنگ هنگام نزدیک شدن نشانگر (Hover)
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    // رویداد کلیک برای شروع سیم‌کشی
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    bool isHovered; // متغیری برای تشخیص اینکه آیا موس روی پایه است یا خیر
};