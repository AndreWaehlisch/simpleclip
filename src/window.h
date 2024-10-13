#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QClipboard>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QPushButton>
#include <QCloseEvent>
#include <QPixmap>
#include <QFileIconProvider>
#include <QSystemTrayIcon>
#include <QMenu>

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
    ~Window();

public slots:
    void button_up_clicked();
    void button_down_clicked();
    void button_delete_clicked();
    void button_clear_clicked();

private slots:
    void clipboard_updated();
    void tray_clicked(QSystemTrayIcon::ActivationReason reason);

private:
    const QFileIconProvider iconDB;
    QPixmap webPixmap;
    QPixmap foldersPixmap;
    QTableWidget *historyTable;
    QClipboard *clipboard;
    QSystemTrayIcon *tray;
    QMenu *trayMenu;
    size_t last_hash;
    bool clipboardUpdate = true; // only parse updates from outside our program

    QPushButton *button_up;
    QPushButton *button_down;
    QPushButton *button_delete;
    QPushButton *button_clear;

    void closeEvent(QCloseEvent *event) override;
    void setNewClipboard();
};

#endif
