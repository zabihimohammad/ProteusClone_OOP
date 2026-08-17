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
    // ساخت فرم از ویژگی‌ها
    explicit PropertiesDialog(const QString &title, const QMap<QString, QString> &properties, QWidget *parent = nullptr);

    // مقادیر ویرایش‌شده
    QMap<QString, QString> getUpdatedProperties() const;

private:
    // ورودی هر ویژگی
    QMap<QString, QLineEdit*> inputFields;

    QPushButton *btnSave;
    QPushButton *btnCancel;

    void setupUI(const QString &title, const QMap<QString, QString> &properties);
    void applyStyles();
};
