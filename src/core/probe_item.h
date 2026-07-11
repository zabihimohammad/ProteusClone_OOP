#pragma once
#include <QGraphicsItem>
#include <QString>
#include <QPainter>

class ProbeItem : public QGraphicsItem {
public:
    ProbeItem(QGraphicsItem *parent = nullptr);

    // تابعی برای آپدیت کردن متن و موقعیت پروب
    void updateProbe(const QString &voltage, const QPointF &pos);

    // توابع اجباری Qt برای رسم گرافیک اختصاصی
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    QString currentVoltage;
};