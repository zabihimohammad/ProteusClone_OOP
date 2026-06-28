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
#include <QPainter> // این کتابخانه را به بالای فایل اضافه کنید

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

        // ۱. قرار دادن نام قطعه در پاکت نامه (برای بوم)
        mimeData->setText(this->text());
        drag->setMimeData(mimeData);

        // ==========================================
        // بخش جدید: ساخت شبح گرافیکی برای زیر موس
        // ==========================================

        // الف) یک بوم نقاشی کوچک (Pixmap) موقت می‌سازیم
        QPixmap pixmap(80, 40);
        pixmap.fill(Qt::transparent); // پس‌زمینه را شفاف می‌کنیم

        // ب) نقاشی کردن شکل قطعه روی این تصویر
        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing); // نرم کردن لبه‌ها

        // کشیدن یک مستطیل نیمه‌شفاف (شبح قطعه)
        painter.setBrush(QColor(0, 51, 102, 180)); // رنگ آبی با شفافیت
        painter.setPen(QPen(Qt::white, 2));
        painter.drawRoundedRect(2, 2, 76, 36, 5, 5); // مستطیل گوشه‌گرد

        // نوشتن نام قطعه وسط آن
        painter.drawText(pixmap.rect(), Qt::AlignCenter, this->text());
        painter.end();

        // ج) چسباندن این نقاشی به نشانگر موس
        drag->setPixmap(pixmap);

        // د) تنظیم نقطه ثقل (HotSpot): می‌خواهیم موس دقیقاً وسط قطعه باشد
        drag->setHotSpot(QPoint(pixmap.width() / 2, pixmap.height() / 2));

        // ==========================================

        // پرتاب کردن پاکت به سمت بوم!
        drag->exec(Qt::CopyAction);
    }
};
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    // ۱. تنظیم ویژگی‌های ظاهری پنجره اصلی نرم‌افزار
    setWindowTitle("Proteus Clone - OOP Project");
    resize(1024, 768);

    // ۲. ساخت بوم طراحی و متصل کردن آن به دوربین نمایش‌دهنده
    scene = new CircuitScene(this);
    view = new CircuitView(this);
    view->setScene(scene);
    setCentralWidget(view);
    //موقت برای تست
    TestDragButton *testBtn1 = new TestDragButton("MCU", this);
    testBtn1->setGeometry(10, 10, 100, 40); // قرار دادن دکمه در بالا-سمت چپ

    TestDragButton *testBtn2 = new TestDragButton("RESISTOR", this);
    testBtn2->setGeometry(10, 60, 100, 40);
    // =========================================================
    // بخش جدید: چیدن ویترین قطعات روی صفحه شطرنجی
    // =========================================================

    // ۱. مقاومت (مرکز صفحه)
    Resistor *r1 = new Resistor();
    r1->setPos(0, 0);
    scene->addItem(r1);

    // ۲. خازن (سمت راست مقاومت)
    Capacitor *c1 = new Capacitor();
    c1->setPos(100, 0);
    scene->addItem(c1);

    // ۳. منبع تغذیه یا باتری (سمت چپ مقاومت)
    DCVoltageSource *v1 = new DCVoltageSource();
    v1->setPos(-100, 0);
    scene->addItem(v1);

    // ۴. زمین یا GND (دقیقاً زیر منبع تغذیه)
    Ground *gnd1 = new Ground();
    gnd1->setPos(-100, 80);
    scene->addItem(gnd1);

    // ۵. سلف (دقیقاً زیر مقاومت)
    Inductor *l1 = new Inductor();
    l1->setPos(0, 80);
    scene->addItem(l1);

    // ۶. گیت منطقی AND (سمت راست خازن)
    AndGate *and1 = new AndGate();
    and1->setPos(220, 0);
    scene->addItem(and1);

    // ۷. تراشه مرکزی پردازنده (در بخش بالایی مدار)
    MCUChip *mcu1 = new MCUChip();
    mcu1->setPos(0, -150);
    scene->addItem(mcu1);
    // --- ردیف قطعات تعاملی ---
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

    // --- ردیف گیت‌های منطقی پیشرفته ---
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
    // --- قطعات جانبی پیشرفته ---
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
}

MainWindow::~MainWindow() {
    // به دلیل استفاده از this، مدیریت حافظه خودکار توسط Qt انجام می‌شود
}