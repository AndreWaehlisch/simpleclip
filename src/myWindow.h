#ifndef MYWINDOW_H
#define MYWINDOW_H

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

enum clipboard_dataIDs {
    clipboardID_data = Qt::UserRole,
    clipboardID_formats
};

class myWindow : public QWidget
{
    Q_OBJECT

public:
    myWindow();

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
    void openFileOfItem(const int, const int);
    void showWindow();

    static QString trimText(const QString);
};

#endif
