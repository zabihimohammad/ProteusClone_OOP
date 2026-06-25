#include "circuit_scene.h"
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QPen>
#include <QColor>
#include "../core/terminal.h"
#include "../core/wire.h"
#include "../core/auto_router.h"
#include <QKeyEvent>
CircuitScene::CircuitScene(QObject *parent)
        : QGraphicsScene(parent), isWiring(false), tempWire(nullptr), startTerminal(nullptr) {

    gridSize = 20; // تنظیم ابعاد شبکه شطرنجی

    // تنظیم ابعاد بی‌نهایت برای بوم (از مختصات -5000 تا +5000)
    setSceneRect(-5000, -5000, 10000, 10000);
}

void CircuitScene::drawBackground(QPainter *painter, const QRectF &rect) {
    // پر کردن پس‌زمینه با رنگ ملایم
    painter->fillRect(rect, QColor(245, 245, 245));

    // تنظیم قلم برای نقاط گرید
    QPen pen;
    pen.setColor(QColor(200, 200, 200));
    pen.setWidth(1);
    painter->setPen(pen);

    // بهینه‌سازی رسم ناحیه قابل دید
    int left = int(rect.left()) - (int(rect.left()) % gridSize);
    int top = int(rect.top()) - (int(rect.top()) % gridSize);

    // رسم نقاط روی بوم
    for (int x = left; x < rect.right(); x += gridSize) {
        for (int y = top; y < rect.bottom(); y += gridSize) {
            painter->drawPoint(x, y);
        }
    }
}

void CircuitScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {

        QGraphicsItem *item = itemAt(event->scenePos(), QTransform());
        Terminal *clickedTerminal = dynamic_cast<Terminal*>(item);

        if (clickedTerminal) {
            if (!isWiring) {
                // شروع سیم‌کشی
                isWiring = true;
                startTerminal = clickedTerminal;

                QPointF startPos = clickedTerminal->sceneBoundingRect().center();
                tempWire = new Wire(startTerminal, startPos);
                addItem(tempWire);
                return;
            } else {
                // --- پایان سیم‌کشی (وصل شدن به پایه دوم) ---
                if (clickedTerminal != startTerminal) {
                    QPointF startP = startTerminal->sceneBoundingRect().center();
                    QPointF endP = clickedTerminal->sceneBoundingRect().center();

                    // فراخوانی مسیریاب خودکار هوشمند
                    QVector<QPointF> smartPath = AutoRouter::findPath(this, startP, endP, startTerminal, clickedTerminal, tempWire);

                    tempWire->setFullRoute(smartPath); // تزریق مسیر هوشمند به سیم
                    tempWire->confirmConnection(clickedTerminal);

                    isWiring = false;
                    tempWire = nullptr;
                    startTerminal = nullptr;
                    return;
                }
            }
        } else if (isWiring) {
            // لغو سیم‌کشی با کلیک روی فضای خالی
            removeItem(tempWire);
            delete tempWire;
            isWiring = false;
            tempWire = nullptr;
            startTerminal = nullptr;
        }
    }

    QGraphicsScene::mousePressEvent(event);
}

void CircuitScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (isWiring && tempWire) {
        tempWire->setEndPoint(event->scenePos());
    }
    QGraphicsScene::mouseMoveEvent(event);
}
// ==========================================
// رویداد فشردن کلیدهای کیبورد (برای حذف قطعات)
// ==========================================
void CircuitScene::keyPressEvent(QKeyEvent *event) {
    // اگر کاربر کلید Delete یا Backspace را فشار داد
    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {

        // لیست تمام آیتم‌هایی که انتخاب شده‌اند را بگیر
        QList<QGraphicsItem*> itemsToRemove = selectedItems();

        for (QGraphicsItem *item : itemsToRemove) {
            // ۱. ابتدا آیتم را از روی بوم پاک کن
            removeItem(item);

            // ۲. سپس آن را از حافظه سیستم کاملاً حذف کن (جلوگیری از Memory Leak)
            delete item;
        }
    }

    // اجرای رویدادهای پیش‌فرض کیبورد
    QGraphicsScene::keyPressEvent(event);
}