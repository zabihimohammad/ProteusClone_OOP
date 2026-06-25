#include "mcu.h"
#include <QPainter>
#include "../gui/hex_upload_dialog.h"
#include <QGraphicsSceneMouseEvent>
#include <QMessageBox>
#include "../core/terminal.h"
MCUChip::MCUChip() {
    for (int i = -20; i <= 20; i += 20) {
        (new Terminal(this))->setPos(-50, i); // پایه‌های سمت چپ
        (new Terminal(this))->setPos(50, i);  // پایه‌های سمت راست
        (new Terminal(this))->setPos(i, -50); // پایه‌های بالا
        (new Terminal(this))->setPos(i, 50);  // پایه‌های پایین
    }
}
QRectF MCUChip::boundingRect() const { return QRectF(-60, -60, 120, 120); }
void MCUChip::process() {}

void MCUChip::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    // رسم بدنه اصلی IC
    painter->setBrush(QColor(40, 40, 40)); // رنگ خاکستری تیره برای داخل تراشه
    painter->drawRect(-40, -40, 80, 80);

    // رسم نقطه راهنمای پایه 1 (بالا سمت چپ)
    painter->setBrush(Qt::white);
    painter->drawEllipse(-30, -30, 5, 5);

    // نوشتن نام تراشه در مرکز
    painter->setPen(Qt::white);
    painter->drawText(QRectF(-40, -40, 80, 80), Qt::AlignCenter, "MCU\nCore");

    // رسم پایه‌ها (Pins)
    painter->setPen(pen);
    for (int i = -20; i <= 20; i += 20) {
        painter->drawLine(-50, i, -40, i); // پایه‌های چپ
        painter->drawLine(40, i, 50, i);   // پایه‌های راست
        painter->drawLine(i, -50, i, -40); // پایه‌های بالا
        painter->drawLine(i, 40, i, 50);   // پایه‌های پایین
    }
}
void MCUChip::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    // جلوگیری از تداخل رویداد با جابجایی عادی قطعه
    event->accept();

    // ساخت یک نمونه از پنجره آپلود
    // از ()event->widget برای پیدا کردن پنجره والد استفاده می‌کنیم
    HexUploadDialog dialog(event->widget());

    // نمایش پنجره به صورت صلب (Modal). برنامه منتظر می‌ماند تا کاربر پنجره را ببندد
    if (dialog.exec() == QDialog::Accepted) {
        QString selectedFile = dialog.getSelectedFilePath();

        if (!selectedFile.isEmpty()) {
            hexFilePath = selectedFile; // ذخیره مسیر فایل برای استفاده بک‌اند

            // نمایش یک پیغام موفقیت‌آمیز به کاربر
            QMessageBox::information(
                    event->widget(),
                    "Firmware Loaded",
                    "HEX file successfully mapped to MCU memory!\nPath: " + hexFilePath
            );
        }
    }
}