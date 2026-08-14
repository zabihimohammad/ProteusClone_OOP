#pragma once

#include <QDialog>

class QLabel;
class QLineEdit;
class QListWidget;
class QStackedWidget;

class HelpDialog final : public QDialog
{
    Q_OBJECT

public:
    enum Topic {
        Overview,
        Projects,
        Components,
        Wiring,
        Simulation,
        Canvas,
        Shortcuts,
        Troubleshooting
    };

    explicit HelpDialog(Topic topic = Overview, QWidget *parent = nullptr);
    void selectTopic(Topic topic);

private:
    void addTopic(const QString &title, const QString &keywords,
                  const QString &introduction, const QString &details, int picture);
    void filterTopics(const QString &text);

    QLineEdit *m_search = nullptr;
    QListWidget *m_topics = nullptr;
    QStackedWidget *m_pages = nullptr;
    QLabel *m_pageTitle = nullptr;
};
