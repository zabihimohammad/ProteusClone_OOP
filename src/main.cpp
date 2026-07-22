#include <QApplication>
#include "gui/mainwindow.h"
#include "ui/startdialog.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QApplication::setApplicationName("Circuit Studio");
    QApplication::setOrganizationName("OOP Core");
    QApplication::setFont(QFont("Segoe UI", 10));

    StartDialog start;
    if (start.exec() != QDialog::Accepted) return 0;

    MainWindow w(start.canvasSize(), start.projectFilePath());
    if (!start.projectName().isEmpty())
        w.setWindowTitle(start.projectName() + " — Circuit Studio");
    w.show();
    return a.exec();
}
