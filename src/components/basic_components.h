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
// ==========================================
// کلاس خازن
// ==========================================
class Capacitor : public Element {
public:
    Capacitor();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;
};

// ==========================================
// کلاس منبع ولتاژ مستقیم (باتری)
// ==========================================
class DCVoltageSource : public Element {
public:
    DCVoltageSource();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;
};
// ==========================================
// کلاس زمین (GND)
// ==========================================
class Ground : public Element {
public:
    Ground();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;
};

// ==========================================
// کلاس سلف (Inductor)
// ==========================================
class Inductor : public Element {
public:
    Inductor();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;
};
// ==========================================
// پالس کلاک (Pulse Generator)
// ==========================================
class PulseGenerator : public Element {
public:
    PulseGenerator();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;
};

// ==========================================
// کلید قطع و وصل (Switch)
// ==========================================
class Switch : public Element {
public:
    Switch();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;
};

// ==========================================
// دکمه فشاری (Push Button)
// ==========================================
class PushButton : public Element {
public:
    PushButton();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;
};

// ==========================================
// دیود نورانی (LED)
// ==========================================
class LED : public Element {
public:
    LED();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;
};

// ==========================================
// سون‌سگمنت (7-Segment Display)
// ==========================================
class SevenSegment : public Element {
public:
    SevenSegment();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;
};