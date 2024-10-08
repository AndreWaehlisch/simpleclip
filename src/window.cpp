#include <QGuiApplication>
#include <QGridLayout>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QTableWidgetSelectionRange>
#include <QSettings>
#include <QByteArray>
#include <QMimeData>
#include <QHash>
#include <QTime>

#include "window.h"

#define MAXROWS 5 // maximum number of rows our QTableWidget will grow to

Window::Window(QWidget *parent) : QWidget(parent) {
    setWindowTitle("simpleclip");

    const QSize size(300, 500);
    resize(size);
    setMinimumSize(size);
    setMaximumSize(size);

    historyTable = new QTableWidget(0, 2);
    historyTable->setSortingEnabled(false);
    historyTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    historyTable->setSelectionMode(QAbstractItemView::SingleSelection);
    const QStringList labels = { tr("Content"), tr("Timestamp") };
    historyTable->setHorizontalHeaderLabels(labels);
    historyTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

    button_up = new QPushButton("^");
    button_down = new QPushButton("v");
    button_delete = new QPushButton("x");
    button_clear = new QPushButton(tr("clear"));

    connect(button_up, &QAbstractButton::clicked, this, &Window::button_up_clicked);
    connect(button_down, &QAbstractButton::clicked, this, &Window::button_down_clicked);
    connect(button_delete, &QAbstractButton::clicked, this, &Window::button_delete_clicked);
    connect(button_clear, &QAbstractButton::clicked, this, &Window::button_clear_clicked);

    QGridLayout *mainLayout = new QGridLayout;

    mainLayout->addWidget(historyTable, 0, 0, 1, 4);
    mainLayout->addWidget(button_up, 1, 0);
    mainLayout->addWidget(button_down, 1, 1);
    mainLayout->addWidget(button_delete, 1, 2);
    mainLayout->addWidget(button_clear, 1, 3);

    setLayout(mainLayout);

    clipboard = QGuiApplication::clipboard();
    connect(clipboard, &QClipboard::dataChanged, this, &Window::clipboard_updated);
}

void Window::clipboard_updated(){
    const QMimeData * const clipboard_mimedata = clipboard->mimeData(QClipboard::Clipboard);
    const QStringList clipboard_mimeformats = clipboard_mimedata->formats();
    const QByteArray clipboard_bytearray = clipboard_mimedata->data(clipboard_mimeformats[0]);
    const uint clipboard_hash = qHash(clipboard_bytearray);

    // only act on new items
    if (clipboard_hash != last_hash) {
        last_hash = clipboard_hash;
        qDebug() << clipboard->text() << clipboard_hash << clipboard_mimedata << clipboard_mimedata->hasImage() << clipboard_mimeformats[0];

        const int row = historyTable->rowCount();

        // add new column only if space availble (defined by MAXROWS macro)
        if ( row < MAXROWS ) {
            historyTable->insertRow(row);
        } else {
            // shuffle items
            // TODO
        }

        // set new item on bottom
        //TODO
        QTableWidgetItem *entry = new QTableWidgetItem();
        entry->setText(clipboard->text());
        entry->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        entry->setFlags(entry->flags() ^ Qt::ItemIsEditable);
        historyTable->setItem(row, 0, entry);

        // set timestamp for current item
        const QTime now = QTime::currentTime();
        QTableWidgetItem *timestamp = new QTableWidgetItem();
        timestamp->setText(now.toString());
        timestamp->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        timestamp->setFlags(timestamp->flags() ^ Qt::ItemIsEditable);
        historyTable->setItem(row, 1, timestamp);

        historyTable->selectRow(row);
    }
}

void Window::button_up_clicked(){
    const int row = historyTable->currentRow();

    if ( row > 0 ) {
        historyTable->selectRow(row - 1);
    }
}

void Window::button_down_clicked(){
    const int row = historyTable->currentRow();

    if ( (row < MAXROWS) && (row != -1) ) {
        historyTable->selectRow(row + 1);
    }
}

void Window::button_delete_clicked(){
    // TODO
}

void Window::button_clear_clicked(){
    historyTable->clearContents();
    historyTable->setRowCount(0);
}

void Window::closeEvent(QCloseEvent *event) {
    QSettings settings;
    settings.setValue("window_geometry", saveGeometry());
    QWidget::closeEvent(event);
}
