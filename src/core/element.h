#pragma once
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QMap> // اضافه شد برای سیستم داینامیک
#include "terminal.h"
#include "wire.h"
#include "../gui/properties_dialog.h"

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

    // ==========================================
    // توابع سیستم داینامیک تنظیمات (مخصوص ارث‌بری قطعات)
    // ==========================================

    // ۱. برگرداندن نام قطعه جهت نمایش در هدر پنجره
    virtual QString getComponentName() const {
        return "Component";
    }

    // ۲. ارسال لیست ویژگی‌ها و مقادیر پیش‌فرض آن‌ها به دیالوگ
    virtual QMap<QString, QString> getProperties() const {
        return QMap<QString, QString>(); // پیش‌فرض: بدون تنظیمات
    }

    // ۳. دریافت مقادیر جدید تایپ‌شده توسط کاربر و ذخیره آن‌ها در قطعه
    virtual void setProperties(const QMap<QString, QString>& props) {
        // قطعات فرزند، کدهای ذخیره‌سازی خود را اینجا می‌نویسند
    }

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

    // رهگیری دابل‌کلیک جهت باز کردن تنظیمات
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override {
        // ۱. وقتی کاربر دابل‌کلیک کرد، فقط کلیک چپ را قبول کن
        if (event->button() == Qt::LeftButton) {

            // ۲. دریافت ویژگی‌های قطعه از توابع مجازی
            QMap<QString, QString> currentProps = getProperties();

            // اگر این قطعه هیچ ویژگی‌ای برای تنظیم ندارد، بی‌خیال شو
            if (currentProps.isEmpty()) {
                qDebug() << "No properties defined for this component.";
                event->accept();
                return;
            }

            // ۳. ساخت دیالوگ داینامیک با ارسال نام و ویژگی‌های قطعه
            PropertiesDialog dialog(getComponentName(), currentProps);

            // ۴. اگر کاربر تغییرات را ذخیره کرد (Save Changes)
            if (dialog.exec() == QDialog::Accepted) {

                // لیست جدید ویژگی‌ها را بگیر و به قطعه تزریق کن
                setProperties(dialog.getUpdatedProperties());

                // به بوم بگو قطعه را دوباره رسم کند (تا اگر متنی روی قطعه هست آپدیت شود)
                update();

                qDebug() << "Properties successfully updated for" << getComponentName();
            }

            // رویداد مصرف شد
            event->accept();
            return;
        }

        // اجرای رفتار پیش‌فرض در صورت نیاز
        QGraphicsItem::mouseDoubleClickEvent(event);
    }
};