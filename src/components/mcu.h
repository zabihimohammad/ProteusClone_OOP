#pragma once
#include "../core/element.h"

class MCUChip : public Element {
public:
    MCUChip();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;
    // (کدهای قبلی کلاس MCUChip ...)
protected:
    // بازنویسی رویداد دبل‌کلیک موس در Qt Graphics View
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QString hexFilePath; // متغیری که مسیر فریمورک آپلود شده را در خود نگه می‌دارد
};
// Microcontroller Core and IO
