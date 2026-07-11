#pragma once
#include <QGraphicsItem>
#include <QString>
#include <QPainter>

class ProbeItem : public QGraphicsItem {
public:
    ProbeItem(QGraphicsItem *parent = nullptr);

    // فقط همین یک تابع برای آپدیت پروب کافیست (ساده و امن)
    void updateProbe(const QString &voltage, const QPointF &pos);

    // توابع اجباری Qt برای رسم گرافیک اختصاصی
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    // فقط یک رشته متنی ساده، بدون هیچ پوینتري (*)
    QString currentVoltage;
};