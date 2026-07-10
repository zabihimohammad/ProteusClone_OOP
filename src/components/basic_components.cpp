#include "basic_components.h"
#include <QPainter>
#include <QPainterPath>
#include <QFont> // اضافه شده برای تنظیم فونت نوشته‌ها
#include "../core/terminal.h"

// ==========================================
// ۱. پیاده‌سازی کلاس مقاومت (Resistor)
// ==========================================
Resistor::Resistor() {
    // ایجاد ترمینال‌های (پایه‌های) سمت چپ و راست برای اتصال سیم‌ها
    (new Terminal(this))->setPos(-30, 0);
    (new Terminal(this))->setPos(30, 0);
}

QRectF Resistor::boundingRect() const {
    // کادر احاطه‌کننده قطعه شامل فضای لازم برای رسم شکل و نمایش متن مقدار آن
    return QRectF(-35, -25, 70, 50);
}

void Resistor::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red); // تغییر رنگ در حالت انتخاب
    painter->setPen(pen);

    // رسم خطوط زیگزاگ مقاومت (استاندارد IEEE)
    painter->drawLine(-30, 0, -20, 0);
    painter->drawLine(-20, 0, -15, -10);
    painter->drawLine(-15, -10, -5, 10);
    painter->drawLine(-5, 10, 5, -10);
    painter->drawLine(5, -10, 15, 10);
    painter->drawLine(15, 10, 20, 0);
    painter->drawLine(20, 0, 30, 0);

    // === بخش جدید: نمایش مقدار داینامیک روی بوم ===
    painter->setFont(QFont("Consolas", 8, QFont::Bold));
    painter->setPen(Qt::darkBlue); // رنگ متن برای خوانایی بهتر متفاوت است
    // چاپ متغیر resistance (مثلا 10k) بالای قطعه
    painter->drawText(QRectF(-35, -25, 70, 15), Qt::AlignCenter, resistance);
}

void Resistor::process() {
    // منطق ریاضی و مداری برای بک‌اند (موتور شبیه‌ساز)
}

// ==========================================
// ۲. پیاده‌سازی کلاس خازن (Capacitor)
// ==========================================
Capacitor::Capacitor() {
    (new Terminal(this))->setPos(-30, 0);
    (new Terminal(this))->setPos(30, 0);
}

QRectF Capacitor::boundingRect() const {
    return QRectF(-35, -30, 70, 60);
}

void Capacitor::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    // رسم پایه‌ها و صفحات موازی خازن
    painter->drawLine(-30, 0, -5, 0);
    painter->drawLine(5, 0, 30, 0);
    painter->drawLine(-5, -15, -5, 15);
    painter->drawLine(5, -15, 5, 15);

    // === بخش جدید: نمایش مقدار داینامیک ===
    painter->setFont(QFont("Consolas", 8, QFont::Bold));
    painter->setPen(Qt::darkBlue);
    painter->drawText(QRectF(-35, -30, 70, 15), Qt::AlignCenter, capacitance);
}

void Capacitor::process() {}

// ==========================================
// ۳. پیاده‌سازی باتری (DC Voltage Source)
// ==========================================
DCVoltageSource::DCVoltageSource() {
    (new Terminal(this))->setPos(-30, 0);
    (new Terminal(this))->setPos(30, 0);
}

QRectF DCVoltageSource::boundingRect() const {
    return QRectF(-40, -40, 80, 80);
}

void DCVoltageSource::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    // رسم پایه‌ها و صفحات منبع ولتاژ
    painter->drawLine(-30, 0, -10, 0);
    painter->drawLine(10, 0, 30, 0);
    painter->drawLine(-10, -20, -10, 20); // قطب مثبت (بلند)

    QPen thickPen = pen;
    thickPen.setWidth(5);
    painter->setPen(thickPen);
    painter->drawLine(10, -10, 10, 10); // قطب منفی (کوتاه و ضخیم)

    // رسم علامت +
    painter->setPen(pen);
    painter->drawLine(-25, -15, -15, -15);
    painter->drawLine(-20, -20, -20, -10);

    // === بخش جدید: نمایش مقدار داینامیک ===
    painter->setFont(QFont("Consolas", 8, QFont::Bold));
    painter->setPen(Qt::darkBlue);
    painter->drawText(QRectF(-40, -35, 80, 15), Qt::AlignCenter, voltage);
}

void DCVoltageSource::process() {}

// ==========================================
// ۴. پیاده‌سازی زمین (Ground)
// ==========================================
Ground::Ground() {
    (new Terminal(this))->setPos(0, 0);
}

QRectF Ground::boundingRect() const { return QRectF(-20, 0, 40, 30); }

void Ground::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    painter->drawLine(0, 0, 0, 15);
    painter->drawLine(-15, 15, 15, 15);
    painter->drawLine(-10, 20, 10, 20);
    painter->drawLine(-5, 25, 5, 25);

    // زمین معمولاً مقدار متنی ندارد، لذا drawText نیازی نیست.
}

void Ground::process() {}

// ==========================================
// ۵. پیاده‌سازی سلف (Inductor)
// ==========================================
Inductor::Inductor() {
    (new Terminal(this))->setPos(-30, 0);
    (new Terminal(this))->setPos(30, 0);
}

QRectF Inductor::boundingRect() const { return QRectF(-35, -25, 70, 50); }

void Inductor::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    painter->drawLine(-30, 0, -20, 0);
    painter->drawArc(-20, -10, 13, 20, 0, 180 * 16);
    painter->drawArc(-7, -10, 13, 20, 0, 180 * 16);
    painter->drawArc(6, -10, 13, 20, 0, 180 * 16);
    painter->drawLine(19, 0, 30, 0);

    // === بخش جدید: نمایش مقدار داینامیک ===
    painter->setFont(QFont("Consolas", 8, QFont::Bold));
    painter->setPen(Qt::darkBlue);
    painter->drawText(QRectF(-35, -25, 70, 15), Qt::AlignCenter, inductance);
}

void Inductor::process() {}

// ==========================================
// ۶. پیاده‌سازی پالس کلاک (Pulse Generator)
// ==========================================
PulseGenerator::PulseGenerator() {
    (new Terminal(this))->setPos(30, 0);
}

QRectF PulseGenerator::boundingRect() const { return QRectF(-25, -35, 55, 60); }

void PulseGenerator::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    painter->drawEllipse(-20, -20, 40, 40);
    painter->drawLine(-10, 10, -10, -10);
    painter->drawLine(-10, -10, 0, -10);
    painter->drawLine(0, -10, 0, 10);
    painter->drawLine(0, 10, 10, 10);
    painter->drawLine(20, 0, 30, 0);

    // === بخش جدید: نمایش فرکانس روی قطعه ===
    painter->setFont(QFont("Consolas", 7, QFont::Bold));
    painter->setPen(Qt::darkBlue);
    painter->drawText(QRectF(-25, -35, 50, 15), Qt::AlignCenter, frequency);
}

void PulseGenerator::process() {}

// ==========================================
// ۷. کلید قطع و وصل (Switch)
// ==========================================
Switch::Switch() {
    (new Terminal(this))->setPos(-30, 0);
    (new Terminal(this))->setPos(30, 0);
}

QRectF Switch::boundingRect() const { return QRectF(-35, -30, 70, 60); }

void Switch::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    painter->drawLine(-30, 0, -15, 0);
    painter->drawEllipse(-15, -2, 4, 4);

    // رسم اهرم بر اساس وضعیت اولیه (Open/Closed)
    if (initialState == "Closed") {
        painter->drawLine(-13, -2, 11, -2); // خط صاف
    } else {
        painter->drawLine(-13, -2, 10, -15); // خط کج
    }

    painter->drawEllipse(11, -2, 4, 4);
    painter->drawLine(15, 0, 30, 0);

    // === بخش جدید: نمایش وضعیت ===
    painter->setFont(QFont("Consolas", 8, QFont::Bold));
    painter->setPen(Qt::darkBlue);
    painter->drawText(QRectF(-35, -30, 70, 15), Qt::AlignCenter, initialState);
}

void Switch::process() {}

// ==========================================
// ۸. دکمه فشاری (Push Button)
// ==========================================
PushButton::PushButton() {
    (new Terminal(this))->setPos(-30, 0);
    (new Terminal(this))->setPos(30, 0);
}

QRectF PushButton::boundingRect() const { return QRectF(-35, -35, 70, 60); }

void PushButton::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    painter->drawLine(-30, 0, -10, 0);
    painter->drawEllipse(-10, -2, 4, 4);
    painter->drawLine(-15, -15, 15, -15);
    painter->drawLine(0, -15, 0, -25);
    painter->drawEllipse(6, -2, 4, 4);
    painter->drawLine(10, 0, 30, 0);

    // === بخش جدید: نمایش نوع کلید ===
    painter->setFont(QFont("Consolas", 7, QFont::Bold));
    painter->setPen(Qt::darkBlue);
    painter->drawText(QRectF(-35, -35, 70, 15), Qt::AlignCenter, type);
}

void PushButton::process() {}

// ==========================================
// ۹. دیود نورانی (LED)
// ==========================================
LED::LED() {
    (new Terminal(this))->setPos(-30, 0);
    (new Terminal(this))->setPos(30, 0);
}

QRectF LED::boundingRect() const { return QRectF(-35, -35, 70, 65); }

void LED::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    painter->drawLine(-30, 0, -10, 0);
    painter->drawLine(10, 0, 30, 0);

    QPolygonF triangle;
    triangle << QPointF(-10, -10) << QPointF(-10, 10) << QPointF(10, 0);
    painter->drawPolygon(triangle);
    painter->drawLine(10, -10, 10, 10);

    // فلش‌های نورانی
    painter->drawLine(0, -12, 10, -22);
    painter->drawLine(10, -22, 5, -22);
    painter->drawLine(10, -22, 10, -17);
    painter->drawLine(8, -8, 18, -18);
    painter->drawLine(18, -18, 13, -18);
    painter->drawLine(18, -18, 18, -13);

    // === بخش جدید: نمایش رنگ ال‌ای‌دی ===
    painter->setFont(QFont("Consolas", 8, QFont::Bold));
    painter->setPen(Qt::darkBlue);
    painter->drawText(QRectF(-35, -35, 70, 15), Qt::AlignCenter, color);
}

void LED::process() {}

// ==========================================
// ۱۰. سون‌سگمنت (7-Segment)
// ==========================================
SevenSegment::SevenSegment() {}

QRectF SevenSegment::boundingRect() const { return QRectF(-25, -45, 50, 90); }

void SevenSegment::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    painter->drawRect(-20, -30, 40, 60);

    QPen segPen(QColor(220, 220, 220), 4);
    painter->setPen(segPen);
    painter->drawLine(-10, -20, 10, -20);
    painter->drawLine(15, -15, 15, -5);
    painter->drawLine(15, 5, 15, 15);
    painter->drawLine(-10, 20, 10, 20);
    painter->drawLine(-15, 5, -15, 15);
    painter->drawLine(-15, -15, -15, -5);
    painter->drawLine(-10, 0, 10, 0);
    painter->drawEllipse(15, 20, 3, 3);

    // === بخش جدید: نمایش رنگ سون سگمنت ===
    painter->setFont(QFont("Consolas", 7, QFont::Bold));
    painter->setPen(Qt::darkBlue);
    painter->drawText(QRectF(-25, -45, 50, 15), Qt::AlignCenter, color);
}

void SevenSegment::process() {}
// ============================================================================
// پیاده‌سازی Ground (زمین)
// ============================================================================
Ground::Ground() {
    outGnd = new Terminal(this);
    outGnd->setPos(0, -20);
}
QRectF Ground::boundingRect() const { return QRectF(-20, -25, 40, 45); }
void Ground::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    painter->drawLine(0, -20, 0, 0); // سیم پایه
    painter->drawLine(-15, 0, 15, 0); // خط اول
    painter->drawLine(-10, 5, 10, 5); // خط دوم
    painter->drawLine(-5, 10, 5, 10); // خط سوم
}
void Ground::process() {
    outGnd->setVoltage(0.0); // مرجع ولتاژ صفر
}

// ============================================================================
// پیاده‌سازی DC Voltage Source
// ============================================================================
DCVoltageSource::DCVoltageSource() {
    outPos = new Terminal(this);
    outPos->setPos(0, -20);
}
QRectF DCVoltageSource::boundingRect() const { return QRectF(-20, -25, 40, 50); }
void DCVoltageSource::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    painter->drawLine(0, -20, 0, -10);
    painter->drawEllipse(-10, -10, 20, 20);
    painter->drawLine(-5, 0, 5, 0); // علامت +
    painter->drawLine(0, -5, 0, 5);

    painter->setFont(QFont("Consolas", 8));
    painter->drawText(QRectF(-20, 15, 40, 15), Qt::AlignCenter, voltage);
}
void DCVoltageSource::process() {
    double v = voltage.replace("V", "").toDouble();
    outPos->setVoltage(v); // تزریق ولتاژ تنظیمی به مدار
}
QMap<QString, QString> DCVoltageSource::getProperties() const {
    QMap<QString, QString> props;
    props["Voltage"] = voltage;
    return props;
}
void DCVoltageSource::setProperties(const QMap<QString, QString>& props) {
    if (props.contains("Voltage")) voltage = props["Voltage"];
}

// ============================================================================
// پیاده‌سازی Clock Generator
// ============================================================================
ClockGenerator::ClockGenerator() {
    outClk = new Terminal(this);
    outClk->setPos(25, 0);
}
QRectF ClockGenerator::boundingRect() const { return QRectF(-25, -25, 55, 65); }
void ClockGenerator::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    painter->drawRect(-20, -20, 40, 40);
    painter->drawLine(20, 0, 25, 0); // سیم خروجی

    // رسم گرافیک موج مربعی
    painter->drawLine(-10, 10, -5, 10);
    painter->drawLine(-5, 10, -5, -10);
    painter->drawLine(-5, -10, 5, -10);
    painter->drawLine(5, -10, 5, 10);
    painter->drawLine(5, 10, 10, 10);

    painter->setFont(QFont("Consolas", 7));
    painter->drawText(QRectF(-25, 25, 50, 15), Qt::AlignCenter, frequency);
}
void ClockGenerator::process() {
    // اگر کلاک در وضعیت بالا بود ولتاژ اعمال کن، وگرنه 0 ولت
    double v = currentState ? amplitude.replace("V", "").toDouble() : 0.0;
    outClk->setVoltage(v);
}
void ClockGenerator::toggleClock() {
    currentState = !currentState; // برعکس کردن وضعیت صفر و یک
    update();
}
QMap<QString, QString> ClockGenerator::getProperties() const {
    QMap<QString, QString> props;
    props["Frequency"] = frequency;
    props["Amplitude"] = amplitude;
    return props;
}
void ClockGenerator::setProperties(const QMap<QString, QString>& props) {
    if (props.contains("Frequency")) frequency = props["Frequency"];
    if (props.contains("Amplitude")) amplitude = props["Amplitude"];
}