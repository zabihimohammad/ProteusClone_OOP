#include "mcu.h"
#include <QPainter>
#include <QFont> // برای تنظیم فونت نوشته‌ها
#include "../core/terminal.h"

// ==========================================
// سازنده (Constructor) میکروکنترلر
// ==========================================
MCUChip::MCUChip() {
    // ایجاد پایه‌ها (پین‌ها) در ۴ طرف میکروکنترلر (شبیه پکیج‌های QFP یا DIP)
    for (int i = -20; i <= 20; i += 20) {
        (new Terminal(this))->setPos(-50, i); // پایه‌های سمت چپ
        (new Terminal(this))->setPos(50, i);  // پایه‌های سمت راست
        (new Terminal(this))->setPos(i, -50); // پایه‌های بالا
        (new Terminal(this))->setPos(i, 50);  // پایه‌های پایین
    }
}

// ==========================================
// تعیین محدوده کلیک و رسم قطعه
// ==========================================
QRectF MCUChip::boundingRect() const {
    return QRectF(-60, -60, 120, 120);
}

void MCUChip::process() {
    // منطق اجرای کدهای هگز در موتور شبیه‌ساز (مرحله بک‌اند)
}

// ==========================================
// رسم گرافیکی میکروکنترلر روی بوم
// ==========================================
void MCUChip::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    // ۱. رسم بدنه اصلی IC
    painter->setBrush(QColor(40, 40, 40)); // رنگ خاکستری تیره (شبیه پلاستیک IC)
    painter->drawRect(-40, -40, 80, 80);

    // ۲. رسم نقطه راهنمای پایه شماره 1 (بالا سمت چپ)
    painter->setBrush(Qt::white);
    painter->drawEllipse(-30, -30, 5, 5);

    // ۳. رسم پایه‌های فلزی (Pins)
    painter->setPen(pen);
    for (int i = -20; i <= 20; i += 20) {
        painter->drawLine(-50, i, -40, i); // پایه‌های چپ
        painter->drawLine(40, i, 50, i);   // پایه‌های راست
        painter->drawLine(i, -50, i, -40); // پایه‌های بالا
        painter->drawLine(i, 40, i, 50);   // پایه‌های پایین
    }

    // ==========================================
    // ۴. نمایش اطلاعات داینامیک روی بدنه تراشه
    // ==========================================

    // چاپ نام تراشه
    painter->setPen(Qt::white);
    painter->setFont(QFont("Consolas", 10, QFont::Bold));
    painter->drawText(QRectF(-40, -30, 80, 20), Qt::AlignCenter, "MCU");

    // چاپ فرکانس کاری
    painter->setFont(QFont("Consolas", 7));
    painter->setPen(QColor(200, 200, 200)); // خاکستری روشن
    painter->drawText(QRectF(-40, -10, 80, 20), Qt::AlignCenter, clockFrequency);

    // بررسی وضعیت فایل هگز و نمایش آن با رنگ مناسب
    QString hexStatusText;
    if (hexFilePath == "Not Loaded" || hexFilePath.isEmpty()) {
        painter->setPen(QColor(255, 100, 100)); // رنگ قرمز (هشدار: کد ندارد)
        hexStatusText = "NO HEX";
    } else {
        painter->setPen(QColor(100, 255, 100)); // رنگ سبز (آماده اجرا)
        hexStatusText = "HEX OK";
    }
    painter->drawText(QRectF(-40, 10, 80, 20), Qt::AlignCenter, hexStatusText);
}