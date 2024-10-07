#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QClipboard>
#include <QTableWidget>
#include <QPushButton>

#include <QDebug>

#define MAXROWS 5

class Window : public QWidget {
    Q_OBJECT

    public:
        Window(QWidget *parent = nullptr);

    private slots:
        void clipboard_updated();
        void button_up_clicked();
        void button_down_clicked();
        void button_delete_clicked();
        void button_clear_clicked();

    private:
        QTableWidget *historyTable;
        QClipboard *clipboard;
        uint last_hash;

        QPushButton *button_up;
        QPushButton *button_down;
        QPushButton *button_delete;
        QPushButton *button_clear;
};

#endif
