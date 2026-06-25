#pragma once
#include <QGraphicsItem>

// کلاس مجازی مادر که تمام قطعات از آن ارث‌بری می‌کنند
class Element : public QGraphicsItem {
public:
    Element() {
        // با روشن کردن این دو فلگ، تمام قطعات مدار شما به صورت خودکار با موس جابجا و انتخاب می‌شوند!
        setFlags(ItemIsSelectable | ItemIsMovable);
    }

    virtual ~Element() = default;

    // این تابع برای هم‌تیمی شماست که بعداً منطق ریاضی مدار را بنویسد
    virtual void process() = 0;
};