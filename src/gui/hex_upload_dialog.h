#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class HexUploadDialog : public QDialog {
Q_OBJECT
public:
    explicit HexUploadDialog(QWidget *parent = nullptr);

    // مسیر فایل انتخاب‌شده
    QString getSelectedFilePath() const;

private slots:
    // انتخاب فایل HEX
    void onBrowseClicked();

private:
    QLabel *label;
    QLineEdit *pathLineEdit;   // کادر متنی نمایش مسیر فایل
    QPushButton *browseButton; // دکمه انتخاب فایل
    QPushButton *okButton;     // دکمه تایید نهایی
};