#include "peripherals.h"
#include <QPainter>

// --- حافظه خارجی (RAM/EEPROM) ---
MemoryChip::MemoryChip() {}
QRectF MemoryChip::boundingRect() const { return QRectF(-50, -60, 100, 120); }
void MemoryChip::process() {}
void MemoryChip::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    painter->setBrush(QColor(50, 50, 50));
    painter->drawRect(-40, -50, 80, 100); // بدنه آی‌سی

    painter->setPen(Qt::white);
    painter->drawText(QRectF(-40, -50, 80, 100), Qt::AlignCenter, "RAM\nEEPROM");

    painter->setPen(pen);
    // خطوط آدرس (چپ) و داده (راست)
    for (int i = -40; i <= 40; i += 10) {
        painter->drawLine(-50, i, -40, i); // Address lines
        painter->drawLine(40, i, 50, i);   // Data lines
    }
}

// --- نمایشگر کاراکتری (LCD 16x2) ---
LCD16x2::LCD16x2() {}
QRectF LCD16x2::boundingRect() const { return QRectF(-70, -35, 140, 70); }
void LCD16x2::process() {}
void LCD16x2::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    // قاب دور LCD (سبز تیره)
    painter->setBrush(QColor(34, 139, 34));
    painter->drawRect(-60, -25, 120, 50);

    // صفحه نمایشگر (سبز روشن)
    painter->setBrush(QColor(152, 251, 152));
    painter->drawRect(-50, -15, 100, 30);

    // پایه‌های اتصال (14 یا 16 پایه در بالای LCD)
    int startX = -55;
    for(int i=0; i<14; i++) {
        painter->drawLine(startX + (i*8), -25, startX + (i*8), -35);
    }
}

// --- صفحه کلید ماتریسی (Keypad) ---
Keypad::Keypad() {}
QRectF Keypad::boundingRect() const { return QRectF(-40, -50, 80, 100); }
void Keypad::process() {}
void Keypad::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    painter->setBrush(QColor(200, 200, 200));
    painter->drawRect(-35, -45, 70, 75); // بدنه کیپد

    // رسم دکمه‌های 4x4
    painter->setBrush(Qt::white);
    for(int row = -40; row < 25; row += 16) {
        for(int col = -30; col < 30; col += 15) {
            painter->drawRect(col, row, 12, 12);
        }
    }

    // 8 پایه خروجی (سطر و ستون) در پایین
    for(int i = -20; i <= 20; i += 6) {
        painter->drawLine(i, 30, i, 45);
    }
}

// --- مبدل آنالوگ به دیجیتال (ADC) ---
ADC_Chip::ADC_Chip() {}
QRectF ADC_Chip::boundingRect() const { return QRectF(-65, -40, 105, 80); }
void ADC_Chip::process() {}
void ADC_Chip::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    painter->setBrush(QColor(70, 130, 180)); // آبی فولادی
    painter->drawRect(-30, -30, 60, 60);
    painter->setPen(Qt::white);
    painter->drawText(QRectF(-30, -30, 60, 60), Qt::AlignCenter, "ADC");

    painter->setPen(pen);
    painter->drawLine(-40, 0, -30, 0); // ورودی آنالوگ
    painter->drawText(-55, 5, "Vin");

    for(int i = -20; i <= 20; i += 6) {
        painter->drawLine(30, i, 40, i); // خروجی‌های دیجیتال موازی
    }
}

// --- مبدل دیجیتال به آنالوگ (DAC) ---
DAC_Chip::DAC_Chip() {}
QRectF DAC_Chip::boundingRect() const { return QRectF(-40, -40, 110, 80); }
void DAC_Chip::process() {}
void DAC_Chip::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    painter->setBrush(QColor(210, 105, 30)); // نارنجی تیره
    painter->drawRect(-30, -30, 60, 60);
    painter->setPen(Qt::white);
    painter->drawText(QRectF(-30, -30, 60, 60), Qt::AlignCenter, "DAC");

    painter->setPen(pen);
    painter->drawLine(30, 0, 40, 0); // خروجی آنالوگ
    painter->drawText(45, 5, "Vout");

    for(int i = -20; i <= 20; i += 6) {
        painter->drawLine(-40, i, -30, i); // ورودی‌های دیجیتال موازی
    }
}