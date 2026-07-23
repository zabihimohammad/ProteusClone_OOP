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

    setCanvasSize(m_canvasRect.size());
    voltageProbe = new ProbeItem();
    addItem(voltageProbe);
    isProbeEnabled = false;
}

void CircuitScene::setGridSize(int size) {
    m_gridSize = qBound(8, size, 64);
    invalidate(sceneRect(), QGraphicsScene::BackgroundLayer);
}

void CircuitScene::setCanvasSize(const QSizeF &size) {
    if (size.width() < 1 || size.height() < 1) return;
    m_canvasRect = QRectF(-size.width() / 2.0, -size.height() / 2.0,
                         size.width(), size.height());
    const qreal margin = qMax(size.width(), size.height()) * 2.0;
    setSceneRect(m_canvasRect.adjusted(-margin, -margin, margin, margin));
    invalidate(sceneRect(), QGraphicsScene::BackgroundLayer);
}

void CircuitScene::drawBackground(QPainter *painter, const QRectF &rect) {
    // پر کردن پس‌زمینه با رنگ ملایم
    painter->fillRect(rect, QColor("#E9EDF2"));
    painter->fillRect(m_canvasRect, QColor("#FBFCFD"));

    // تنظیم قلم برای نقاط گرید
    QPen pen;
    pen.setColor(QColor(199, 207, 218, 180));
    pen.setWidth(1);
    painter->setPen(pen);

    // بهینه‌سازی رسم ناحیه قابل دید
    int left = int(rect.left()) - (int(rect.left()) % m_gridSize);
    int top = int(rect.top()) - (int(rect.top()) % m_gridSize);

    // رسم نقاط روی بوم
    for (int x = left; x < rect.right(); x += m_gridSize) {
        for (int y = top; y < rect.bottom(); y += m_gridSize) {
            painter->drawPoint(x, y);
        }
    }
    painter->setPen(QPen(QColor("#1473E6"), 2));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(m_canvasRect);
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
            if (tempWire) {
                removeItem(tempWire);
                delete tempWire;
            }
            isWiring = false;
            tempWire = nullptr;
            startTerminal = nullptr;
            event->accept();
            return;
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
QGraphicsItem *CircuitScene::addComponent(const QString &componentType, const QPointF &position) {
    QGraphicsItem *item = nullptr;
    if (componentType == "MCU") item = new MCUChip();
    else if (componentType == "RESISTOR") item = new Resistor();
    else if (componentType == "CAPACITOR") item = new Capacitor();
    else if (componentType == "INDUCTOR") item = new Inductor();
    else if (componentType == "DC_SOURCE") item = new DCVoltageSource();
    else if (componentType == "GROUND") item = new Ground();
    else if (componentType == "AND_GATE") item = new AndGate();
    else if (componentType == "OR_GATE") item = new OrGate();
    else if (componentType == "NOT_GATE") item = new NotGate();
    else if (componentType == "XOR_GATE") item = new XorGate();
    else if (componentType == "NAND_GATE") item = new NandGate();
    else if (componentType == "D_FLIP_FLOP") item = new DFlipFlop();
    else if (componentType == "LED") item = new LED();
    else if (componentType == "SWITCH") item = new Switch();
    else if (componentType == "PUSH_BUTTON") item = new PushButton();
    else if (componentType == "SEVEN_SEGMENT") item = new SevenSegment();
    else if (componentType == "PULSE_GENERATOR") item = new PulseGenerator();
    else if (componentType == "MEMORY") item = new MemoryChip();
    else if (componentType == "LCD") item = new LCD16x2();
    else if (componentType == "KEYPAD") item = new Keypad();
    else if (componentType == "ADC") item = new ADC_Chip();
    else if (componentType == "DAC") item = new DAC_Chip();

    if (!item) return nullptr;
    QPointF finalPosition = position;
    if (m_snapEnabled) {
        finalPosition.setX(qRound(finalPosition.x() / m_gridSize) * m_gridSize);
        finalPosition.setY(qRound(finalPosition.y() / m_gridSize) * m_gridSize);
    }
    item->setPos(finalPosition);
    addItem(item);
    FileManager::recordState(this);
    return item;
}

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
        // قطعه را دقیقاً در محلی که موس رها شده با استفاده از سیستم Snap می‌سازد
        addComponent(event->mimeData()->text(), event->scenePos());
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}
