#pragma once
#include <QGraphicsItem>
#include "../core/terminal.h"
#include "../core/wire.h"

// تمام قطعات شما در آینده باید از این کلاس ارث ببرند
class ComponentBase : public QGraphicsItem {
public:
    ComponentBase() {
        setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    }

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override {
        if (change == ItemPositionHasChanged) {
            // پیدا کردن تمام ترمینال‌های فرزند و آپدیت سیم‌های آن‌ها
            for (QGraphicsItem *child : childItems()) {
                Terminal *term = dynamic_cast<Terminal*>(child);
                if (term) {
                    for (Wire *wire : term->getConnectedWires()) {
                        wire->updateRoute();
                    }
                }
            }
        }
        return QGraphicsItem::itemChange(change, value);
    }
};