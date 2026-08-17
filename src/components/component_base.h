#pragma once
#include <QGraphicsItem>
#include "../core/terminal.h"
#include "../core/wire.h"

// پایه مشترک قطعات مدار
class ComponentBase : public QGraphicsItem {
public:
    ComponentBase() {
        setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    }

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override {
        if (change == ItemPositionHasChanged) {
            // مسیر سیم‌های متصل را تازه کن.
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
