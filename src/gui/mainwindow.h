#pragma once
#include <QMainWindow>

// برای افزایش سرعت کامپایل و جلوگیری از تداخل هدرها، کلاس‌های گرافیکی را Forward Declare می‌کنیم
class CircuitScene;
class CircuitView;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    // تعریف پوینترها (نشانی‌ها) به بوم طراحی و دوربین نمایش آن
    // این متغیرها در طول اجرای برنامه، دسترسی ما را به صفحه شطرنجی حفظ می‌کنند
    CircuitScene *scene;
    CircuitView *view;
};