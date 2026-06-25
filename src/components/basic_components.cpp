#include "basic_components.h"
#include <QPainter>
#include <QPainterPath> // برای رسم شکل‌های پیچیده‌تر
#include "../core/terminal.h"
Resistor::Resistor() {
    // ساخت ترمینال سمت چپ و قرار دادن آن در مختصات انتهای سیم چپ
    Terminal *t1 = new Terminal(this);
    t1->setPos(-30, 0);

    // ساخت ترمینال سمت راست
    Terminal *t2 = new Terminal(this);
    t2->setPos(30, 0);
}

QRectF Resistor::boundingRect() const {
    // یک مستطیل فرضی با عرض ۷۰ و ارتفاع ۳۰ که مرکز آن نقطه (0,0) است
    return QRectF(-35, -15, 70, 30);
}

void Resistor::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    // تنظیم ضخامت قلم و رنگ پیش‌فرض (مشکی)
    QPen pen(Qt::black, 2);

    // اگر کاربر روی مقاومت کلیک کرد، رنگ آن قرمز شود تا مشخص شود انتخاب شده است
    if (isSelected()) {
        pen.setColor(Qt::red);
    }
    painter->setPen(pen);

    // رسم پایه‌ها و خطوط زیگزاگ مقاومت با استفاده از مختصات هندسی دقیق
    painter->drawLine(-30, 0, -20, 0);       // پایه چپ
    painter->drawLine(-20, 0, -15, -10);     // شروع زیگزاگ به بالا
    painter->drawLine(-15, -10, -5, 10);     // پایین
    painter->drawLine(-5, 10, 5, -10);       // بالا
    painter->drawLine(5, -10, 15, 10);       // پایین
    painter->drawLine(15, 10, 20, 0);        // پایان زیگزاگ
    painter->drawLine(20, 0, 30, 0);         // پایه راست
}

void Resistor::process() {
    // منطق جریان و ولتاژ که بعداً نوشته می‌شود
}
// ==========================================
// پیاده‌سازی ظاهر گرافیکی خازن
// ==========================================
Capacitor::Capacitor() {
    (new Terminal(this))->setPos(-30, 0);
    (new Terminal(this))->setPos(30, 0);
}

QRectF Capacitor::boundingRect() const {
    // کادر فرضی خازن: کمی بلندتر از مقاومت در نظر می‌گیریم تا صفحات خازن جا شوند
    return QRectF(-35, -25, 70, 50);
}

void Capacitor::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red); // تغییر رنگ در صورت انتخاب شدن
    painter->setPen(pen);

    // ۱. رسم سیم پایه‌های ورودی و خروجی (خط افقی)
    painter->drawLine(-30, 0, -5, 0); // سیم سمت چپ تا رسیدن به جوشن خازن
    painter->drawLine(5, 0, 30, 0);   // سیم سمت راست

    // ۲. رسم صفحات موازی خازن (خطوط عمودی)
    painter->drawLine(-5, -15, -5, 15); // صفحه سمت چپ (جوشن اول)
    painter->drawLine(5, -15, 5, 15);   // صفحه سمت راست (جوشن دوم)
}

void Capacitor::process() {}


// ==========================================
// پیاده‌سازی ظاهر گرافیکی منبع تغذیه (باتری)
// ==========================================
DCVoltageSource::DCVoltageSource() {
    (new Terminal(this))->setPos(-30, 0);
    (new Terminal(this))->setPos(30, 0);
}

QRectF DCVoltageSource::boundingRect() const {
    return QRectF(-40, -30, 80, 60);
}

void DCVoltageSource::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    // ۱. رسم سیم پایه‌های ورودی و خروجی
    painter->drawLine(-30, 0, -10, 0); // سیم سمت چپ (متصل به قطب مثبت)
    painter->drawLine(10, 0, 30, 0);   // سیم سمت راست (متصل به قطب منفی)

    // ۲. رسم قطب مثبت (خط عمودی بلند و نازک)
    painter->drawLine(-10, -20, -10, 20);

    // ۳. رسم قطب منفی (خط عمودی کوتاه اما ضخیم‌تر)
    QPen thickPen = pen;
    thickPen.setWidth(5); // ضخامت قلم را برای قطب منفی بیشتر می‌کنیم
    painter->setPen(thickPen);
    painter->drawLine(10, -10, 10, 10);

    // ۴. رسم علامت (+) کوچک در بالا سمت چپ برای زیبایی و راهنمایی کاربر
    painter->setPen(pen); // برگرداندن قلم به ضخامت عادی
    painter->drawLine(-25, -15, -15, -15); // خط افقی مثبت
    painter->drawLine(-20, -20, -20, -10); // خط عمودی مثبت
}

void DCVoltageSource::process() {}
// --- پیاده‌سازی زمین (GND) ---
Ground::Ground() {
    (new Terminal(this))->setPos(0, 0);
}
QRectF Ground::boundingRect() const { return QRectF(-20, 0, 40, 30); }
void Ground::process() {}

void Ground::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    painter->drawLine(0, 0, 0, 15);       // سیم اتصال عمودی
    painter->drawLine(-15, 15, 15, 15);   // خط افقی اول (بزرگ)
    painter->drawLine(-10, 20, 10, 20);   // خط افقی دوم (متوسط)
    painter->drawLine(-5, 25, 5, 25);     // خط افقی سوم (کوچک)
}

// --- پیاده‌سازی سلف (Inductor) ---
Inductor::Inductor() {
    (new Terminal(this))->setPos(-30, 0);
    (new Terminal(this))->setPos(30, 0);
}
QRectF Inductor::boundingRect() const { return QRectF(-35, -15, 70, 30); }
void Inductor::process() {}

void Inductor::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    painter->drawLine(-30, 0, -20, 0); // پایه چپ
    // رسم 3 حلقه سلف با استفاده از کمان (Arc)
    // در Qt زاویه‌ها بر اساس 1/16 درجه محاسبه می‌شوند (لذا در 16 ضرب شده است)
    painter->drawArc(-20, -10, 13, 20, 0, 180 * 16);
    painter->drawArc(-7, -10, 13, 20, 0, 180 * 16);
    painter->drawArc(6, -10, 13, 20, 0, 180 * 16);
    painter->drawLine(19, 0, 30, 0);   // پایه راست
}


// --- پالس کلاک ---
PulseGenerator::PulseGenerator() {
    (new Terminal(this))->setPos(30, 0);
}
QRectF PulseGenerator::boundingRect() const { return QRectF(-25, -25, 50, 50); }
void PulseGenerator::process() {}
void PulseGenerator::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);
    // رسم دایره اصلی و شکل موج مربعی داخل آن
    painter->drawEllipse(-20, -20, 40, 40);
    painter->drawLine(-10, 10, -10, -10); // خط عمودی بالا
    painter->drawLine(-10, -10, 0, -10);  // خط افقی بالا
    painter->drawLine(0, -10, 0, 10);     // خط عمودی پایین
    painter->drawLine(0, 10, 10, 10);     // خط افقی پایین
    painter->drawLine(20, 0, 30, 0);      // پایه خروجی
}

// --- کلید قطع و وصل (Switch) ---
Switch::Switch() {
    (new Terminal(this))->setPos(-30, 0);
    (new Terminal(this))->setPos(30, 0);
}
QRectF Switch::boundingRect() const { return QRectF(-35, -25, 70, 50); }
void Switch::process() {}
void Switch::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);
    painter->drawLine(-30, 0, -15, 0);    // پایه چپ
    painter->drawEllipse(-15, -2, 4, 4);  // گره چپ
    painter->drawLine(-13, -2, 10, -15);  // اهرم کلید (در حالت باز)
    painter->drawEllipse(11, -2, 4, 4);   // گره راست
    painter->drawLine(15, 0, 30, 0);      // پایه راست
}

// --- دکمه فشاری (Push Button) ---
PushButton::PushButton() {
    (new Terminal(this))->setPos(-30, 0);
    (new Terminal(this))->setPos(30, 0);
}
QRectF PushButton::boundingRect() const { return QRectF(-35, -30, 70, 50); }
void PushButton::process() {}
void PushButton::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);
    painter->drawLine(-30, 0, -10, 0);    // پایه چپ
    painter->drawEllipse(-10, -2, 4, 4);  // گره چپ
    painter->drawLine(-15, -15, 15, -15); // صفحه فلزی دکمه (بالا)
    painter->drawLine(0, -15, 0, -25);    // شاسی فشاری دکمه
    painter->drawEllipse(6, -2, 4, 4);    // گره راست
    painter->drawLine(10, 0, 30, 0);      // پایه راست
}

// --- دیود نورانی (LED) ---
LED::LED() {
    (new Terminal(this))->setPos(-30, 0);
    (new Terminal(this))->setPos(30, 0);
}
QRectF LED::boundingRect() const { return QRectF(-35, -30, 70, 60); }
void LED::process() {}
void LED::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);
    painter->drawLine(-30, 0, -10, 0); // آند
    painter->drawLine(10, 0, 30, 0);   // کاتد
    // مثلث دیود
    QPolygonF triangle;
    triangle << QPointF(-10, -10) << QPointF(-10, 10) << QPointF(10, 0);
    painter->drawPolygon(triangle);
    painter->drawLine(10, -10, 10, 10); // خط کاتد
    // فلش‌های نور (خارج‌شونده)
    painter->drawLine(0, -12, 10, -22);
    painter->drawLine(10, -22, 5, -22);
    painter->drawLine(10, -22, 10, -17);
    painter->drawLine(8, -8, 18, -18);
    painter->drawLine(18, -18, 13, -18);
    painter->drawLine(18, -18, 18, -13);
}

// --- سون‌سگمنت (7-Segment) ---
SevenSegment::SevenSegment() {}
QRectF SevenSegment::boundingRect() const { return QRectF(-25, -35, 50, 70); }
void SevenSegment::process() {}
void SevenSegment::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);
    painter->drawRect(-20, -30, 40, 60); // قاب کلی نمایشگر

    // رسم یک عدد 8 خاموش (به صورت خطوط ملایم) به عنوان الگو
    QPen segPen(QColor(220, 220, 220), 4);
    painter->setPen(segPen);
    painter->drawLine(-10, -20, 10, -20); // A
    painter->drawLine(15, -15, 15, -5);   // B
    painter->drawLine(15, 5, 15, 15);     // C
    painter->drawLine(-10, 20, 10, 20);   // D
    painter->drawLine(-15, 5, -15, 15);   // E
    painter->drawLine(-15, -15, -15, -5); // F
    painter->drawLine(-10, 0, 10, 0);     // G (مرکز)
    painter->drawEllipse(15, 20, 3, 3);   // ممیز (DP)
}
