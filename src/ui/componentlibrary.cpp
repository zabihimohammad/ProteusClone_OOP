#include "componentlibrary.h"

#include <QLabel>
#include <QHash>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>
#include <QApplication>
#include <QPixmap>
#include <QPainter>
#include <QGraphicsScene>
#include "../components/basic_components.h"
#include "../components/logic_gates.h"
#include "../components/mcu.h"
#include "../components/peripherals.h"
ComponentLibrary::ComponentLibrary(QWidget *parent) : QFrame(parent)
{
    setObjectName("libraryPanel");
    setMinimumWidth(270);
    setMaximumWidth(340);

    m_components = {
            {"BATTERY", "Battery", "Sources", "Real DC Battery with internal resistance", "─| |ı─"},
            {"VOLTMETER", "Voltmeter", "Measurement", "Digital DC Voltmeter", "( V )"},
            {"AMMETER", "Ammeter", "Measurement", "Digital DC Ammeter", "( A )"},
        {"RESISTOR", "Resistor", "Analog", "Limits current flow", R"(─/\/\/─)"},
        {"CAPACITOR", "Capacitor", "Analog", "Stores electrical charge", "─| |─"},
        {"INDUCTOR", "Inductor", "Analog", "Stores energy in a magnetic field", "─((((─"},
        {"DC_SOURCE", "DC Source", "Sources", "Ideal direct-voltage source", "─( + − )─"},
        {"GROUND", "Ground", "Sources", "Circuit reference node", "  ⏚"},
        {"AND_GATE", "AND Gate", "Digital", "Two-input logical AND", "─|AND)─"},
        {"OR_GATE", "OR Gate", "Digital", "Two-input logical OR", "─)OR )─"},
        {"NOT_GATE", "NOT Gate", "Digital", "Logical inverter", "─▷o─"},
        {"XOR_GATE", "XOR Gate", "Digital", "Exclusive OR gate", "─)XOR)─"},
        {"NAND_GATE", "NAND Gate", "Digital", "Inverted AND gate", "─|AND)o─"},
        {"D_FLIP_FLOP", "D Flip-Flop", "Digital", "Sequential storage element", "─| D Q|─"},
        {"LED", "LED", "Interactive", "Light-emitting diode", "─▷|─  ↗"},
        {"SWITCH", "Switch", "Interactive", "User-controlled switch", "─o/ o─"},
        {"PUSH_BUTTON", "Push Button", "Interactive", "Momentary user input", "─o  o─"},
        {"SEVEN_SEGMENT", "Seven Segment", "Outputs", "Numeric display", "[ 8 ]"},
        {"LCD", "LCD 16×2", "Outputs", "Character display", "[ LCD ]"},
        {"PULSE_GENERATOR", "Pulse Generator", "Sources", "Digital clock source", "─▱▔▱─"},
        {"MCU", "MCU", "Advanced", "Microcontroller component", "[ MCU ]"},
        {"MEMORY", "Memory", "Advanced", "External memory chip", "[ RAM ]"},
        {"KEYPAD", "Keypad", "Advanced", "Matrix keypad", "[ # ]"},
        {"ADC", "ADC", "Converters", "Analog-to-digital converter", "[ A→D ]"},
        {"DAC", "DAC", "Converters", "Digital-to-analog converter", "[ D→A ]"},
        {"OSCILLOSCOPE", "Oscilloscope", "Outputs", "Real-time voltage waveform scope", "[ ∿ SCOPE ]"}

    };

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(16, 18, 16, 16);
    layout->setSpacing(12);

    auto *title = new QLabel(tr("Components"));
    title->setObjectName("panelTitle");
    m_search = new QLineEdit;
    m_search->setPlaceholderText(tr("Search name or category…"));
    m_search->setClearButtonEnabled(true);

    m_tree = new QTreeWidget;
    m_tree->setHeaderHidden(true);
    m_tree->setIndentation(16);
    m_tree->setRootIsDecorated(true);
    m_tree->viewport()->installEventFilter(this);
    auto *preview = new QFrame;
    preview->setObjectName("previewCard");
    auto *previewLayout = new QVBoxLayout(preview);
    m_symbol = new QLabel(R"(─/\/\/─)");
    m_symbol->setObjectName("symbolPreview");
    m_symbol->setAlignment(Qt::AlignCenter);
    m_name = new QLabel(tr("Select a component"));
    m_name->setObjectName("previewTitle");
    m_description = new QLabel(tr("Its schematic preview appears here."));
    m_description->setWordWrap(true);
    m_description->setAlignment(Qt::AlignCenter);
    previewLayout->addWidget(m_symbol);
    previewLayout->addWidget(m_name, 0, Qt::AlignCenter);
    previewLayout->addWidget(m_description);

    m_empty = new QLabel(tr("No matching component"));
    m_empty->setAlignment(Qt::AlignCenter);
    m_empty->setObjectName("emptyState");
    m_empty->hide();

    auto *activeHeader = new QWidget;
    auto *activeHeaderLayout = new QHBoxLayout(activeHeader);
    activeHeaderLayout->setContentsMargins(0, 2, 0, 0);
    auto *activeTitle = new QLabel(tr("Active components"));
    activeTitle->setObjectName("sectionTitle");
    activeHeaderLayout->addWidget(activeTitle);
    activeHeaderLayout->addStretch();
    m_active = new QListWidget;
    m_active->setMaximumHeight(82);
    m_active->setToolTip(tr("Components currently enabled for this project"));

    layout->addWidget(title);
    layout->addWidget(m_search);
    layout->addWidget(m_tree, 1);
    layout->addWidget(m_empty);
    layout->addWidget(activeHeader);
    layout->addWidget(m_active);
    layout->addWidget(preview);

    connect(m_search, &QLineEdit::textChanged, this, &ComponentLibrary::rebuildTree);
    connect(m_tree, &QTreeWidget::currentItemChanged, this, &ComponentLibrary::updatePreview);
    connect(m_tree, &QTreeWidget::itemDoubleClicked, this, [this](QTreeWidgetItem *item) {
        if (item && item->data(0, Qt::UserRole).isValid()) {
            if (m_active->findItems(item->text(0), Qt::MatchExactly).isEmpty()) {
                auto *activeItem = new QListWidgetItem(item->text(0), m_active);
                activeItem->setSizeHint(QSize(0, 34));
                auto *row = new QWidget;
                row->setObjectName("activeRow");
                auto *rowLayout = new QHBoxLayout(row);
                rowLayout->setContentsMargins(9, 2, 5, 2);
                auto *label = new QLabel(item->text(0));
                auto *remove = new QPushButton(QString::fromUtf8("−"));
                remove->setObjectName("removeComponent");
                remove->setToolTip(tr("Remove %1 from active components").arg(item->text(0)));
                remove->setFixedSize(26, 26);
                rowLayout->addWidget(label);
                rowLayout->addStretch();
                rowLayout->addWidget(remove);
                m_active->setItemWidget(activeItem, row);
                connect(remove, &QPushButton::clicked, this, [this, activeItem] {
                    delete m_active->takeItem(m_active->row(activeItem));
                });
            }
            const int componentIndex = item->data(0, Qt::UserRole).toInt();
            emit componentActivated(m_components.at(componentIndex).id);
        }
    });
    rebuildTree();
}

void ComponentLibrary::rebuildTree(const QString &query)
{
    m_tree->clear();
    const QString needle = query.trimmed();
    int matches = 0;
    QHash<QString, QTreeWidgetItem *> categories;
    for (int i = 0; i < m_components.size(); ++i) {
        const auto &component = m_components.at(i);
        if (!needle.isEmpty() && !component.name.contains(needle, Qt::CaseInsensitive) &&
            !component.category.contains(needle, Qt::CaseInsensitive))
            continue;

        auto *category = categories.value(component.category, nullptr);
        if (!category) {
            category = new QTreeWidgetItem(m_tree, {component.category});
            category->setFlags(category->flags() & ~Qt::ItemIsSelectable);
            categories.insert(component.category, category);
        }
        auto *item = new QTreeWidgetItem(category, {component.name});
        item->setData(0, Qt::UserRole, i);
        ++matches;
    }
    m_tree->expandAll();
    m_empty->setVisible(matches == 0);
    m_tree->setVisible(matches != 0);
}

void ComponentLibrary::updatePreview(QTreeWidgetItem *current, QTreeWidgetItem *)
{
    if (!current || !current->data(0, Qt::UserRole).isValid())
        return;
    const auto &component = m_components.at(current->data(0, Qt::UserRole).toInt());
    m_symbol->setText(component.symbol);
    m_name->setText(component.name);
    m_description->setText(component.description + "  •  " + component.category);
}
// کشیدن قطعه از کتابخانه
bool ComponentLibrary::eventFilter(QObject *obj, QEvent *event) {
    if (obj == m_tree->viewport()) {

        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                m_dragStartPos = mouseEvent->pos();
            }
        }
        else if (event->type() == QEvent::MouseMove) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

            if ((mouseEvent->buttons() & Qt::LeftButton) &&
                (mouseEvent->pos() - m_dragStartPos).manhattanLength() > QApplication::startDragDistance()) {

                QTreeWidgetItem *item = m_tree->itemAt(m_dragStartPos);

                if (item && item->data(0, Qt::UserRole).isValid()) {
                    int index = item->data(0, Qt::UserRole).toInt();
                    QString componentId = m_components.at(index).id;

                    QDrag *drag = new QDrag(this);
                    QMimeData *mimeData = new QMimeData;
                    mimeData->setText(componentId);
                    drag->setMimeData(mimeData);

                    // قطعه موقت برای تصویر کشیدن
                    QGraphicsItem *tempItem = nullptr;
                    if (componentId == "MCU") tempItem = new MCUChip();
                    else if (componentId == "RESISTOR") tempItem = new Resistor();
                    else if (componentId == "CAPACITOR") tempItem = new Capacitor();
                    else if (componentId == "INDUCTOR") tempItem = new Inductor();
                    else if (componentId == "DC_SOURCE") tempItem = new DCVoltageSource();
                    else if (componentId == "GROUND") tempItem = new Ground();
                    else if (componentId == "AND_GATE") tempItem = new AndGate();
                    else if (componentId == "OR_GATE") tempItem = new OrGate();
                    else if (componentId == "NOT_GATE") tempItem = new NotGate();
                    else if (componentId == "XOR_GATE") tempItem = new XorGate();
                    else if (componentId == "NAND_GATE") tempItem = new NandGate();
                    else if (componentId == "D_FLIP_FLOP") tempItem = new DFlipFlop();
                    else if (componentId == "LED") tempItem = new LED();
                    else if (componentId == "SWITCH") tempItem = new Switch();
                    else if (componentId == "PUSH_BUTTON") tempItem = new PushButton();
                    else if (componentId == "SEVEN_SEGMENT") tempItem = new SevenSegment();
                    else if (componentId == "PULSE_GENERATOR") tempItem = new PulseGenerator();
                    else if (componentId == "MEMORY") tempItem = new MemoryChip();
                    else if (componentId == "LCD") tempItem = new LCD16x2();
                    else if (componentId == "KEYPAD") tempItem = new Keypad();
                    else if (componentId == "ADC") tempItem = new ADC_Chip();
                    else if (componentId == "DAC") tempItem = new DAC_Chip();
                    else if (componentId == "OSCILLOSCOPE") tempItem = new Oscilloscope();
                    if (tempItem) {
                        // بوم موقت قطعه
                        QGraphicsScene tempScene;
                        tempScene.addItem(tempItem);

                        // ابعاد قطعه و پایه‌ها
                        QRectF rect = tempScene.itemsBoundingRect();
                        rect.adjust(-2, -2, 2, 2); // حاشیه تصویر

                        // تصویر شفاف
                        QPixmap pixmap(rect.size().toSize());
                        pixmap.fill(Qt::transparent);

                        QPainter painter(&pixmap);
                        painter.setRenderHint(QPainter::Antialiasing);

                        // قطعه را روی تصویر رسم کن.
                        tempScene.render(&painter, QRectF(), rect);
                        painter.end();

                        // تصویر نیمه‌شفاف هنگام کشیدن
                        QPixmap ghostPixmap(pixmap.size());
                        ghostPixmap.fill(Qt::transparent);
                        QPainter ghostPainter(&ghostPixmap);
                        ghostPainter.setOpacity(0.6);
                        ghostPainter.drawPixmap(0, 0, pixmap);
                        ghostPainter.end();

                        drag->setPixmap(ghostPixmap);

                        // نشانگر در مرکز قطعه
                        drag->setHotSpot(QPoint(-rect.left(), -rect.top()));

                        // بوم موقت، قطعه را آزاد می‌کند.
                    }

                    drag->exec(Qt::CopyAction);
                    return true;
                }
            }
        }
    }
    return QFrame::eventFilter(obj, event);
}
