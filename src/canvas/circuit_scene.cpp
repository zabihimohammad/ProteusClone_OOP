#include "circuit_scene.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData> // اضافه شده برای پشتیبانی از Drag & Drop
#include <QPainter>
#include <QPen>
#include <QColor>
#include <QKeyEvent>
#include <QDebug>

#include "../core/terminal.h"
#include "../core/wire.h"
#include "../core/auto_router.h"
#include "../core/element.h"
#include "../components/mcu.h"
#include "../components/basic_components.h"
#include "../components/logic_gates.h"
#include "../components/peripherals.h"
#include "../io/file_manager.h"

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

// ==========================================
// رویدادهای موس (سیم‌کشی و جابجایی)
// ==========================================
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

                    tempWire->setFullRoute(smartPath);
                    tempWire->confirmConnection(clickedTerminal);

                    isWiring = false;
                    tempWire = nullptr;
                    startTerminal = nullptr;

                    // ثبت در تاریخچه برای کارکردن Undo بعد از سیم‌کشی
                    FileManager::recordState(this);
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

void CircuitScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsScene::mouseReleaseEvent(event);

    // اگر قطعه‌ای در حال جابجایی (Drag) بوده و حالا رها شده، یک عکس برای Undo بگیریم
    if (!isWiring && selectedItems().count() > 0) {
        FileManager::recordState(this);
    }
}

// ==========================================
// رویداد فشردن کلیدهای کیبورد (میانبرها و حذف)
// ==========================================
void CircuitScene::keyPressEvent(QKeyEvent *event) {
    // --- 1. سیستم پاک کردن قطعات (Delete) ---
    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        bool somethingDeleted = false;
        for (QGraphicsItem *item : selectedItems()) {
            removeItem(item);
            delete item;
            somethingDeleted = true;
        }
        // اگر چیزی پاک شد، وضعیت جدید را برای Undo ثبت کن
        if (somethingDeleted) {
            FileManager::recordState(this);
            qDebug() << "[CircuitScene] Items deleted and state recorded.";
        }
        return;
    }

    // --- 2. میانبرهای ترکیبی با Control ---
    if (event->modifiers() & Qt::ControlModifier) {
        // 💾 سیستم Save
        if (event->key() == Qt::Key_S) {
            FileManager::saveCircuit("my_circuit_test.json", this);
            qDebug() << "[TEST] Circuit Saved to my_circuit_test.json";
            return;
        }
        // 📂 سیستم Load
        if (event->key() == Qt::Key_O) {
            FileManager::loadCircuit("my_circuit_test.json", this);
            qDebug() << "[TEST] Circuit Loaded!";
            return;
        }
        // ↩️ سیستم Undo / Redo
        if (event->key() == Qt::Key_Z) {
            if (event->modifiers() & Qt::ShiftModifier) {
                FileManager::redo(this);
            } else {
                FileManager::undo(this);
            }
            return;
        } else if (event->key() == Qt::Key_Y) {
            FileManager::redo(this);
            return;
        }
    }
    QGraphicsScene::keyPressEvent(event);
}

// ==========================================
// سیستم Drag & Drop (کشیدن و رها کردن قطعات)
// ==========================================
void CircuitScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event) {
    if (event->mimeData()->hasText()) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void CircuitScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event) {
    if (event->mimeData()->hasText()) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void CircuitScene::dropEvent(QGraphicsSceneDragDropEvent *event) {
    if (event->mimeData()->hasText()) {
        QString componentType = event->mimeData()->text();
        QPointF dropPosition = event->scenePos();

        // چسباندن خودکار قطعه به خطوط شبکه (Snap to Grid) برای تمیزی مدار
        dropPosition.setX(qRound(dropPosition.x() / gridSize) * gridSize);
        dropPosition.setY(qRound(dropPosition.y() / gridSize) * gridSize);

        QGraphicsItem *newItem = nullptr;

        // کارخانه تولید قطعات
        if (componentType == "MCU") newItem = new MCUChip();
        else if (componentType == "RESISTOR") newItem = new Resistor();
        else if (componentType == "CAPACITOR") newItem = new Capacitor();
        else if (componentType == "AND_GATE") newItem = new AndGate();
        else if (componentType == "OR_GATE") newItem = new OrGate();
        else if (componentType == "LED") newItem = new LED();

        // اگر قطعه با موفقیت ساخته شد
        if (newItem != nullptr) {
            newItem->setPos(dropPosition);
            addItem(newItem);

            // ثبت وضعیت در تاریخچه
            FileManager::recordState(this);
        }

        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}