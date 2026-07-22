#include "startdialog.h"

#include <QComboBox>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QFrame>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QSaveFile>
#include <QSettings>
#include <QSpinBox>
#include <QStandardPaths>
#include <QVBoxLayout>

namespace {
constexpr int kRecentProjectCount = 5;
}

StartDialog::StartDialog(QWidget *parent) : QDialog(parent)
{
    setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint |
                   Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
    setWindowTitle(tr("Welcome to Circuit Studio"));
    resize(920, 590);
    setMinimumSize(760, 520);
    setModal(true);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(38, 30, 38, 30);
    layout->setSpacing(18);

    auto *eyebrow = new QLabel(tr("OOP CORE  /  PROTEUS CLONE"));
    eyebrow->setObjectName("eyebrow");

    auto *headingRow = new QHBoxLayout;
    auto *headingText = new QVBoxLayout;
    auto *title = new QLabel(tr("Design circuits, simply."));
    title->setObjectName("welcomeTitle");
    auto *subtitle = new QLabel(tr("Create a clean workspace or continue from an existing project."));
    subtitle->setObjectName("subtitle");
    headingText->addWidget(title);
    headingText->addWidget(subtitle);

    auto *open = new QPushButton(tr("Open project…"));
    open->setObjectName("openButton");
    auto *create = new QPushButton(tr("Create project  →"));
    create->setObjectName("createButton");
    auto *topActions = new QHBoxLayout;
    topActions->setSpacing(9);
    topActions->addWidget(open);
    topActions->addWidget(create);
    headingRow->addLayout(headingText, 1);
    headingRow->addLayout(topActions);

    auto *body = new QHBoxLayout;
    body->setSpacing(16);

    auto *recentCard = new QFrame;
    recentCard->setObjectName("recentCard");
    recentCard->setMinimumWidth(260);
    recentCard->setMaximumWidth(320);
    auto *recentLayout = new QVBoxLayout(recentCard);
    recentLayout->setContentsMargins(18, 18, 18, 18);
    auto *recentTitle = new QLabel(tr("Recent projects"));
    recentTitle->setObjectName("cardTitle");
    auto *recentHint = new QLabel(tr("Your five most recent workspaces"));
    recentHint->setObjectName("cardHint");
    m_recent = new QListWidget;
    m_recent->setObjectName("recentProjects");
    m_recent->setSpacing(3);
    recentLayout->addWidget(recentTitle);
    recentLayout->addWidget(recentHint);
    recentLayout->addSpacing(5);
    recentLayout->addWidget(m_recent, 1);

    auto *helpCard = new QFrame;
    helpCard->setObjectName("sideCard");
    auto *helpLayout = new QVBoxLayout(helpCard);
    helpLayout->setContentsMargins(16, 13, 16, 13);
    helpLayout->setSpacing(5);
    auto *helpTitle = new QLabel(tr("Help"));
    helpTitle->setObjectName("sideCardTitle");
    auto *gettingStarted = new QPushButton(tr("Getting started  →"));
    gettingStarted->setObjectName("linkButton");
    auto *shortcuts = new QPushButton(tr("Keyboard shortcuts  →"));
    shortcuts->setObjectName("linkButton");
    helpLayout->addWidget(helpTitle);
    helpLayout->addWidget(gettingStarted);
    helpLayout->addWidget(shortcuts);

    auto *aboutCard = new QFrame;
    aboutCard->setObjectName("sideCard");
    auto *aboutLayout = new QVBoxLayout(aboutCard);
    aboutLayout->setContentsMargins(16, 13, 16, 13);
    aboutLayout->setSpacing(3);
    auto *aboutTitle = new QLabel(tr("About"));
    aboutTitle->setObjectName("sideCardTitle");
    auto *aboutText = new QLabel(tr("Circuit Studio  •  v0.1\nOOP Core — Proteus Clone"));
    aboutText->setObjectName("aboutText");
    auto *aboutDetails = new QPushButton(tr("Project details  →"));
    aboutDetails->setObjectName("linkButton");
    aboutLayout->addWidget(aboutTitle);
    aboutLayout->addWidget(aboutText);
    aboutLayout->addWidget(aboutDetails);

    auto *leftColumn = new QWidget;
    auto *leftLayout = new QVBoxLayout(leftColumn);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(10);
    leftLayout->addWidget(recentCard, 1);
    leftLayout->addWidget(helpCard);
    leftLayout->addWidget(aboutCard);

    auto *projectCard = new QFrame;
    projectCard->setObjectName("projectCard");
    auto *projectLayout = new QVBoxLayout(projectCard);
    projectLayout->setContentsMargins(22, 20, 22, 20);
    projectLayout->setSpacing(14);
    auto *projectTitle = new QLabel(tr("New project"));
    projectTitle->setObjectName("cardTitle");
    auto *projectHint = new QLabel(tr("Choose a name, save location, and canvas preset."));
    projectHint->setObjectName("cardHint");

    auto *form = new QFormLayout;
    form->setSpacing(13);
    form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    m_name = new QLineEdit(tr("Untitled circuit"));

    m_location = new QLineEdit;
    const QString documents = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    m_location->setText(QDir(documents).filePath("CircuitStudio"));
    auto *locationRow = new QWidget;
    auto *locationLayout = new QHBoxLayout(locationRow);
    locationLayout->setContentsMargins(0, 0, 0, 0);
    locationLayout->setSpacing(8);
    auto *browse = new QPushButton(tr("Browse…"));
    locationLayout->addWidget(m_location, 1);
    locationLayout->addWidget(browse);

    m_sizePreset = new QComboBox;
    m_sizePreset->addItem(tr("Default — 1600 × 1000"), QSize(1600, 1000));
    m_sizePreset->addItem(tr("Landscape A4 — 1600 × 1131"), QSize(1600, 1131));
    m_sizePreset->addItem(tr("Landscape A3 — 2263 × 1600"), QSize(2263, 1600));
    m_sizePreset->addItem(tr("Portrait A4 — 1131 × 1600"), QSize(1131, 1600));
    m_sizePreset->addItem(tr("HD — 1920 × 1080"), QSize(1920, 1080));
    m_sizePreset->addItem(tr("Custom"), QSize());

    m_width = new QSpinBox;
    m_height = new QSpinBox;
    for (auto *spin : {m_width, m_height}) {
        spin->setRange(400, 10000);
        spin->setSingleStep(100);
        spin->setSuffix(tr(" px"));
    }
    m_width->setValue(1600);
    m_height->setValue(1000);
    auto *dimensions = new QWidget;
    auto *dimensionLayout = new QHBoxLayout(dimensions);
    dimensionLayout->setContentsMargins(0, 0, 0, 0);
    dimensionLayout->setSpacing(8);
    dimensionLayout->addWidget(m_width);
    dimensionLayout->addWidget(new QLabel("×"));
    dimensionLayout->addWidget(m_height);

    form->addRow(tr("Project name"), m_name);
    form->addRow(tr("Save location"), locationRow);
    form->addRow(tr("Canvas preset"), m_sizePreset);
    form->addRow(tr("Canvas size"), dimensions);
    projectLayout->addWidget(projectTitle);
    projectLayout->addWidget(projectHint);
    projectLayout->addSpacing(5);
    projectLayout->addLayout(form);
    projectLayout->addStretch();

    body->addWidget(leftColumn);
    body->addWidget(projectCard, 1);
    layout->addWidget(eyebrow);
    layout->addLayout(headingRow);
    layout->addLayout(body, 1);

    connect(create, &QPushButton::clicked, this, &StartDialog::createProject);
    connect(open, &QPushButton::clicked, this, &StartDialog::openProject);
    connect(browse, &QPushButton::clicked, this, [this] {
        const QString directory = QFileDialog::getExistingDirectory(
            this, tr("Choose project location"), m_location->text());
        if (!directory.isEmpty())
            m_location->setText(QDir::toNativeSeparators(directory));
    });
    connect(m_sizePreset, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int index) {
        const QSize size = m_sizePreset->itemData(index).toSize();
        if (size.isValid()) {
            m_width->setValue(size.width());
            m_height->setValue(size.height());
        }
    });
    const auto selectCustomSize = [this] {
        if (m_sizePreset->currentIndex() != m_sizePreset->count() - 1)
            m_sizePreset->setCurrentIndex(m_sizePreset->count() - 1);
    };
    connect(m_width, &QSpinBox::editingFinished, this, selectCustomSize);
    connect(m_height, &QSpinBox::editingFinished, this, selectCustomSize);
    connect(m_recent, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *item) {
        const QString path = item->data(Qt::UserRole).toString();
        if (path.isEmpty())
            return;
        if (!QFileInfo::exists(path)) {
            QMessageBox::warning(this, tr("Project not found"),
                                 tr("This project no longer exists at:\n%1").arg(path));
            return;
        }
        m_projectFilePath = path;
        m_name->setText(QFileInfo(path).completeBaseName());
        applyProjectMetadata(path);
        rememberProject(path);
        accept();
    });
    connect(gettingStarted, &QPushButton::clicked, this, [this] {
        QMessageBox::information(
            this, tr("Getting started"),
            tr("1. Enter a project name.\n"
               "2. Choose a save location.\n"
               "3. Select a standard canvas size or Custom.\n"
               "4. Click Create project.\n\n"
               "In the editor, use the component library on the left and double-click a component to activate it."));
    });
    connect(shortcuts, &QPushButton::clicked, this, [this] {
        QMessageBox::information(
            this, tr("Keyboard shortcuts"),
            tr("Zoom in:  +\nZoom out:  −\nReset zoom:  0\n"
               "Pan canvas:  Middle mouse drag or Shift + drag"));
    });
    connect(aboutDetails, &QPushButton::clicked, this, [this] {
        QMessageBox::about(
            this, tr("About Circuit Studio"),
            tr("Circuit Studio v0.1\n\n"
               "A C++ and Qt implementation of the OOP Proteus Clone project.\n\n"
               "Team: Shahab, Mohammad Javad, Yasin\n"
               "UI/UX and subprojects 1–2: Shahab"));
    });

    loadRecentProjects();

    setStyleSheet(R"(
        QDialog { background: #F2F5F9; color: #1D2530; }
        QDialog QWidget { color: #1D2530; }
        QLabel { color: #3F4A59; background: transparent; }
        QLabel#eyebrow { color: #1473E6; font-weight: 700; letter-spacing: 1px; }
        QLabel#welcomeTitle { color: #17202C; font-size: 30px; font-weight: 700; }
        QLabel#subtitle, QLabel#cardHint { color: #667180; }
        QLabel#cardTitle { color: #17202C; font-size: 17px; font-weight: 700; }
        QLabel#sideCardTitle { color: #17202C; font-size: 14px; font-weight: 700; }
        QLabel#aboutText { color: #667180; font-size: 11px; }
        QFrame#recentCard, QFrame#projectCard {
            background: white; border: 1px solid #DDE3EA; border-radius: 16px;
        }
        QFrame#sideCard {
            background: white; border: 1px solid #DDE3EA; border-radius: 13px;
        }
        QLineEdit, QSpinBox, QComboBox {
            color: #17202C; background: #F2F4F7; border: 1px solid transparent;
            border-radius: 9px; padding: 8px; selection-color: white;
            selection-background-color: #1473E6;
        }
        QLineEdit:focus, QSpinBox:focus, QComboBox:focus { background: white; border-color: #1473E6; }
        QListWidget#recentProjects { background: transparent; border: none; outline: none; }
        QListWidget#recentProjects::item { color: #253143; padding: 9px; border-radius: 9px; }
        QListWidget#recentProjects::item:hover { background: #F0F5FB; }
        QListWidget#recentProjects::item:selected { color: #0A66D3; background: #E8F2FF; }
        QPushButton {
            color: #253143; background: white; border: 1px solid #D9DFE7;
            border-radius: 10px; padding: 10px 16px; font-weight: 600;
        }
        QPushButton:hover { background: #EAF2FC; }
        QPushButton#linkButton {
            color: #0A66D3; background: transparent; border: none; border-radius: 6px;
            padding: 3px 0; text-align: left; font-weight: 500;
        }
        QPushButton#linkButton:hover { color: #084F9F; background: #F2F7FD; }
        QPushButton#createButton { background: #1473E6; border: none; color: white; }
        QPushButton#createButton:hover { background: #0E65CF; }
    )");
}

void StartDialog::createProject()
{
    const QString name = m_name->text().trimmed();
    const QString location = QDir::fromNativeSeparators(m_location->text().trimmed());
    if (name.isEmpty() || location.isEmpty()) {
        QMessageBox::warning(this, tr("Missing information"),
                             tr("Enter both a project name and save location."));
        return;
    }

    QDir directory;
    if (!directory.mkpath(location)) {
        QMessageBox::critical(this, tr("Cannot create project"),
                              tr("The selected save location could not be created."));
        return;
    }

    QString safeName = name;
    safeName.replace(QRegularExpression(R"([\\/:*?"<>|])"), "_");
    m_projectFilePath = QDir(location).filePath(safeName + ".circuit.json");
    if (QFileInfo::exists(m_projectFilePath)) {
        const auto answer = QMessageBox::question(
            this, tr("Replace existing project"),
            tr("A project with this name already exists. Replace it?"));
        if (answer != QMessageBox::Yes)
            return;
    }

    QJsonObject canvas{{"width", m_width->value()}, {"height", m_height->value()}};
    QJsonObject project{{"name", name}, {"canvas", canvas},
                        {"elements", QJsonArray{}}, {"wires", QJsonArray{}}};
    QSaveFile file(m_projectFilePath);
    if (!file.open(QIODevice::WriteOnly) ||
        file.write(QJsonDocument(project).toJson(QJsonDocument::Indented)) < 0 || !file.commit()) {
        QMessageBox::critical(this, tr("Cannot save project"), file.errorString());
        m_projectFilePath.clear();
        return;
    }

    rememberProject(m_projectFilePath);
    accept();
}

void StartDialog::openProject()
{
    const QString path = QFileDialog::getOpenFileName(
        this, tr("Open circuit project"), m_location->text(),
        tr("Circuit projects (*.circuit.json *.json *.txt);;All files (*)"));
    if (path.isEmpty())
        return;
    m_projectFilePath = path;
    m_name->setText(QFileInfo(path).completeBaseName().remove(".circuit"));
    applyProjectMetadata(path);
    rememberProject(path);
    accept();
}

void StartDialog::applyProjectMetadata(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) return;
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll());
    const QJsonObject canvas = document.object().value("canvas").toObject();
    const int width = canvas.value("width").toInt();
    const int height = canvas.value("height").toInt();
    if (width >= m_width->minimum() && height >= m_height->minimum()) {
        m_width->setValue(width);
        m_height->setValue(height);
        m_sizePreset->setCurrentIndex(m_sizePreset->count() - 1);
    }
    const QString savedName = document.object().value("name").toString();
    if (!savedName.isEmpty()) m_name->setText(savedName);
}

void StartDialog::loadRecentProjects()
{
    const QStringList paths = QSettings().value("recentProjects").toStringList();
    for (int i = 0; i < kRecentProjectCount; ++i) {
        if (i < paths.size()) {
            const QFileInfo info(paths.at(i));
            auto *item = new QListWidgetItem(QString("%1\n%2")
                                                  .arg(info.completeBaseName().remove(".circuit"),
                                                       QDir::toNativeSeparators(info.absolutePath())));
            item->setData(Qt::UserRole, paths.at(i));
            item->setToolTip(QDir::toNativeSeparators(paths.at(i)));
            m_recent->addItem(item);
        } else {
            auto *item = new QListWidgetItem(tr("No recent project"));
            item->setFlags(Qt::NoItemFlags);
            item->setForeground(QColor("#9AA3AF"));
            m_recent->addItem(item);
        }
    }
}

void StartDialog::rememberProject(const QString &path)
{
    QSettings settings;
    QStringList paths = settings.value("recentProjects").toStringList();
    paths.removeAll(path);
    paths.prepend(path);
    while (paths.size() > kRecentProjectCount)
        paths.removeLast();
    settings.setValue("recentProjects", paths);
}

QString StartDialog::projectName() const
{
    return m_name->text().trimmed();
}

QString StartDialog::projectFilePath() const
{
    return m_projectFilePath;
}

QSize StartDialog::canvasSize() const
{
    return {m_width->value(), m_height->value()};
}
