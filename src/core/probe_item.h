#pragma once
#include <QGraphicsItem>
#include <QString>
#include <QPainter>

class ProbeItem : public QGraphicsItem {
public:
    ProbeItem(QGraphicsItem *parent = nullptr);

    // مقدار نمایشی پروب
    void updateProbe(const QString &voltage, const QPointF &pos);

    // رسم پروب
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    // متن ولتاژ
    QString currentVoltage;
};