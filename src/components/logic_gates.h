#pragma once
#include "../core/element.h"

class AndGate : public Element {
public:
    AndGate();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;
};// AND, OR, NOT, D-FlipFlop
// ==========================================
// گیت OR
// ==========================================
class OrGate : public Element {
public:
    OrGate();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;
};

// ==========================================
// گیت NOT
// ==========================================
class NotGate : public Element {
public:
    NotGate();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;
};

// ==========================================
// گیت XOR
// ==========================================
class XorGate : public Element {
public:
    XorGate();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;
};

// ==========================================
// گیت NAND
// ==========================================
class NandGate : public Element {
public:
    NandGate();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;
};

// ==========================================
// فلیپ‌فلاپ (D-FlipFlop)
// ==========================================
class DFlipFlop : public Element {
public:
    DFlipFlop();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;
};