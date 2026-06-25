#pragma once
#include <QGraphicsItem>

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
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
};