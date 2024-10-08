#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QClipboard>
#include <QTableWidget>
#include <QPushButton>
#include <QCloseEvent>

#include <QDebug>

class Window : public QWidget {
    Q_OBJECT

    public:
        Window(QWidget *parent = nullptr);

    public slots:
        void button_up_clicked();
        void button_down_clicked();
        void button_delete_clicked();
        void button_clear_clicked();

    private slots:
        void clipboard_updated();

    private:
        QTableWidget *historyTable;
        QClipboard *clipboard;
        uint last_hash;

        QPushButton *button_up;
        QPushButton *button_down;
        QPushButton *button_delete;
        QPushButton *button_clear;

        void closeEvent(QCloseEvent *event) override;
};

#endif
