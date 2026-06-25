#pragma once
#include <QGraphicsView>
#include <QWheelEvent>
#include <QMouseEvent>

class CircuitView : public QGraphicsView {
    Q_OBJECT
public:
    explicit CircuitView(QWidget *parent = nullptr);

protected:
    // بازنویسی رویداد چرخاندن غلتک موس (اسکرول) برای ایجاد قابلیت Zoom In / Zoom Out
    void wheelEvent(QWheelEvent *event) override;

    // بازنویسی رویدادهای کلیک موس برای قابلیت جابجایی صفحه (Panning)
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    bool isPanning;       // پرچمی که نشان می‌دهد آیا در حال کشیدن صفحه هستیم یا خیر
    QPoint lastMousePos;  // ذخیره آخرین مختصات موس برای محاسبه میزان جابجایی
};