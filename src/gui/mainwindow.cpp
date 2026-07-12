#include "mainwindow.h"

// هدرهای مربوط به بخش بوم گرافیکی و دوربین
#include "../canvas/circuit_scene.h"
#include "../canvas/circuit_view.h"

// هدرهای مربوط به کتابخانه تمام قطعاتی که تا الان ساخته‌ایم
#include "../components/basic_components.h"
#include "../components/logic_gates.h"
#include "../components/mcu.h"
#include "../components/peripherals.h"
#include <QPushButton>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>
#include <QPainter>
#include "../core/simulation_engine.h"
#include <QTimer>
#include <QCursor>
#include "../core/wire.h"
#include "../core/terminal.h"
#include "../core/probe_item.h"
// ====================================================
// کلاس موقت تست با قابلیت نمایش تصویر گرافیکی حین Drag
// ====================================================
class TestDragButton : public QPushButton {
public:
    TestDragButton(QString text, QWidget *parent = nullptr) : QPushButton(text, parent) {}

protected:
    void mouseMoveEvent(QMouseEvent *event) override {
        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;

        mimeData->setText(this->text());
        drag->setMimeData(mimeData);

        QPixmap pixmap(80, 40);
        pixmap.fill(Qt::transparent);

        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing);

        painter.setBrush(QColor(0, 51, 102, 180));
        painter.setPen(QPen(Qt::white, 2));
        painter.drawRoundedRect(2, 2, 76, 36, 5, 5);

        painter.drawText(pixmap.rect(), Qt::AlignCenter, this->text());
        painter.end();

        drag->setPixmap(pixmap);
        drag->setHotSpot(QPoint(pixmap.width() / 2, pixmap.height() / 2));
        drag->exec(Qt::CopyAction);
    }
};

// ====================================================
// تابع سازنده (هنگام باز شدن برنامه اجرا می‌شود)
// ====================================================
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    // ۱. تنظیم ویژگی‌های ظاهری پنجره
    setWindowTitle("Proteus Clone - OOP Project");
    resize(1024, 768);

    // ۲. ساخت بوم طراحی و دوربین
    scene = new CircuitScene(this);
    view = new CircuitView(this);
    view->setScene(scene);
    setCentralWidget(view);

    // ۳. دکمه‌های موقت تست
    TestDragButton *testBtn1 = new TestDragButton("MCU", this);
    testBtn1->setGeometry(10, 10, 100, 40);

    TestDragButton *testBtn2 = new TestDragButton("RESISTOR", this);
    testBtn2->setGeometry(10, 60, 100, 40);

    // =========================================================
    // 🛠️ دکمه فعال/غیرفعال کردن پروب ولتاژ
    // =========================================================
    QPushButton *probeButton = new QPushButton("🔍 Probe: OFF", this);
    probeButton->setCheckable(true);
    probeButton->setGeometry(10, 110, 120, 30); // پایین‌تر از دکمه‌های تست قرار گرفت

    connect(probeButton, &QPushButton::toggled, this, [=](bool checked) {
        scene->isProbeEnabled = checked; // نام متغیر به scene تغییر کرد

        if (checked) {
            probeButton->setText("🔍 Probe: ON");
            probeButton->setStyleSheet("background-color: #a8f0c6; font-weight: bold; border-radius: 5px;");
        } else {
            probeButton->setText("🔍 Probe: OFF");
            probeButton->setStyleSheet("");

            if (scene->voltageProbe) {
                scene->voltageProbe->hide();
            }
        }
    });

    // =========================================================
    // چیدن قطعات روی صفحه شطرنجی
    // =========================================================
    Resistor *r1 = new Resistor();
    r1->setPos(0, 0);
    scene->addItem(r1);

    Capacitor *c1 = new Capacitor();
    c1->setPos(100, 0);
    scene->addItem(c1);

    DCVoltageSource *v1 = new DCVoltageSource();
    v1->setPos(-100, 0);
    scene->addItem(v1);

    Ground *gnd1 = new Ground();
    gnd1->setPos(-100, 80);
    scene->addItem(gnd1);

    Inductor *l1 = new Inductor();
    l1->setPos(0, 80);
    scene->addItem(l1);

    AndGate *and1 = new AndGate();
    and1->setPos(220, 0);
    scene->addItem(and1);

    MCUChip *mcu1 = new MCUChip();
    mcu1->setPos(0, -150);
    scene->addItem(mcu1);

    PulseGenerator *pulse = new PulseGenerator();
    pulse->setPos(-200, -100);
    scene->addItem(pulse);

    Switch *sw1 = new Switch();
    sw1->setPos(-100, -100);
    scene->addItem(sw1);

    PushButton *pb1 = new PushButton();
    pb1->setPos(0, -100);
    scene->addItem(pb1);

    LED *led1 = new LED();
    led1->setPos(100, -100);
    scene->addItem(led1);

    SevenSegment *seg1 = new SevenSegment();
    seg1->setPos(200, -100);
    scene->addItem(seg1);

    OrGate *or1 = new OrGate();
    or1->setPos(-200, 150);
    scene->addItem(or1);

    NotGate *not1 = new NotGate();
    not1->setPos(-100, 150);
    scene->addItem(not1);

    XorGate *xor1 = new XorGate();
    xor1->setPos(0, 150);
    scene->addItem(xor1);

    NandGate *nand1 = new NandGate();
    nand1->setPos(100, 150);
    scene->addItem(nand1);

    DFlipFlop *dff1 = new DFlipFlop();
    dff1->setPos(250, 150);
    scene->addItem(dff1);

    MemoryChip *ram = new MemoryChip();
    ram->setPos(-350, 0);
    scene->addItem(ram);

    LCD16x2 *lcd = new LCD16x2();
    lcd->setPos(-350, -150);
    scene->addItem(lcd);

    Keypad *kp = new Keypad();
    kp->setPos(-350, 150);
    scene->addItem(kp);

    ADC_Chip *adc = new ADC_Chip();
    adc->setPos(350, -100);
    scene->addItem(adc);

    DAC_Chip *dac = new DAC_Chip();
    dac->setPos(350, 100);
    scene->addItem(dac);
    // =========================================================
    // ⚙️ راه‌اندازی قلب تپنده شبیه‌ساز (Simulation Engine)
    // =========================================================

    SimulationEngine *engine = new SimulationEngine(scene, this);

    QTimer *simTimer = new QTimer(this);
    connect(simTimer, &QTimer::timeout, this, [=]() {

        // ۱. اجرای محاسبات ریاضی مدار
        engine->stepSimulation();

        // ۲. سیستم رهگیری ایمن (Safe Tracking)
        // شرط جدید: فقط اگر پنجره برنامه فعال است و موس داخل بوم است، پروب را آپدیت کن
        if (isActiveWindow() && view->underMouse() && scene->isProbeEnabled && scene->voltageProbe) {

            QPoint globalPos = QCursor::pos();
            QPoint viewPos = view->mapFromGlobal(globalPos);
            QPointF scenePos = view->mapToScene(viewPos);

            QList<QGraphicsItem*> items = scene->items(scenePos);
            bool found = false;

            for (QGraphicsItem* item : items) {
                if (Wire* wire = dynamic_cast<Wire*>(item)) {
                    scene->voltageProbe->updateProbe(wire->voltageLevel, scenePos);
                    found = true;
                    break;
                }
                else if (Terminal* term = dynamic_cast<Terminal*>(item)) {
                    scene->voltageProbe->updateProbe(term->voltageLevel, scenePos);
                    found = true;
                    break;
                }
            }

            if (!found) scene->voltageProbe->hide();

        } else if (scene->voltageProbe) {
            // اگر موس از برنامه خارج شد، فوراً پروب را مخفی کن تا کرش نکند
            scene->voltageProbe->hide();
        }
    });

    simTimer->start(100);
}

// ====================================================
// تابع مخرب (هنگام بسته شدن برنامه اجرا می‌شود)
// ====================================================
MainWindow::~MainWindow() {
    // کاملاً خالی می‌ماند (Qt خودش حافظه‌ها را پاک می‌کند)
}