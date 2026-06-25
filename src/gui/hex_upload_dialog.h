#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class HexUploadDialog : public QDialog {
Q_OBJECT
public:
    explicit HexUploadDialog(QWidget *parent = nullptr);

    // تابعی برای گرفتن مسیر فایلی که کاربر انتخاب کرده است
    QString getSelectedFilePath() const;

private slots:
    // اسلات (تابع) اختصاصی برای مدیریت کلیک روی دکمه Browse
    void onBrowseClicked();

private:
    QLabel *label;
    QLineEdit *pathLineEdit;   // کادر متنی نمایش مسیر فایل
    QPushButton *browseButton; // دکمه انتخاب فایل
    QPushButton *okButton;     // دکمه تایید نهایی
};