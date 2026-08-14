#include <QApplication>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>
#include <QStackedWidget>

#include <functional>

#include "gui/mainwindow.h"
#include "ui/startdialog.h"

namespace {
struct ProjectInfo {
    QSize canvasSize{1600, 1000};
    QString name;
};

ProjectInfo projectInfo(const QString &path)
{
    ProjectInfo info;
    info.name = QFileInfo(path).completeBaseName().remove(".circuit");

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return info;

    const QJsonObject object = QJsonDocument::fromJson(file.readAll()).object();
    const QJsonObject canvas = object.value("canvas").toObject();
    const int width = canvas.value("width").toInt();
    const int height = canvas.value("height").toInt();
    if (width > 0 && height > 0)
        info.canvasSize = QSize(width, height);

    const QString savedName = object.value("name").toString().trimmed();
    if (!savedName.isEmpty())
        info.name = savedName;
    return info;
}

void rememberProject(const QString &path)
{
    QSettings settings;
    QStringList paths = settings.value("recentProjects").toStringList();
    paths.removeAll(path);
    paths.prepend(path);
    while (paths.size() > 5)
        paths.removeLast();
    settings.setValue("recentProjects", paths);
}
}

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    QApplication::setApplicationName("Circuit Studio");
    QApplication::setOrganizationName("OOP Core");
    QApplication::setFont(QFont("Segoe UI", 10));

    QStackedWidget window;
    window.resize(1280, 780);
    window.setMinimumSize(900, 600);
    window.setWindowTitle(QObject::tr("Welcome to Circuit Studio"));

    auto *start = new StartDialog;
    start->setWindowFlags(Qt::Widget);
    start->setModal(false);
    window.addWidget(start);

    MainWindow *editor = nullptr;

    const auto showWelcome = [&] {
        start->setBackAvailable(editor != nullptr);
        start->refreshRecentProjects();
        start->show();
        window.setCurrentWidget(start);
        window.setWindowTitle(QObject::tr("Welcome to Circuit Studio"));
    };

    std::function<void(const QSize &, const QString &, const QString &)> showEditor;
    showEditor = [&](const QSize &canvasSize, const QString &path,
                     const QString &projectName) {
        MainWindow *oldEditor = editor;
        editor = new MainWindow(canvasSize, path);
        editor->setWindowFlags(Qt::Widget);
        if (!projectName.isEmpty())
            editor->setWindowTitle(projectName + QObject::tr(" — Circuit Studio"));

        window.addWidget(editor);
        window.setCurrentWidget(editor);
        window.setWindowTitle(editor->windowTitle());

        QObject::connect(editor, &QWidget::windowTitleChanged, &window,
                         &QWidget::setWindowTitle);
        QObject::connect(editor, &MainWindow::homeRequested, &window, showWelcome);
        QObject::connect(editor, &MainWindow::newProjectRequested, &window, showWelcome);
        QObject::connect(editor, &MainWindow::openProjectRequested, &window, [&] {
            const QString selectedPath = QFileDialog::getOpenFileName(
                &window, QObject::tr("Open circuit project"), {},
                QObject::tr("Circuit projects (*.circuit.json *.json *.txt);;All files (*)"));
            if (selectedPath.isEmpty())
                return;
            rememberProject(selectedPath);
            const ProjectInfo info = projectInfo(selectedPath);
            showEditor(info.canvasSize, selectedPath, info.name);
        });

        if (oldEditor) {
            window.removeWidget(oldEditor);
            oldEditor->deleteLater();
        }
    };

    QObject::connect(start, &QDialog::accepted, &window, [&] {
        showEditor(start->canvasSize(), start->projectFilePath(), start->projectName());
    });
    QObject::connect(start, &StartDialog::backRequested, &window, [&] {
        if (!editor)
            return;
        window.setCurrentWidget(editor);
        window.setWindowTitle(editor->windowTitle());
    });

    window.showMaximized();
    return application.exec();
}
