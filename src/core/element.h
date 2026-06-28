#pragma once
#include <QGraphicsItem>
#include "terminal.h"  // برای شناختن پین‌ها و ترمینال‌ها
#include "wire.h"      // برای شناختن سیم‌ها و آپدیت مسیر آن‌ها

// کلاس مجازی مادر که تمام قطعات از آن ارث‌بری می‌کنند
class Element : public QGraphicsItem {
public:
    Element() {
        // فلگ جدید ItemSendsGeometryChanges به موتور Qt می‌گوید که تغییرات مختصات را قبل از اعمال نهایی به ما گزارش دهد
        setFlags(ItemIsSelectable | ItemIsMovable | ItemSendsGeometryChanges);
    }

    virtual ~Element() = default;

    // این تابع برای هم‌تیمی شماست که بعداً منطق ریاضی مدار را بنویسد
    virtual void process() = 0;

protected:
    // تابعی از Qt که مختصات را هنگام کشیدن با موس، رهگیری و اصلاح می‌کند
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override {
        // اگر قطعه در حال جابجایی روی بوم است
        if (change == ItemPositionHasChanged) {

            // ۱. روی تمام فرزندان این قطعه (که همان ترمینال‌ها/پایه‌ها هستند) حلقه می‌زنیم
            for (QGraphicsItem *child : childItems()) {

                // ۲. بررسی می‌کنیم که آیا این فرزند واقعاً یک "ترمینال" است؟
                Terminal *term = dynamic_cast<Terminal*>(child);
                if (term) {

                    // ۳. اگر ترمینال بود، به تمام سیم‌های متصل به آن می‌گوییم مسیرشان را دوباره محاسبه کنند
                    for (Wire *wire : term->getConnectedWires()) {
                        if (wire) {
                            wire->updateRoute(); // فراخوانی مجدد الگوریتم A*
                        }
                    }
                }
            }
        }

        // در نهایت، اجازه می‌دهیم Qt کار اصلی خودش (جابجا کردن قطعه) را انجام دهد
        return QGraphicsItem::itemChange(change, value);
    }
};