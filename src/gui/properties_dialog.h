#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QMap>

class PropertiesDialog : public QDialog {
Q_OBJECT

public:
    // سازنده حالا یک لیست از ویژگی‌ها (Key-Value) را دریافت می‌کند
    explicit PropertiesDialog(const QString &title, const QMap<QString, QString> &properties, QWidget *parent = nullptr);

    // برگرداندن مقادیر جدید به قطعه
    QMap<QString, QString> getUpdatedProperties() const;

private:
    // نقشه‌ای برای ذخیره و دسترسی به تمام فیلدهای متنیِ ساخته شده
    QMap<QString, QLineEdit*> inputFields;

    QPushButton *btnSave;
    QPushButton *btnCancel;

    void setupUI(const QString &title, const QMap<QString, QString> &properties);
    void applyStyles();
};