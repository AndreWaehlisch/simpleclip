#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QClipboard>
#include <QTableWidget>
#include <QPushButton>
#include <QCloseEvent>
#include <QPixmap>
#include <QFileIconProvider>
#include <QMenu>
#include <QRegularExpression>
#include <QSystemTrayIcon>

#include <QDebug>

enum clipboard_dataIDs {
    clipboardID_data = Qt::UserRole,
    clipboardID_formats
};

class Window : public QWidget
{
    Q_OBJECT

public:
    Window();

public slots:
    void button_up_clicked();
    void button_down_clicked();

private slots:
    void clipboard_updated();
    void tray_clicked(QSystemTrayIcon::ActivationReason);
    void button_clear_clicked();
    void button_delete_clicked();
    void myTimerEvent();

private:
    const QFileIconProvider iconDB;
    QPixmap webPixmap;
    QPixmap foldersPixmap;
    QTableWidget *historyTable;
    QClipboard *clipboard;
    QMenu *trayMenu;
    size_t last_hash;
    bool clipboardUpdate = true; // only parse updates from outside our program
    const QRegularExpression regex = QRegularExpression(R"(^file:///(.+)$)", QRegularExpression::MultilineOption | QRegularExpression::InvertedGreedinessOption);

    QPushButton *button_up;
    QPushButton *button_down;
    QPushButton *button_delete;
    QPushButton *button_clear;

    void closeEvent(QCloseEvent *) override;
    void setNewClipboard();

    static QString trimText(const QString);
};

#endif
