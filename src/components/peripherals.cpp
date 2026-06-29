#include "peripherals.h"
#include <QPainter>
#include <QFont> // برای تنظیم فونت نوشته‌های داینامیک
#include "../core/terminal.h"
#include "../core/wire.h"

// ==========================================
// ۱. حافظه خارجی (RAM/EEPROM)
// ==========================================
MemoryChip::MemoryChip() {
    // ایجاد پایه‌های آدرس و داده با حلقه
    for (int i = -40; i <= 40; i += 10) {
        (new Terminal(this))->setPos(-50, i); // خطوط آدرس (چپ)
        (new Terminal(this))->setPos(50, i);  // خطوط داده (راست)
    }
}

QRectF MemoryChip::boundingRect() const {
    // افزایش ارتفاع کادر برای جا دادن حجم حافظه و نام فایل هگز
    return QRectF(-60, -75, 120, 150);
}

void MemoryChip::process() {}

void MemoryChip::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    // رسم بدنه آی‌سی
    painter->setBrush(QColor(50, 50, 50));
    painter->drawRect(-40, -50, 80, 100);

    // چاپ نام تراشه
    painter->setPen(Qt::white);
    painter->drawText(QRectF(-40, -50, 80, 100), Qt::AlignCenter, "RAM\nEEPROM");

    // رسم خطوط اتصال پایه‌ها
    painter->setPen(pen);
    for (int i = -40; i <= 40; i += 10) {
        painter->drawLine(-50, i, -40, i); // Address lines
        painter->drawLine(40, i, 50, i);   // Data lines
    }

    // === نمایش داینامیک ظرفیت حافظه و وضعیت فریمور ===
    painter->setFont(QFont("Consolas", 8, QFont::Bold));

    // چاپ ظرفیت حافظه در بالای تراشه
    painter->setPen(Qt::darkBlue);
    painter->drawText(QRectF(-60, -70, 120, 15), Qt::AlignCenter, memorySize);

    // چاپ وضعیت فایل هگز در پایین تراشه
    if (initialHexPath == "Not Loaded" || initialHexPath.isEmpty()) {
        painter->setPen(Qt::red);
        painter->drawText(QRectF(-60, 55, 120, 15), Qt::AlignCenter, "NO HEX");
    } else {
        painter->setPen(QColor(0, 150, 0)); // سبز تیره
        painter->drawText(QRectF(-60, 55, 120, 15), Qt::AlignCenter, "HEX OK");
    }
}

// ==========================================
// ۲. نمایشگر کاراکتری (LCD 16x2)
// ==========================================
LCD16x2::LCD16x2() {
    int startX = -55;
    for(int i = 0; i < 14; i++) {
        (new Terminal(this))->setPos(startX + (i * 8), -35);
    }
}

QRectF LCD16x2::boundingRect() const {
    return QRectF(-70, -55, 140, 90);
}

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

    // پایه‌های اتصال (بالای LCD)
    int startX = -55;
    for(int i=0; i<14; i++) {
        painter->drawLine(startX + (i*8), -25, startX + (i*8), -35);
    }

    // === نمایش داینامیک پروتکل ارتباطی ===
    painter->setFont(QFont("Consolas", 7, QFont::Bold));
    painter->setPen(Qt::darkBlue);
    QString infoText = busMode;
    if (busMode == "I2C") infoText += " (" + i2cAddress + ")";
    painter->drawText(QRectF(-70, 30, 140, 15), Qt::AlignCenter, infoText);
}

// ==========================================
// ۳. صفحه کلید ماتریسی (Keypad 4x4)
// ==========================================
Keypad::Keypad() {
    for(int i = 0; i < 4; i++) {
        (new Terminal(this))->setPos(-30 + (i * 20), 45);
    }
}

QRectF Keypad::boundingRect() const {
    return QRectF(-45, -50, 90, 120);
}

void Keypad::process() {}

void Keypad::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    // بدنه کیپد
    painter->setBrush(QColor(200, 200, 200));
    painter->drawRect(-35, -45, 70, 75);

    // رسم دکمه‌های 4x4
    painter->setBrush(Qt::white);
    for(int row = -40; row < 25; row += 16) {
        for(int col = -30; col < 30; col += 15) {
            painter->drawRect(col, row, 12, 12);
        }
    }

    // رسم خطوط پایه‌ها
    for(int i = -30; i <= 30; i += 20) {
        painter->drawLine(i, 30, i, 45);
    }

    // === نمایش داینامیک زمان دی‌بانس (Debounce) ===
    painter->setFont(QFont("Consolas", 7, QFont::Bold));
    painter->setPen(Qt::darkBlue);
    painter->drawText(QRectF(-45, 55, 90, 15), Qt::AlignCenter, "DB: " + debounceTimeMs);
}

// ==========================================
// ۴. مبدل آنالوگ به دیجیتال (ADC)
// ==========================================
ADC_Chip::ADC_Chip() {
    (new Terminal(this))->setPos(-40, 0); // Vin
    for(int i = -20; i <= 20; i += 10) {
        (new Terminal(this))->setPos(40, i); // Digital Out
    }
}

QRectF ADC_Chip::boundingRect() const {
    return QRectF(-65, -55, 120, 110);
}

void ADC_Chip::process() {}

void ADC_Chip::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    // بدنه مبدل
    painter->setBrush(QColor(70, 130, 180));
    painter->drawRect(-30, -30, 60, 60);
    painter->setPen(Qt::white);
    painter->drawText(QRectF(-30, -30, 60, 60), Qt::AlignCenter, "ADC");

    // پایه ورودی آنالوگ
    painter->setPen(pen);
    painter->drawLine(-40, 0, -30, 0);
    painter->drawText(-55, 5, "Vin");

    // پایه‌های خروجی دیجیتال
    for(int i = -20; i <= 20; i += 10) {
        painter->drawLine(30, i, 40, i);
    }

    // === نمایش داینامیک رزولوشن و ولتاژ مرجع ===
    painter->setFont(QFont("Consolas", 7, QFont::Bold));
    painter->setPen(Qt::darkBlue);
    painter->drawText(QRectF(-65, -50, 120, 15), Qt::AlignCenter, resolutionBits);
    painter->drawText(QRectF(-65, 35, 120, 15), Qt::AlignCenter, "Vref: " + referenceVoltage);
}

// ==========================================
// ۵. مبدل دیجیتال به آنالوگ (DAC)
// ==========================================
DAC_Chip::DAC_Chip() {
    (new Terminal(this))->setPos(40, 0); // Vout
    for(int i = -20; i <= 20; i += 10) {
        (new Terminal(this))->setPos(-40, i); // Digital In
    }
}

QRectF DAC_Chip::boundingRect() const {
    return QRectF(-65, -55, 120, 110);
}

void DAC_Chip::process() {}

void DAC_Chip::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    // بدنه مبدل
    painter->setBrush(QColor(210, 105, 30));
    painter->drawRect(-30, -30, 60, 60);
    painter->setPen(Qt::white);
    painter->drawText(QRectF(-30, -30, 60, 60), Qt::AlignCenter, "DAC");

    // پایه خروجی آنالوگ
    painter->setPen(pen);
    painter->drawLine(30, 0, 40, 0);
    painter->drawText(45, 5, "Vout");

    // پایه‌های ورودی دیجیتال
    for(int i = -20; i <= 20; i += 10) {
        painter->drawLine(-40, i, -30, i);
    }

    // === نمایش داینامیک رزولوشن و ولتاژ خروجی ===
    painter->setFont(QFont("Consolas", 7, QFont::Bold));
    painter->setPen(Qt::darkBlue);
    painter->drawText(QRectF(-65, -50, 120, 15), Qt::AlignCenter, resolutionBits);
    painter->drawText(QRectF(-65, 35, 120, 15), Qt::AlignCenter, "Max: " + maxOutputVoltage);
}