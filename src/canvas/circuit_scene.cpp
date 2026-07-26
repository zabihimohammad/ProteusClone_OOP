#include "circuit_scene.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QPainter>
#include <QPen>
#include <QColor>
#include <QKeyEvent>
#include <QDebug>
#include <QGraphicsView>
#include "../core/terminal.h"
#include "../core/wire.h"
#include "../core/auto_router.h"
#include "../core/element.h"
#include "../components/mcu.h"
#include "../components/basic_components.h"
#include "../components/logic_gates.h"
#include "../components/peripherals.h"
#include "../io/file_manager.h"
#include <QLineF>
static QPointF m_crosshairPos; // اضافه کردن این متغیر برای ثبت موقعیت نشانگر متقاطع
// تابع قدرتمند و ۱۰۰٪ دقیق: پروجکشن برداری برای یافتن نقطه برخورد روی سیم
static QPointF splitWireExactly(Wire* clickedWire, QPointF targetPos, JunctionNode* junction, QGraphicsScene* scene) {
    Terminal *targetStart = clickedWire->getStartTerminal();
    Terminal *targetEnd = clickedWire->getEndTerminal();
    QVector<QPointF> oldPath = clickedWire->getPoints();

    QVector<QPointF> path1, path2;
    bool splitFound = false;

    double minDistance = 1e9;
    int bestSegment = 0;
    QPointF exactPos = targetPos;

    // ۱. ریاضیات برداری برای پیدا کردن نزدیک‌ترین نقطه کاملاً منطبق بر سیم
    for (int i = 0; i < oldPath.size() - 1; ++i) {
        QPointF a = oldPath[i];
        QPointF b = oldPath[i+1];
        QPointF proj;

        double dx = b.x() - a.x();
        double dy = b.y() - a.y();

        if (qAbs(dx) < 0.1 && qAbs(dy) < 0.1) {
            proj = a;
        } else {
            double t = ((targetPos.x() - a.x()) * dx + (targetPos.y() - a.y()) * dy) / (dx * dx + dy * dy);
            t = qBound(0.0, t, 1.0);
            proj = QPointF(a.x() + t * dx, a.y() + t * dy);
        }

        double dist = QLineF(targetPos, proj).length();
        if (dist < minDistance) {
            minDistance = dist;
            bestSegment = i;
            exactPos = proj; // این نقطه تضمین می‌کند که از روی سیم خارج نمی‌شویم
        }
    }

    // ۲. بریدن سیم و تراز کردن نقاط
    for (int i = 0; i < oldPath.size() - 1; ++i) {
        QPointF A = oldPath[i];
        QPointF B = oldPath[i+1];

        if (!splitFound) {
            path1.append(A);
            if (i == bestSegment) {
                // فیکس طلایی: جلوگیری از هرگونه خطای اعشاری در خطوط صاف
                if (qAbs(A.x() - B.x()) < 0.5) exactPos.setX(A.x());
                if (qAbs(A.y() - B.y()) < 0.5) exactPos.setY(A.y());

                path1.append(exactPos);
                path2.append(exactPos);
                path2.append(B);
                splitFound = true;
            }
        } else {
            path2.append(B);
        }
    }

    if (!splitFound) {
        path1 = {targetStart->sceneBoundingRect().center(), exactPos};
        path2 = {exactPos, targetEnd->sceneBoundingRect().center()};
    }

    junction->setPos(exactPos);

    scene->removeItem(clickedWire);
    delete clickedWire;

    Wire *w1 = new Wire(targetStart, targetStart->sceneBoundingRect().center());
    w1->confirmConnection(junction->term);
    scene->addItem(w1);
    w1->setFullRoute(path1);

    Wire *w2 = new Wire(junction->term, junction->term->sceneBoundingRect().center());
    w2->confirmConnection(targetEnd);
    scene->addItem(w2);
    w2->setFullRoute(path2);

    return exactPos;
}

CircuitScene::CircuitScene(QObject *parent)
        : QGraphicsScene(parent), isWiring(false), tempWire(nullptr), startTerminal(nullptr) {
    setFocusOnTouch(true);
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
    painter->fillRect(rect, QColor("#E9EDF2"));
    painter->fillRect(m_canvasRect, QColor("#FBFCFD"));

    QPen pen;
    pen.setColor(QColor(199, 207, 218, 180));
    pen.setWidth(1);
    painter->setPen(pen);

    int left = int(rect.left()) - (int(rect.left()) % m_gridSize);
    int top = int(rect.top()) - (int(rect.top()) % m_gridSize);

    for (int x = left; x < rect.right(); x += m_gridSize) {
        for (int y = top; y < rect.bottom(); y += m_gridSize) {
            painter->drawPoint(x, y);
        }
    }
    painter->setPen(QPen(QColor("#1473E6"), 2));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(m_canvasRect);
}

void CircuitScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        setWiringMode(false); // 🛠️ لغو هوشمند با راست‌کلیک
        event->accept();
        return;
    }

    if (event->button() == Qt::LeftButton) {
        QPointF rawPos = event->scenePos();
        QPointF dropPos = rawPos;
        if (m_snapEnabled) {
            dropPos.setX(qRound(dropPos.x() / m_gridSize) * m_gridSize);
            dropPos.setY(qRound(dropPos.y() / m_gridSize) * m_gridSize);
        }

        // جستجو برای یافتن قطعه، ترمینال یا سیم زیر موس
        QGraphicsItem *targetItem = nullptr;
        QList<QGraphicsItem*> exactItems = items(rawPos);
        if (exactItems.isEmpty()) exactItems = items(QRectF(rawPos.x() - 8, rawPos.y() - 8, 16, 16));

        for (QGraphicsItem *it : exactItems) {
            if (it == tempWire) continue;
            if (dynamic_cast<Terminal*>(it) || dynamic_cast<Wire*>(it)) {
                targetItem = it;
                break;
            }
        }

        Terminal *clickedTerminal = dynamic_cast<Terminal*>(targetItem);
        Wire *clickedWire = dynamic_cast<Wire*>(targetItem);

        // 🛠️ هوشمندسازی UX: اگر کاربر روی یک ترمینال کلیک کرد، خودکار وارد حالت سیم‌کشی شو!
        if (clickedTerminal && !m_wiringMode) {
            setWiringMode(true);
        }

        if (!m_wiringMode) {
            QGraphicsScene::mousePressEvent(event); // واگذاری به Qt برای Select و Drag
            return;
        }

        if (!isWiring) {
            if (clickedTerminal) {
                isWiring = true;
                startTerminal = clickedTerminal;
                tempWire = new Wire(startTerminal, startTerminal->sceneBoundingRect().center());
                addItem(tempWire);
                return;
            } else if (clickedWire) {
                JunctionNode *junction = new JunctionNode();
                addItem(junction);
                QPointF exactPos = splitWireExactly(clickedWire, m_snapEnabled ? dropPos : rawPos, junction, this);
                isWiring = true;
                startTerminal = junction->term;
                tempWire = new Wire(startTerminal, exactPos);
                addItem(tempWire);
                return;
            }
        } else {
            if (clickedTerminal) {
                if (clickedTerminal != startTerminal) {
                    QVector<QPointF> smartPath = AutoRouter::findPath(this, startTerminal->sceneBoundingRect().center(), clickedTerminal->sceneBoundingRect().center(), startTerminal, clickedTerminal, tempWire);
                    tempWire->setFullRoute(smartPath);
                    tempWire->confirmConnection(clickedTerminal);
                    isWiring = false; tempWire = nullptr; startTerminal = nullptr;
                    setWiringMode(false); // خروج خودکار از حالت سیم‌کشی بعد از اتصال موفق
                    FileManager::recordState(this);
                }
                return;
            } else if (clickedWire) {
                JunctionNode *junction = new JunctionNode();
                addItem(junction);
                QPointF exactPos = splitWireExactly(clickedWire, m_snapEnabled ? dropPos : rawPos, junction, this);
                tempWire->confirmConnection(junction->term);
                QVector<QPointF> smartPath = AutoRouter::findPath(this, startTerminal->sceneBoundingRect().center(), exactPos, startTerminal, junction->term, tempWire);
                tempWire->setFullRoute(smartPath);
                isWiring = false; tempWire = nullptr; startTerminal = nullptr;
                setWiringMode(false);
                FileManager::recordState(this);
                return;
            } else {
                JunctionNode *junction = new JunctionNode();
                junction->setPos(dropPos);
                addItem(junction);
                tempWire->confirmConnection(junction->term);
                QVector<QPointF> smartPath = AutoRouter::findPath(this, startTerminal->sceneBoundingRect().center(), dropPos, startTerminal, junction->term, tempWire);
                tempWire->setFullRoute(smartPath);
                isWiring = false; tempWire = nullptr; startTerminal = nullptr;
                setWiringMode(false);
                FileManager::recordState(this);
                return;
            }
        }
    }
    QGraphicsScene::mousePressEvent(event);
}
void CircuitScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    // ثبت موقعیت موس برای خطوط متقاطع LTspice
    m_crosshairPos = event->scenePos();

    if (m_wiringMode) {
        invalidate(sceneRect(), QGraphicsScene::ForegroundLayer); // آپدیت آنی لایه رویی
    }

    if (isWiring && tempWire) {
        tempWire->setEndPoint(event->scenePos());
    }
    QGraphicsScene::mouseMoveEvent(event);
}

void CircuitScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsScene::mouseReleaseEvent(event);
    if (!isWiring && selectedItems().count() > 0) {
        FileManager::recordState(this);
    }
}

// ==========================================
// رویداد فشردن کلیدهای کیبورد (میانبرها، سیم‌کشی و حذف)
// ==========================================
void CircuitScene::keyPressEvent(QKeyEvent *event) {
    // --- فعال کردن حالت سیم‌کشی (W) ---
    if (event->key() == Qt::Key_W) {
        m_wiringMode = true;
        for (auto view : views()) view->setCursor(Qt::CrossCursor); // تغییر شکل موس به حالت هدف‌گیری
        qDebug() << "Wiring Mode: ON";
        return;
    }

    // --- لغو حالت سیم‌کشی (Esc) ---
    if (event->key() == Qt::Key_Escape) {
        if (isWiring) {
            if (tempWire) { removeItem(tempWire); delete tempWire; }
            isWiring = false; tempWire = nullptr; startTerminal = nullptr;
        }
        m_wiringMode = false;
        for (auto view : views()) view->setCursor(Qt::ArrowCursor); // بازگشت موس به حالت عادی
        qDebug() << "Wiring Mode: OFF";
        return;
    }


    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        bool somethingDeleted = false;

        // حذف مستقیم و بدون ریسک هر آیتمی که کاربر انتخاب کرده است
        for (QGraphicsItem *item : selectedItems()) {
            if (item && item->scene()) {
                removeItem(item);
                delete item;
                somethingDeleted = true;
            }
        }

        if (somethingDeleted) {
            FileManager::recordState(this);
            qDebug() << "[CircuitScene] Selected items deleted safely.";
        }
        return;
    }

    // --- میانبرهای ترکیبی با Control ---
    // --- میانبرهای ترکیبی با Control ---
    if (event->modifiers() & Qt::ControlModifier) {
// 🛠️ سناریو جدید: کپی کردن قطعات انتخاب شده (Ctrl + C)
        // میانبر کپی کامپوننت‌ها
        if (event->key() == Qt::Key_C) {
            copySelectedComponents(); // 🛠️ فراخوانی مستقیم تابع عمومی
            return;
        }

        // میانبر پیست کامپوننت‌ها
        if (event->key() == Qt::Key_V) {
            QGraphicsView* activeView = views().isEmpty() ? nullptr : views().first();
            QPointF targetOrigin = activeView ? activeView->mapToScene(activeView->mapFromGlobal(QCursor::pos())) : QPointF(0,0);
            pasteCopiedComponents(targetOrigin); // 🛠️ فراخوانی مستقیم تابع عمومی
            return;
        }
        // 🛠️ سناریو جدید: چرخش ۹۰ درجه قطعات انتخاب شده با Ctrl + R
        // 🛠️ چرخش ۹۰ درجه قطعات انتخاب شده با Ctrl + R
        if (event->key() == Qt::Key_R) {
            bool rotatedAny = false;

            for (QGraphicsItem *item : selectedItems()) {
                Element *element = dynamic_cast<Element*>(item);

                if (element && element->getComponentName() != "Junction Node") {
                    // تنظیم نقطه دوران روی مرکز قطعه
                    element->setTransformOriginPoint(element->boundingRect().center());

                    // اعمال چرخش ۹۰ درجه چرخه‌ای
                    qreal newRotation = element->rotation() + 90.0;
                    if (newRotation >= 360.0) newRotation -= 360.0;
                    element->setRotation(newRotation);

                    // به‌روزرسانی مکان گرافیکی سیم‌های متصل
                    for (QGraphicsItem *child : element->childItems()) {
                        Terminal *term = dynamic_cast<Terminal*>(child);
                        if (term) {
                            for (Wire *wire : term->getConnectedWires()) {
                                QVector<QPointF> wirePoints = wire->getPoints();

                                if (!wirePoints.isEmpty()) {
                                    QPointF newTerminalPos = term->sceneBoundingRect().center();

                                    if (wire->getStartTerminal() == term) {
                                        wirePoints[0] = newTerminalPos;
                                    }
                                    if (wire->getEndTerminal() == term) {
                                        wirePoints.last() = newTerminalPos;
                                    }

                                    wire->setFullRoute(wirePoints);
                                }
                            }
                        }
                    }
                    rotatedAny = true;
                }
            }

            if (rotatedAny) {
                FileManager::recordState(this);
                update();
                qDebug() << "[Rotation] Component(s) rotated successfully.";
            }
            return;
        }
        // 🛠️ سناریو جدید: قرینه‌سازی افقی قطعات انتخاب شده با Ctrl + M
        // 🛠️ نسخه اصلاح‌شده: قرینه‌سازی افقی قطعات به همراه خوانا نگه‌داشتن متن‌ها
        // 🛠️ نسخه اصلاح‌شده: قرینه‌سازی افقی قطعات به همراه خوانا نگه‌داشتن متن‌ها
        if (event->key() == Qt::Key_M) {
            bool mirroredAny = false;

            for (QGraphicsItem *item : selectedItems()) {
                Element *element = dynamic_cast<Element*>(item);

                if (element && element->getComponentName() != "Junction Node") {
                    element->setTransformOriginPoint(element->boundingRect().center());

                    // ۱. اعمال قرینه‌سازی روی خود قطعه
                    QTransform transform = element->transform();
                    transform.scale(-1, 1);
                    element->setTransform(transform);

                    // ۲. فیکس کردن متن‌ها: خنثی کردن اثر میرور روی نوشته‌های فرزند
                    for (QGraphicsItem *child : element->childItems()) {
                        // بررسی انواع آیتم‌های متنی متداول در Qt
                        bool isText = dynamic_cast<QGraphicsTextItem*>(child) != nullptr ||
                                      dynamic_cast<QGraphicsSimpleTextItem*>(child) != nullptr;

                        if (isText) {
                            child->setTransformOriginPoint(child->boundingRect().center());
                            QTransform textTransform = child->transform();
                            // یک بار دیگر روی محور X میرور می‌کنیم تا متن به حالت عادی و خوانا برگردد
                            textTransform.scale(-1, 1);
                            child->setTransform(textTransform);
                        }
                    }

                    // ۳. به‌روزرسانی آنی مسیر سیم‌های متصل به پایه‌ها
                    for (QGraphicsItem *child : element->childItems()) {
                        Terminal *term = dynamic_cast<Terminal*>(child);
                        if (term) {
                            for (Wire *wire : term->getConnectedWires()) {
                                QVector<QPointF> wirePoints = wire->getPoints();

                                if (!wirePoints.isEmpty()) {
                                    QPointF newTerminalPos = term->sceneBoundingRect().center();

                                    if (wire->getStartTerminal() == term) {
                                        wirePoints[0] = newTerminalPos;
                                    }
                                    if (wire->getEndTerminal() == term) {
                                        wirePoints.last() = newTerminalPos;
                                    }

                                    wire->setFullRoute(wirePoints);
                                }
                            }
                        }
                    }
                    mirroredAny = true;
                }
            }

            if (mirroredAny) {
                FileManager::recordState(this);
                update();
                qDebug() << "[Mirroring] Component mirrored and text readability preserved.";
            }
            return;
        }

        // کدهای قبلی شما (Ctrl+S, Ctrl+O, Ctrl+Z و غیره) در ادامه اینجا قرار دارند:
        if (event->key() == Qt::Key_S) { FileManager::saveCircuit("my_circuit_test.json", this); return; }
        if (event->key() == Qt::Key_O) { FileManager::loadCircuit("my_circuit_test.json", this); return; }
        if (event->key() == Qt::Key_Z) {
            if (event->modifiers() & Qt::ShiftModifier) FileManager::redo(this);
            else FileManager::undo(this);
            return;
        }
        else if (event->key() == Qt::Key_Y) { FileManager::redo(this); return; }
    }

    QGraphicsScene::keyPressEvent(event);
}

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
    else if (componentType == "OSCILLOSCOPE") item = new Oscilloscope();
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
        addComponent(event->mimeData()->text(), event->scenePos());
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}
// ==========================================
// رسم لایه رویی (خطوط متقاطع سیم‌کشی مثل LTspice)
// ==========================================
void CircuitScene::drawForeground(QPainter *painter, const QRectF &rect) {
    if (m_wiringMode) {
        QPen pen(QColor(100, 100, 100, 180)); // رنگ خاکستری نیمه‌شفاف
        pen.setStyle(Qt::DashLine);           // حالت نقطه‌چین
        pen.setWidth(1);
        pen.setCosmetic(true); // جادوی Qt: ضخامت خط همیشه 1 پیکسل می‌ماند حتی اگر زوم کنید!
        painter->setPen(pen);

        // رسم خط عمودی و افقی که کل صفحه را می‌پوشانند
        painter->drawLine(QPointF(m_crosshairPos.x(), rect.top()), QPointF(m_crosshairPos.x(), rect.bottom()));
        painter->drawLine(QPointF(rect.left(), m_crosshairPos.y()), QPointF(rect.right(), m_crosshairPos.y()));
    }
    QGraphicsScene::drawForeground(painter, rect);
}
// 🛠️ تابع اختصاصی کپی قطعات و سیم‌ها
void CircuitScene::copySelectedComponents() {
    m_clipboardComponents.clear();
    m_clipboardWires.clear();

    QList<QGraphicsItem*> selected = selectedItems();
    QList<Element*> selectedElements;
    QPointF referencePos;
    bool hasReference = false;

    for (QGraphicsItem *item : selected) {
        Element *el = dynamic_cast<Element*>(item);
        if (el && el->getComponentName() != "Junction Node") {
            if (!hasReference) {
                referencePos = el->scenePos();
                hasReference = true;
            }
            selectedElements.append(el);

            CopiedComponent comp;
            comp.type = el->getComponentName();

            // نگاشت نام‌های کارخانه
            if (comp.type == "Microcontroller (MCU)") comp.type = "MCU";
            else if (comp.type == "Ground (GND)") comp.type = "GROUND";
            else if (comp.type == "DC Voltage Source") comp.type = "DC_SOURCE";
            else if (comp.type == "Pulse Generator") comp.type = "PULSE_GENERATOR";
            else if (comp.type == "7-Segment Display") comp.type = "SEVEN_SEGMENT";
            else if (comp.type == "LCD 16x2 Display") comp.type = "LCD";
            else if (comp.type == "Matrix Keypad 4x4") comp.type = "KEYPAD";
            else if (comp.type == "Analog to Digital Converter (ADC)") comp.type = "ADC";
            else if (comp.type == "Digital to Analog Converter (DAC)") comp.type = "DAC";
            else comp.type = comp.type.toUpper().replace(" GATE", "_GATE").replace("-TYPE ", "_").replace(" ", "_");

            comp.properties = el->getProperties();
            comp.relativePos = el->scenePos() - referencePos;
            comp.originalAddress = el;
            m_clipboardComponents.append(comp);
        }
    }

    for (QGraphicsItem *item : selected) {
        if (Wire *wire = dynamic_cast<Wire*>(item)) {
            Terminal *startTerm = wire->getStartTerminal();
            Terminal *endTerm = wire->getEndTerminal();
            if (!startTerm || !endTerm) continue;

            Element *startEl = dynamic_cast<Element*>(startTerm->parentItem());
            Element *endEl = dynamic_cast<Element*>(endTerm->parentItem());

            int startIdx = selectedElements.indexOf(startEl);
            int endIdx = selectedElements.indexOf(endEl);

            if (startIdx != -1 && endIdx != -1) {
                CopiedWire cw;
                cw.startComponentIndex = startIdx;
                cw.endComponentIndex = endIdx;

                int tIdx = 0;
                for (QGraphicsItem* child : startEl->childItems()) {
                    if (Terminal* t = dynamic_cast<Terminal*>(child)) {
                        if (t == startTerm) { cw.startTerminalIndex = tIdx; break; }
                        tIdx++;
                    }
                }

                tIdx = 0;
                for (QGraphicsItem* child : endEl->childItems()) {
                    if (Terminal* t = dynamic_cast<Terminal*>(child)) {
                        if (t == endTerm) { cw.endTerminalIndex = tIdx; break; }
                        tIdx++;
                    }
                }
                m_clipboardWires.append(cw);
            }
        }
    }
}

// 🛠️ تابع اختصاصی پیست قطعات بر اساس مختصات ورودی هدف
void CircuitScene::pasteCopiedComponents(const QPointF &targetScenePos) {
    if (m_clipboardComponents.isEmpty()) return;

    clearSelection();
    QVector<Element*> pastedElements;

    QPointF targetOrigin = targetScenePos;
    if (m_snapEnabled) {
        targetOrigin.setX(qRound(targetOrigin.x() / m_gridSize) * m_gridSize);
        targetOrigin.setY(qRound(targetOrigin.y() / m_gridSize) * m_gridSize);
    }

    for (const CopiedComponent &comp : m_clipboardComponents) {
        QPointF finalPos = targetOrigin + comp.relativePos;
        QGraphicsItem *newItem = addComponent(comp.type, finalPos);

        if (newItem) {
            Element *newEl = dynamic_cast<Element*>(newItem);
            if (newEl) {
                newEl->setProperties(comp.properties);
                newEl->setSelected(true);
                pastedElements.append(newEl);
            } else { pastedElements.append(nullptr); }
        } else { pastedElements.append(nullptr); }
    }

    for (const CopiedWire &cw : m_clipboardWires) {
        Element *startEl = pastedElements.value(cw.startComponentIndex, nullptr);
        Element *endEl = pastedElements.value(cw.endComponentIndex, nullptr);

        if (startEl && endEl) {
            Terminal *startTerm = nullptr;
            Terminal *endTerm = nullptr;

            int tIdx = 0;
            for (QGraphicsItem* child : startEl->childItems()) {
                if (Terminal* t = dynamic_cast<Terminal*>(child)) {
                    if (tIdx == cw.startTerminalIndex) { startTerm = t; break; }
                    tIdx++;
                }
            }

            tIdx = 0;
            for (QGraphicsItem* child : endEl->childItems()) {
                if (Terminal* t = dynamic_cast<Terminal*>(child)) {
                    if (tIdx == cw.endTerminalIndex) { endTerm = t; break; }
                    tIdx++;
                }
            }

            if (startTerm && endTerm) {
                Wire *newWire = new Wire(startTerm, startTerm->sceneBoundingRect().center());
                newWire->confirmConnection(endTerm);
                addItem(newWire);
                newWire->updateRoute();
                newWire->setSelected(true);
            }
        }
    }

    FileManager::recordState(this);
    update();
}
// 🛠️ متد کنترل حالت سیم‌کشی از طریق UI
void CircuitScene::setWiringMode(bool mode) {
    m_wiringMode = mode;
    if (!m_wiringMode && isWiring) {
        if (tempWire) { removeItem(tempWire); delete tempWire; tempWire = nullptr; }
        isWiring = false; startTerminal = nullptr;
    }
    for (auto view : views()) {
        view->setCursor(mode ? Qt::CrossCursor : Qt::ArrowCursor);
    }
}