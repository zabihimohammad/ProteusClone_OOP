#include "helpdialog.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QScrollArea>
#include <QSplitter>
#include <QStackedWidget>
#include <QVBoxLayout>

namespace {
// تصویر ساده هر راهنما
class GuidePicture final : public QWidget
{
public:
    explicit GuidePicture(int picture, QWidget *parent = nullptr)
        : QWidget(parent), m_picture(picture)
    {
        setMinimumHeight(210);
        setMaximumHeight(250);
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.fillRect(rect(), QColor("#F7FAFE"));

        QRectF area = rect().adjusted(22, 18, -22, -18);
        painter.setPen(QPen(QColor("#D9E2EC"), 1));
        painter.setBrush(Qt::white);
        painter.drawRoundedRect(area, 14, 14);

        switch (m_picture) {
        case 0: drawWorkspace(painter, area); break;
        case 1: drawProjects(painter, area); break;
        case 2: drawComponents(painter, area); break;
        case 3: drawWiring(painter, area); break;
        case 4: drawSimulation(painter, area); break;
        case 5: drawCanvas(painter, area); break;
        case 6: drawKeyboard(painter, area); break;
        default: drawTroubleshooting(painter, area); break;
        }
    }

private:
    static void text(QPainter &painter, const QRectF &area, const QString &value,
                     int size = 10, bool bold = false, QColor color = QColor("#344054"))
    {
        QFont font("Segoe UI", size);
        font.setBold(bold);
        painter.setFont(font);
        painter.setPen(color);
        painter.drawText(area, Qt::AlignCenter, value);
    }

    static void pill(QPainter &painter, QRectF area, const QString &value, bool active = false)
    {
        painter.setPen(Qt::NoPen);
        painter.setBrush(active ? QColor("#1473E6") : QColor("#EEF3F8"));
        painter.drawRoundedRect(area, 8, 8);
        text(painter, area, value, 9, true, active ? Qt::white : QColor("#344054"));
    }

    static void callout(QPainter &painter, QPointF point, int number)
    {
        painter.setPen(QPen(Qt::white, 2));
        painter.setBrush(QColor("#1473E6"));
        painter.drawEllipse(point, 13, 13);
        text(painter, QRectF(point.x() - 13, point.y() - 13, 26, 26), QString::number(number), 9, true, Qt::white);
    }

    static void drawWorkspace(QPainter &painter, const QRectF &area)
    {
        QRectF app = area.adjusted(18, 17, -18, -17);
        painter.setPen(QPen(QColor("#D7DFE8"), 1));
        painter.setBrush(QColor("#EEF2F6"));
        painter.drawRoundedRect(app, 8, 8);
        painter.setBrush(Qt::white);
        painter.drawRect(QRectF(app.left(), app.top(), app.width(), 31));
        text(painter, QRectF(app.left() + 12, app.top(), 105, 31), "File   Edit   View", 8);
        painter.setBrush(QColor("#F8FAFC"));
        painter.drawRect(QRectF(app.left() + 10, app.top() + 42, 145, app.height() - 52));
        text(painter, QRectF(app.left() + 20, app.top() + 49, 120, 25), "Components", 10, true);
        painter.setBrush(Qt::white);
        QRectF canvas(app.left() + 166, app.top() + 42, app.width() - 176, app.height() - 52);
        painter.drawRect(canvas);
        painter.setPen(QPen(QColor("#DCE8F5"), 1));
        for (qreal x = canvas.left() + 12; x < canvas.right(); x += 16)
            for (qreal y = canvas.top() + 12; y < canvas.bottom(); y += 16)
                painter.drawPoint(QPointF(x, y));
        callout(painter, QPointF(app.left() + 85, app.top() + 90), 1);
        callout(painter, canvas.center(), 2);
        callout(painter, QPointF(app.right() - 72, app.top() + 16), 3);
    }

    static void drawProjects(QPainter &painter, const QRectF &area)
    {
        const qreal y = area.center().y() - 48;
        QRectF welcome(area.left() + 35, y, 210, 96);
        painter.setPen(QPen(QColor("#D9E2EC"), 1));
        painter.setBrush(QColor("#F8FAFC"));
        painter.drawRoundedRect(welcome, 12, 12);
        text(painter, QRectF(welcome.left(), welcome.top() + 10, welcome.width(), 24), "Welcome", 12, true);
        pill(painter, QRectF(welcome.left() + 18, welcome.top() + 49, 79, 28), "Create", true);
        pill(painter, QRectF(welcome.left() + 108, welcome.top() + 49, 82, 28), "Open");
        painter.setPen(QPen(QColor("#98A7B8"), 2));
        painter.drawLine(QPointF(welcome.right() + 18, y + 48), QPointF(area.right() - 260, y + 48));
        QRectF editor(area.right() - 242, y, 205, 96);
        painter.setPen(QPen(QColor("#D9E2EC"), 1));
        painter.setBrush(Qt::white);
        painter.drawRoundedRect(editor, 12, 12);
        text(painter, QRectF(editor.left(), editor.top() + 10, editor.width(), 24), "Circuit Editor", 12, true);
        pill(painter, QRectF(editor.left() + 18, editor.top() + 49, 76, 28), "Save", true);
        pill(painter, QRectF(editor.left() + 105, editor.top() + 49, 82, 28), "Save As");
    }

    static void drawComponents(QPainter &painter, const QRectF &area)
    {
        QRectF panel(area.left() + 28, area.top() + 26, 180, area.height() - 52);
        painter.setPen(QPen(QColor("#D9E2EC"), 1));
        painter.setBrush(QColor("#F8FAFC"));
        painter.drawRoundedRect(panel, 10, 10);
        text(painter, QRectF(panel.left(), panel.top() + 8, panel.width(), 22), "Component Library", 10, true);
        const QStringList names{"Sources", "Analog", "Logic", "Measurement"};
        for (int i = 0; i < names.size(); ++i)
            pill(painter, QRectF(panel.left() + 14, panel.top() + 40 + i * 27, panel.width() - 28, 21), names.at(i), i == 1);

        painter.setPen(QPen(QColor("#1473E6"), 2));
        painter.drawLine(QPointF(panel.right() + 20, panel.center().y()), QPointF(area.center().x() + 30, area.center().y()));
        QRectF resistor(area.center().x() + 34, area.center().y() - 26, 125, 52);
        painter.setPen(QPen(QColor("#202A36"), 3));
        QPainterPath path;
        path.moveTo(resistor.left(), resistor.center().y());
        for (int i = 0; i < 6; ++i)
            path.lineTo(resistor.left() + 20 + i * 15, resistor.center().y() + (i % 2 ? 13 : -13));
        path.lineTo(resistor.right(), resistor.center().y());
        painter.drawPath(path);
        callout(painter, QPointF(resistor.right() + 55, resistor.center().y()), 1);
    }

    static void drawWiring(QPainter &painter, const QRectF &area)
    {
        QRectF left(area.left() + 65, area.center().y() - 38, 125, 76);
        QRectF right(area.right() - 190, area.center().y() - 38, 125, 76);
        painter.setPen(QPen(QColor("#344054"), 2));
        painter.setBrush(QColor("#EAF2FC"));
        painter.drawRoundedRect(left, 10, 10);
        painter.drawRoundedRect(right, 10, 10);
        text(painter, left, "Source", 11, true);
        text(painter, right, "LED", 11, true);
        QPointF a(left.right(), left.center().y());
        QPointF b(right.left(), right.center().y());
        painter.setPen(QPen(QColor("#1473E6"), 4));
        QPolygonF wirePath;
        wirePath << a
                 << QPointF(area.center().x(), a.y())
                 << QPointF(area.center().x(), b.y())
                 << b;
        painter.drawPolyline(wirePath);
        painter.setBrush(QColor("#1473E6"));
        painter.drawEllipse(a, 6, 6);
        painter.drawEllipse(b, 6, 6);
        pill(painter, QRectF(area.center().x() - 48, area.top() + 24, 96, 28), "Wire mode", true);
    }

    static void drawSimulation(QPainter &painter, const QRectF &area)
    {
        const qreal left = area.left() + 34;
        const qreal top = area.top() + 26;
        pill(painter, QRectF(left, top, 78, 30), "Run", true);
        pill(painter, QRectF(left + 88, top, 78, 30), "Step");
        pill(painter, QRectF(left + 176, top, 88, 30), "Restart");
        QRectF graph(area.left() + 34, top + 48, area.width() - 68, 100);
        painter.setPen(QPen(QColor("#D9E2EC"), 1));
        painter.setBrush(QColor("#101820"));
        painter.drawRoundedRect(graph, 9, 9);
        painter.setClipRect(graph.adjusted(8, 8, -8, -8));
        painter.setPen(QPen(QColor("#24E0C1"), 3));
        QPainterPath wave;
        wave.moveTo(graph.left() + 10, graph.center().y());
        for (int i = 0; i < 6; ++i) {
            qreal x = graph.left() + 10 + i * (graph.width() - 20) / 6.0;
            wave.lineTo(x, graph.center().y());
            wave.cubicTo(x + 15, graph.top() + 12, x + 30, graph.bottom() - 12,
                         x + (graph.width() - 20) / 6.0, graph.center().y());
        }
        painter.drawPath(wave);
        painter.setClipping(false);
    }

    static void drawCanvas(QPainter &painter, const QRectF &area)
    {
        QRectF dots(area.left() + 30, area.top() + 35, area.width() / 2 - 45, area.height() - 70);
        QRectF lines(area.center().x() + 15, area.top() + 35, area.width() / 2 - 45, area.height() - 70);
        painter.setBrush(QColor("#FAFCFE"));
        painter.setPen(QPen(QColor("#D9E2EC"), 1));
        painter.drawRoundedRect(dots, 9, 9);
        painter.drawRoundedRect(lines, 9, 9);
        painter.setPen(QPen(QColor("#AFC6DE"), 1));
        for (qreal x = dots.left() + 15; x < dots.right(); x += 18)
            for (qreal y = dots.top() + 15; y < dots.bottom(); y += 18)
                painter.drawPoint(QPointF(x, y));
        for (qreal x = lines.left() + 15; x < lines.right(); x += 18)
            painter.drawLine(QPointF(x, lines.top() + 10), QPointF(x, lines.bottom() - 10));
        for (qreal y = lines.top() + 15; y < lines.bottom(); y += 18)
            painter.drawLine(QPointF(lines.left() + 10, y), QPointF(lines.right() - 10, y));
        pill(painter, QRectF(dots.center().x() - 38, dots.bottom() - 34, 76, 24), "Dots");
        pill(painter, QRectF(lines.center().x() - 38, lines.bottom() - 34, 76, 24), "Lines", true);
    }

    static void drawKeyboard(QPainter &painter, const QRectF &area)
    {
        const QStringList keys{"Ctrl + S", "Ctrl + Z", "Ctrl + C", "Ctrl + V", "F5", "F10", "W", "Esc"};
        const qreal keyWidth = (area.width() - 90) / 4.0;
        for (int i = 0; i < keys.size(); ++i) {
            int row = i / 4;
            int column = i % 4;
            QRectF key(area.left() + 30 + column * (keyWidth + 10),
                       area.top() + 42 + row * 58, keyWidth, 38);
            painter.setPen(QPen(QColor("#CAD4DF"), 1));
            painter.setBrush(i == 4 ? QColor("#1473E6") : QColor("#F3F6F9"));
            painter.drawRoundedRect(key, 7, 7);
            text(painter, key, keys.at(i), 9, true, i == 4 ? Qt::white : QColor("#344054"));
        }
    }

    static void drawTroubleshooting(QPainter &painter, const QRectF &area)
    {
        QPointF center(area.left() + 105, area.center().y());
        QPainterPath warning;
        warning.moveTo(center.x(), center.y() - 55);
        warning.lineTo(center.x() - 55, center.y() + 48);
        warning.lineTo(center.x() + 55, center.y() + 48);
        warning.closeSubpath();
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor("#F5B942"));
        painter.drawPath(warning);
        text(painter, QRectF(center.x() - 25, center.y() - 22, 50, 55), "!", 24, true, Qt::white);
        QRectF checklist(area.left() + 205, area.top() + 35, area.width() - 245, area.height() - 70);
        painter.setPen(QPen(QColor("#D9E2EC"), 1));
        painter.setBrush(QColor("#F8FAFC"));
        painter.drawRoundedRect(checklist, 10, 10);
        const QStringList rows{"Check Qt runtime files", "Verify the project path", "Restart the simulation"};
        for (int i = 0; i < rows.size(); ++i) {
            const qreal rowHeight = checklist.height() / rows.size();
            const qreal rowCenter = checklist.top() + rowHeight * (i + 0.5);
            painter.setBrush(QColor("#2EB67D"));
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(QPointF(checklist.left() + 25, rowCenter), 7, 7);
            text(painter, QRectF(checklist.left() + 40, rowCenter - rowHeight / 2,
                                 checklist.width() - 50, rowHeight), rows.at(i), 9, false);
        }
    }

    int m_picture;
};

QString shortcutsTable()
{
    return QStringLiteral(R"(
        <h3>Project and editing</h3>
        <table width="100%" cellspacing="0" cellpadding="7">
          <tr><td><b>Ctrl+N</b></td><td>Open the new-project page</td><td><b>Ctrl+O</b></td><td>Open another project</td></tr>
          <tr><td><b>Ctrl+S</b></td><td>Save</td><td><b>Ctrl+Shift+S</b></td><td>Save As</td></tr>
          <tr><td><b>Ctrl+Z / Ctrl+Y</b></td><td>Undo / Redo</td><td><b>Ctrl+A</b></td><td>Select all components</td></tr>
          <tr><td><b>Ctrl+C / Ctrl+V</b></td><td>Copy / Paste</td><td><b>Delete</b></td><td>Delete the selection</td></tr>
          <tr><td><b>Ctrl+R</b></td><td>Rotate selection</td><td><b>Ctrl+M</b></td><td>Mirror selection</td></tr>
        </table>
        <h3>Canvas and simulation</h3>
        <table width="100%" cellspacing="0" cellpadding="7">
          <tr><td><b>W</b></td><td>Start wire mode</td><td><b>Esc</b></td><td>Cancel wire mode</td></tr>
          <tr><td><b>G</b></td><td>Show or hide the grid</td><td><b>Mouse wheel</b></td><td>Zoom at cursor</td></tr>
          <tr><td><b>Middle drag</b></td><td>Pan the canvas</td><td><b>Shift + drag</b></td><td>Pan with left mouse</td></tr>
          <tr><td><b>F5</b></td><td>Run or pause</td><td><b>F10</b></td><td>Run one simulation step</td></tr>
          <tr><td><b>Ctrl+F5</b></td><td>Restart simulation</td><td><b>F1</b></td><td>Open this Help Center</td></tr>
        </table>
    )");
}
}

HelpDialog::HelpDialog(Topic topic, QWidget *parent) : QDialog(parent)
{
    // فهرست و صفحه‌های راهنما
    setWindowTitle(tr("Circuit Studio Help"));
    setWindowFlag(Qt::Window, true);
    setAttribute(Qt::WA_DeleteOnClose);
    resize(1080, 730);
    setMinimumSize(820, 560);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    auto *header = new QWidget;
    header->setObjectName("helpHeader");
    auto *headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(22, 14, 22, 14);
    auto *brand = new QLabel(tr("?  Circuit Studio Help"));
    brand->setObjectName("helpBrand");
    m_pageTitle = new QLabel;
    m_pageTitle->setObjectName("helpPageTitle");
    headerLayout->addWidget(brand);
    headerLayout->addSpacing(22);
    headerLayout->addWidget(m_pageTitle);
    headerLayout->addStretch();
    auto *close = new QPushButton(tr("Close"));
    connect(close, &QPushButton::clicked, this, &QDialog::close);
    headerLayout->addWidget(close);
    root->addWidget(header);

    auto *splitter = new QSplitter;
    splitter->setChildrenCollapsible(false);
    auto *navigation = new QWidget;
    navigation->setObjectName("helpNavigation");
    navigation->setMinimumWidth(230);
    navigation->setMaximumWidth(300);
    auto *navigationLayout = new QVBoxLayout(navigation);
    navigationLayout->setContentsMargins(16, 18, 16, 18);
    auto *contents = new QLabel(tr("CONTENTS"));
    contents->setObjectName("contentsLabel");
    m_search = new QLineEdit;
    m_search->setPlaceholderText(tr("Search help..."));
    m_search->setClearButtonEnabled(true);
    m_topics = new QListWidget;
    m_topics->setObjectName("helpTopics");
    m_topics->setSpacing(3);
    navigationLayout->addWidget(contents);
    navigationLayout->addWidget(m_search);
    navigationLayout->addWidget(m_topics, 1);

    m_pages = new QStackedWidget;
    addTopic(tr("Getting started"), "overview interface workspace welcome",
             tr("Learn the main parts of Circuit Studio and create your first circuit."),
             tr(R"(<h3>The workspace</h3>
                <ol><li><b>Component Library:</b> search and choose circuit elements.</li>
                <li><b>Canvas:</b> place, select, move and connect components.</li>
                <li><b>Top controls:</b> save, simulate, step and restart the circuit.</li></ol>
                <h3>First circuit</h3>
                <ol><li>Create a project on the Welcome page.</li><li>Add a voltage source, resistor and ground.</li>
                <li>Use Wire mode to connect their terminals.</li><li>Press Run and use Probe to inspect a wire.</li></ol>
                <p><b>Tip:</b> Save regularly with Ctrl+S. Automatic snap keeps components aligned with the selected grid size.</p>)"), 0);
    addTopic(tr("Projects and files"), "new open recent save save as export json home",
             tr("Create, open, save and move between projects without restarting the application."),
             tr(R"(<h3>Welcome page</h3><p>Use <b>Create project</b> for a blank circuit or <b>Open project</b> for an existing file. Recent projects appear on the left.</p>
                <h3>File menu</h3><ul><li><b>New Project:</b> returns to the project creation page.</li>
                <li><b>Open Project:</b> opens another circuit in the same application window.</li>
                <li><b>Back to Welcome:</b> keeps the current editor available through Back to editor.</li>
                <li><b>Save:</b> updates the current file.</li><li><b>Save As:</b> creates a copy at a new path.</li>
                <li><b>Export Image:</b> saves the visible circuit as PNG or JPEG.</li></ul>
                <p>Project files store canvas size, components, connections and supported execution state in JSON format.</p>)"), 1);
    addTopic(tr("Components"), "library part search drag drop properties rotate mirror delete",
             tr("Find components, place them on the canvas and edit their properties."),
             tr(R"(<h3>Adding a component</h3><p>Search by name or category in the library. Double-click a component or drag it onto the canvas. Newly added parts align to the grid.</p>
                <h3>Selecting and editing</h3><ul><li>Click a part to select it; drag empty space for multi-selection.</li>
                <li>Drag selected parts to move them.</li><li>Use Ctrl+R to rotate and Ctrl+M to mirror.</li>
                <li>Press Delete/Backspace to remove selected objects.</li><li>Right-click a component for common actions.</li></ul>
                <p>The preview card below the library shows the selected component's symbol, category and purpose.</p>)"), 2);
    addTopic(tr("Wiring a circuit"), "wire terminal junction connection orthogonal escape",
             tr("Connect component terminals with clean orthogonal wires."),
             tr(R"(<h3>Creating a wire</h3><ol><li>Click <b>Wire</b> or press W.</li><li>Click the first component terminal.</li>
                <li>Move across the canvas and click intermediate points when needed.</li><li>Click the destination terminal to finish.</li></ol>
                <p>Press Esc to cancel an unfinished wire. Junctions and wire points automatically snap to the grid. Use Select mode before moving components.</p>
                <h3>Good practice</h3><p>Use short horizontal and vertical paths, avoid unnecessary crossings, and add junction nodes where several branches must share one electrical net.</p>)"), 3);
    addTopic(tr("Simulation and probes"), "run pause step restart probe voltage log debug",
             tr("Run continuously, advance one tick at a time, or restart the circuit."),
             tr(R"(<h3>Simulation controls</h3><ul><li><b>Run / Pause (F5):</b> starts or pauses continuous simulation.</li>
                <li><b>Step (F10):</b> keeps the circuit paused and advances exactly one simulation tick.</li>
                <li><b>Restart (Ctrl+F5):</b> stops the circuit and resets its runtime state.</li></ul>
                <h3>Probe</h3><p>Enable Probe, then move the pointer over a wire or terminal to inspect its current voltage level. Disable Probe to return to normal editing.</p>
                <h3>Simulation Log</h3><p>The lower log reports simulation actions, history changes and warnings. It can be shown or hidden from View.</p>)"), 4);
    addTopic(tr("Canvas, grid and view"), "canvas grid dots lines snap zoom fit navigator library log",
             tr("Adjust grid appearance, snapping, zoom and visible panels."),
             tr(R"(<h3>Grid options</h3><p>Open <b>View</b> to show or hide the grid, enable automatic snap, choose Dots or Lines, and select the grid size. Preferences are saved for the next launch.</p>
                <h3>Navigation</h3><ul><li>Use the mouse wheel or View menu to zoom.</li><li>Middle-drag or Shift+drag to pan.</li>
                <li>Use Fit Canvas to display the complete sheet.</li><li>The navigator in the lower-right corner provides a map of large circuits.</li></ul>
                <p>The Component Library and Simulation Log can be hidden from View when more canvas space is needed.</p>)"), 5);
    addTopic(tr("Keyboard shortcuts"), "keyboard keys hotkeys ctrl function mouse",
             tr("A quick reference for project, editing, canvas and simulation commands."),
             shortcutsTable(), 6);
    addTopic(tr("Troubleshooting"), "error crash qt dll cmake file load simulation problem",
             tr("Quick checks for common startup, project and simulation problems."),
             tr(R"(<h3>The application does not start</h3><p>If Windows reports error 0xc0000135 or a missing DLL, deploy the Qt runtime next to the executable with <b>windeployqt</b>. Make sure the Qt version matches the MinGW compiler used for the build.</p>
                <h3>CMake cannot find Qt</h3><p>In CLion's CMake settings, add the Qt installation prefix to CMAKE_PREFIX_PATH or set Qt6_DIR to the folder containing Qt6Config.cmake.</p>
                <h3>A project does not open correctly</h3><p>Check that the file exists, is valid JSON and has not been edited with an incompatible structure. Try a recent project from the Welcome page.</p>
                <h3>The circuit gives unexpected results</h3><ul><li>Pause and use Step to inspect state changes.</li><li>Check ground and source connections.</li>
                <li>Use Probe on important wires.</li><li>Read warnings in the Simulation Log.</li><li>Use Restart after changing stateful components.</li></ul>)"), 7);

    splitter->addWidget(navigation);
    splitter->addWidget(m_pages);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes({255, 825});
    root->addWidget(splitter, 1);

    connect(m_topics, &QListWidget::currentRowChanged, this, [this](int row) {
        if (row < 0)
            return;
        m_pages->setCurrentIndex(row);
        m_pageTitle->setText(m_topics->item(row)->text());
    });
    connect(m_search, &QLineEdit::textChanged, this, &HelpDialog::filterTopics);

    setStyleSheet(R"(
        QDialog { background: #F4F7FA; color: #1D2530; }
        QWidget#helpHeader { background: white; border-bottom: 1px solid #DDE3EA; }
        QLabel#helpBrand { color: #1473E6; font-size: 17px; font-weight: 700; }
        QLabel#helpPageTitle { color: #667180; font-size: 14px; }
        QWidget#helpNavigation { background: #F8FAFC; border-right: 1px solid #DDE3EA; }
        QLabel#contentsLabel { color: #7B8794; font-size: 11px; font-weight: 700; }
        QLineEdit { background: white; border: 1px solid #D9E1EA; border-radius: 9px; padding: 9px 11px; }
        QLineEdit:focus { border-color: #1473E6; }
        QListWidget#helpTopics { background: transparent; border: none; outline: none; }
        QListWidget#helpTopics::item { color: #344054; padding: 10px 9px; border-radius: 8px; }
        QListWidget#helpTopics::item:hover { background: #EAF2FC; }
        QListWidget#helpTopics::item:selected { color: #0A66D3; background: #DDEEFF; font-weight: 600; }
        QScrollArea { background: #F4F7FA; border: none; }
        QWidget#helpPage { background: #F4F7FA; }
        QLabel#pageHeading { color: #17202C; font-size: 25px; font-weight: 700; }
        QLabel#pageIntroduction { color: #667180; font-size: 13px; }
        QLabel#pageDetails { color: #344054; font-size: 12px; background: white; border: 1px solid #DDE3EA; border-radius: 12px; padding: 20px; }
        QPushButton { color: #344054; background: #F3F6F9; border: 1px solid #D9E1EA; border-radius: 8px; padding: 7px 15px; }
        QPushButton:hover { background: #E6EFF9; }
        QSplitter::handle { background: #DDE3EA; width: 1px; }
    )");

    selectTopic(topic);
}

void HelpDialog::addTopic(const QString &title, const QString &keywords,
                          const QString &introduction, const QString &details, int picture)
{
    auto *item = new QListWidgetItem(title, m_topics);
    item->setData(Qt::UserRole, title + " " + keywords + " " + introduction);

    auto *scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    auto *page = new QWidget;
    page->setObjectName("helpPage");
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(28, 24, 28, 30);
    layout->setSpacing(14);

    auto *heading = new QLabel(title);
    heading->setObjectName("pageHeading");
    auto *intro = new QLabel(introduction);
    intro->setObjectName("pageIntroduction");
    intro->setWordWrap(true);
    auto *guide = new GuidePicture(picture);
    auto *body = new QLabel(details);
    body->setObjectName("pageDetails");
    body->setWordWrap(true);
    body->setTextFormat(Qt::RichText);
    body->setTextInteractionFlags(Qt::TextSelectableByMouse);
    body->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    layout->addWidget(heading);
    layout->addWidget(intro);
    layout->addWidget(guide);
    layout->addWidget(body);
    layout->addStretch();
    scroll->setWidget(page);
    m_pages->addWidget(scroll);
}

void HelpDialog::filterTopics(const QString &text)
{
    const QString query = text.trimmed();
    int firstVisible = -1;
    for (int i = 0; i < m_topics->count(); ++i) {
        QListWidgetItem *item = m_topics->item(i);
        const bool visible = query.isEmpty() ||
            item->data(Qt::UserRole).toString().contains(query, Qt::CaseInsensitive);
        item->setHidden(!visible);
        if (visible && firstVisible < 0)
            firstVisible = i;
    }
    if (firstVisible >= 0)
        m_topics->setCurrentRow(firstVisible);
}

void HelpDialog::selectTopic(Topic topic)
{
    const int row = qBound(0, static_cast<int>(topic), m_topics->count() - 1);
    m_topics->setCurrentRow(row);
}
