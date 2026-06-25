#pragma once
#include "../core/element.h"

class Resistor : public Element {
public:
    Resistor();

    // دو تابع اجباری برای رسم گرافیکی
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    // پیاده‌سازی تابع محاسباتی (خالی برای بخش فرانت‌اند)
    void process() override;
};// Resistors, Capacitors, Sources
