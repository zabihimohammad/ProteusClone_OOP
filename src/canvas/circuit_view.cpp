#include "circuit_view.h"
#include <QScrollBar>

CircuitView::CircuitView(QWidget *parent) : QGraphicsView(parent), isPanning(false) {
    // روشن کردن آنتی‌آلیاسینگ (Anti-aliasing) برای نرم‌تر و زیباتر شدن خطوط و دایره‌ها
    setRenderHint(QPainter::Antialiasing);

    // وقتی زوم می‌کنیم، زوم به جای مرکز صفحه، دقیقاً روی نقطه‌ای انجام شود که نشانگر موس قرار دارد
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    // مخفی کردن نوارهای اسکرول کنار صفحه (چون با کلیک وسط موس جابجا می‌شویم)
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

// --- بخش اول: پیاده‌سازی زوم ---
void CircuitView::wheelEvent(QWheelEvent *event) {
    // در Qt 6 برای گرفتن زاویه چرخش غلتک از angleDelta استفاده می‌شود.
    // اگر مقدار y مثبت باشد یعنی غلتک به جلو (Zoom In) چرخیده است.
    const double scaleFactor = 1.15; // سرعت و شدت زوم (۱۵ درصد بزرگنمایی در هر پله)

    if (event->angleDelta().y() > 0) {
        // زوم به داخل
        scale(scaleFactor, scaleFactor);
    } else {
        // زوم به بیرون
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }
}

// --- بخش دوم: پیاده‌سازی جابجایی صفحه (Panning) ---
void CircuitView::mousePressEvent(QMouseEvent *event) {
    // اگر کاربر کلیک وسط (غلتک) را فشار داد
    if (event->button() == Qt::MiddleButton) {
        isPanning = true; // حالت کشیدن فعال می‌شود
        lastMousePos = event->pos(); // مختصات فعلی موس را به عنوان نقطه شروع ذخیره می‌کنیم
        setCursor(Qt::ClosedHandCursor); // شکل نشانگر موس را به یک مشت بسته (در حال گرفتن) تغییر می‌دهیم
        event->accept();
    } else {
        // اگر کلیک‌های دیگری بود، روال عادی Qt انجام شود (مثلاً برای انتخاب قطعات)
        QGraphicsView::mousePressEvent(event);
    }
}

void CircuitView::mouseMoveEvent(QMouseEvent *event) {
    // اگر در حالت کشیدن هستیم
    if (isPanning) {
        // محاسبه می‌کنیم موس از کلیک قبلی چقدر جابجا شده است
        int deltaX = event->pos().x() - lastMousePos.x();
        int deltaY = event->pos().y() - lastMousePos.y();

        // نوارهای اسکرول نامرئی را دقیقاً به همان اندازه در جهت معکوس حرکت می‌دهیم تا تصویر جابجا شود
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - deltaX);
        verticalScrollBar()->setValue(verticalScrollBar()->value() - deltaY);

        // مختصات جدید را برای فریم بعدی ذخیره می‌کنیم
        lastMousePos = event->pos();
        event->accept();
    } else {
        QGraphicsView::mouseMoveEvent(event);
    }
}

void CircuitView::mouseReleaseEvent(QMouseEvent *event) {
    // وقتی کاربر دکمه وسط موس را رها کرد
    if (event->button() == Qt::MiddleButton) {
        isPanning = false; // پایان کشیدن
        setCursor(Qt::ArrowCursor); // نشانگر موس به حالت فلش عادی برگردد
        event->accept();
    } else {
        QGraphicsView::mouseReleaseEvent(event);
    }
}
