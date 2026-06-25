#pragma once
#include "../core/element.h"

// ==========================================
// حافظه خارجی (RAM / EEPROM)
// ==========================================
class MemoryChip : public Element {
public:
    MemoryChip();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;
protected:
    // اضافه کردن این تابع برای شنود حرکت قطعه
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
};

// ==========================================
// نمایشگر کاراکتری (LCD 16x2)
// ==========================================
class LCD16x2 : public Element {
public:
    LCD16x2();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;
};

// ==========================================
// صفحه کلید ماتریسی (Keypad 4x4)
// ==========================================
class Keypad : public Element {
public:
    Keypad();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;
};

// ==========================================
// مبدل آنالوگ به دیجیتال (ADC)
// ==========================================
class ADC_Chip : public Element {
public:
    ADC_Chip();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;
};

// ==========================================
// مبدل دیجیتال به آنالوگ (DAC)
// ==========================================
class DAC_Chip : public Element {
public:
    DAC_Chip();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;
};