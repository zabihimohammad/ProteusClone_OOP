#pragma once
#include <QGraphicsScene>
#include <QPainter>
#include <QRectF>

class CircuitScene : public QGraphicsScene {
    Q_OBJECT
public:
    explicit CircuitScene(QObject *parent = nullptr);

protected:
    // این تابع هسته گرافیکی ما برای پس‌زمینه است.
    // موتور Qt به صورت خودکار هر زمان که نیاز به رسم مجدد صفحه باشد (مثلا وقتی زوم می‌کنید) این تابع را صدا می‌زند.
    void drawBackground(QPainter *painter, const QRectF &rect) override;

private:
    int gridSize; // فاصله بین نقطه‌های شطرنجی (مثلاً ۲۰ پیکسل)
};
