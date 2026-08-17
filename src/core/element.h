#pragma once
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QMap>
#include <QJsonObject>
#include "terminal.h"
#include "wire.h"
#include "../gui/properties_dialog.h"
#include <QPainter>
#include <QFontMetrics>

class Element : public QGraphicsItem {
public:
    Element() {
        setFlags(ItemIsSelectable | ItemIsMovable | ItemSendsGeometryChanges);
    }
    virtual ~Element() = default;

    virtual void process() = 0;
    virtual QString getComponentName() const { return "Component"; }
    virtual QMap<QString, QString> getProperties() const { return QMap<QString, QString>(); }
    virtual void setProperties(const QMap<QString, QString>& props) {}

    // وضعیت زمان اجرای قطعه
    virtual QJsonObject getDynamicState() const { return QJsonObject(); }
    virtual void setDynamicState(const QJsonObject& state) {}
    virtual void resetSimulationState() {
        for (QGraphicsItem *child : childItems()) {
            if (Terminal *terminal = dynamic_cast<Terminal *>(child)) {
                terminal->resetState();
            }
        }
        update();
    }

protected:
    void drawReadableText(QPainter *painter, const QRectF &rect, int flags, const QString &text) {
        painter->save();
        bool isMirrored = (transform().m11() < 0);
        qreal currentRotation = rotation();
        QPointF center = rect.center();
        painter->translate(center);
        if (currentRotation != 0.0) painter->rotate(-currentRotation);
        if (isMirrored) painter->scale(-1, 1);
        QRectF localRect(-rect.width() / 2.0, -rect.height() / 2.0, rect.width(), rect.height());
        painter->drawText(localRect, flags, text);
        painter->restore();
    }

    void drawReadableText(QPainter *painter, double x, double y, const QString &text) {
        painter->save();
        bool isMirrored = (transform().m11() < 0);
        qreal currentRotation = rotation();
        painter->translate(x, y);
        if (currentRotation != 0.0) painter->rotate(-currentRotation);
        if (isMirrored) {
            painter->scale(-1, 1);
            QFontMetrics fm(painter->font());
            double textWidth = fm.horizontalAdvance(text);
            if (x > 0) painter->drawText(QPointF(-textWidth, 0), text);
            else painter->drawText(QPointF(0, 0), text);
        } else {
            painter->drawText(QPointF(0, 0), text);
        }
        painter->restore();
    }

    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override {
        if (change == ItemPositionHasChanged) {
            for (QGraphicsItem *child : childItems()) {
                Terminal *term = dynamic_cast<Terminal*>(child);
                if (term) {
                    for (Wire *wire : term->getConnectedWires()) {
                        if (wire) wire->updateRoute();
                    }
                }
            }
        }
        return QGraphicsItem::itemChange(change, value);
    }

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            QMap<QString, QString> currentProps = getProperties();
            if (currentProps.isEmpty()) { event->accept(); return; }
            PropertiesDialog dialog(getComponentName(), currentProps);
            if (dialog.exec() == QDialog::Accepted) {
                setProperties(dialog.getUpdatedProperties());
                update();
            }
            event->accept();
            return;
        }
        QGraphicsItem::mouseDoubleClickEvent(event);
    }
};
