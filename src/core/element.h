//element.h
#pragma once
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QMap> // اضافه شد برای سیستم داینامیک
#include "terminal.h"
#include "wire.h"
#include "../gui/properties_dialog.h"
#include <QPainter>       // 🛠️ اضافه شد برای رفع خطاهای incomplete type
#include <QFontMetrics>   // 🛠️ اضافه شد برای محاسبه ابعاد فونت
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
    // 🛠️ نسخه هوشمند ۱: رسم متن کادردار با خنثی‌سازی همزمان میرور و چرخش
    void drawReadableText(QPainter *painter, const QRectF &rect, int flags, const QString &text) {
        painter->save();

        // تشخیص وضعیت میرور و چرخش فعلی قطعه
        bool isMirrored = (transform().m11() < 0);
        qreal currentRotation = rotation();

        // رفتن به مرکز کادر متن برای چرخاندن و قرینه کردن قلم دور خود متن
        QPointF center = rect.center();
        painter->translate(center);

        // ۱. خنثی کردن زاویه قطعه (چرخاندن پینتر در جهت عکس)
        if (currentRotation != 0.0) {
            painter->rotate(-currentRotation);
        }

        // ۲. خنثی کردن میرور افقی
        if (isMirrored) {
            painter->scale(-1, 1);
        }

        // رسم متن در نقطه صفر جدید (چون پینتر روی مرکز کادر ست شده است)
        QRectF localRect(-rect.width() / 2.0, -rect.height() / 2.0, rect.width(), rect.height());
        painter->drawText(localRect, flags, text);

        painter->restore();
    }

    // 🛠️ نسخه هوشمند ۲: رسم متن نقطه‌ای با خنثی‌سازی همزمان میرور و چرخش (برای پین‌ها)
    void drawReadableText(QPainter *painter, double x, double y, const QString &text) {
        painter->save();

        bool isMirrored = (transform().m11() < 0);
        qreal currentRotation = rotation();

        // انتقال پینتر به نقطه دقیق بیس متن
        painter->translate(x, y);

        // خنثی‌سازی زاویه
        if (currentRotation != 0.0) {
            painter->rotate(-currentRotation);
        }

        // خنثی‌سازی میرور
        if (isMirrored) {
            painter->scale(-1, 1);
            QFontMetrics fm(painter->font());
            double textWidth = fm.horizontalAdvance(text);
            // اصلاح آفست متن در حالت میرور
            if (x > 0) painter->drawText(QPointF(-textWidth, 0), text);
            else painter->drawText(QPointF(0, 0), text);
        } else {
            painter->drawText(QPointF(0, 0), text);
        }

        painter->restore();
    }
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