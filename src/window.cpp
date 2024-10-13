#include <QGuiApplication>
#include <QGridLayout>
#include <QHeaderView>
#include <QSettings>
#include <QByteArray>
#include <QMimeData>
#include <QHash>
#include <QTime>
#include <QList>
#include <QPixmap>
#include <QFileInfo>
#include <QMessageBox>

#include "window.h"

#define WINDOW_WIDTH 300 // total width of window (in px)
#define WINDOW_HEIGHT 500 // total height of window
#define IMAGEHEIGHT 120 // default height of images; also the maximum height of each row
#define MAXROWS 55 // maximum number of rows our QTableWidget will grow to

Window::~Window()
{
    delete historyTable;
    delete button_up;
    delete button_down;
    delete button_delete;
    delete button_clear;
    delete tray;
    //TODO: can we avoid this by setting app as parent of the QWidgets?
}

Window::Window() : QWidget(nullptr)
{
    setWindowTitle("simpleclip");

    const QIcon mainIcon(":/icon.ico");
    const QIcon webIcon(":/web.ico");
    const QIcon foldersIcon(":/folders.ico");
    webPixmap = webIcon.pixmap(IMAGEHEIGHT);
    foldersPixmap = foldersIcon.pixmap(IMAGEHEIGHT);

    const QSize windowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    resize(windowSize);
    setMinimumSize(windowSize);
    setMaximumSize(windowSize);

    trayMenu = new QMenu();
    trayMenu->addAction(windowTitle())->setDisabled(true); // first (disabled) action is a header (=our name)
    trayMenu->addAction(tr("Clear"), this, &Window::button_clear_clicked);
    trayMenu->addSeparator();
    trayMenu->addAction("TODO1", this, &Window::button_clear_clicked); //TODO: show a preview of (10?) clipboard entries, see: tray_clicked and https://doc.qt.io/qt-6/qwidget.html#insertAction
    trayMenu->addAction("TODO2", this, &Window::button_clear_clicked);
    trayMenu->addAction("TODO3", this, &Window::button_clear_clicked);
    trayMenu->addAction("TODO4", this, &Window::button_clear_clicked);
    trayMenu->addAction("TODO5", this, &Window::button_clear_clicked);
    trayMenu->addSeparator();
    trayMenu->addAction(tr("Exit"), this, &Window::close);

    tray = new QSystemTrayIcon(mainIcon);
    connect(tray, &QSystemTrayIcon::activated, this, &Window::tray_clicked);
    tray->setContextMenu(trayMenu);
    tray->show();

    historyTable = new QTableWidget(0, 2);
    historyTable->setSortingEnabled(false);
    historyTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    historyTable->setSelectionMode(QAbstractItemView::SingleSelection);
    const QStringList labels = { tr("Content"), tr("Timestamp") };
    historyTable->setHorizontalHeaderLabels(labels);
    historyTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

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
    clipboard_updated();
    connect(clipboard, &QClipboard::dataChanged, this, &Window::clipboard_updated);
}

void Window::tray_clicked(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger) {
        activateWindow();
    } else if (reason == QSystemTrayIcon::Context) {
        qDebug() << "right-click on tray";
    }
}

void Window::clipboard_updated()
{
    if (!clipboardUpdate) {
        clipboardUpdate = true;
        qDebug() << "SWALLOW";
        return;
    }

    const QMimeData * const clipboard_mimedata = clipboard->mimeData(QClipboard::Clipboard);
    const QStringList clipboard_mimeformats = clipboard_mimedata->formats();

    if (clipboard_mimeformats.isEmpty())
        return; //TODO: if last was not empty, but the system cleared the clipboard (it is now empty), we should display that

    const QString firstFormat = clipboard_mimeformats.first();
    const QByteArray clipboard_firstByteArray = clipboard_mimedata->data(firstFormat);
    const size_t clipboard_hash = qHash(clipboard_firstByteArray);

    // only act on new items
    if (clipboard_hash != last_hash) {
        last_hash = clipboard_hash;
        qDebug() << "RECEIVE:" << clipboard->text() << clipboard_hash << clipboard_mimedata << clipboard_mimedata->hasImage() << firstFormat;
        qDebug() << "FORMATLIST:" << clipboard_mimeformats;
        int rowCount = historyTable->rowCount();

        // make space for a new row if already maxed out
        if (rowCount >= MAXROWS) {
            historyTable->removeRow(rowCount - 1);
        }

        // create new row
        historyTable->insertRow(0);

        // create new item
        QTableWidgetItem *entry = new QTableWidgetItem();
        entry->setFlags(entry->flags() ^ Qt::ItemIsEditable);

        // first, check if image
        QPixmap pixmap;
        if (clipboard_mimedata->hasImage()) {
            const QImage image = clipboard->image(QClipboard::Clipboard);
            pixmap = QPixmap::fromImage(image);
        } else if (clipboard_mimedata->hasText()) {
            const QString clipboardtext = clipboard->text();
            const QStringList splitList = clipboardtext.split(R"(file:///)"); // https://en.cppreference.com/w/cpp/language/string_literal
            const qsizetype listSize = splitList.size();

            if (listSize > 2) {
                // handle a list of files in the format "file:///C:/....file:///C:/...."
                pixmap = foldersPixmap;
            } else if (listSize == 2) {
                // handle (single) files in the format "file:///C:/...."
                const QString cutText = splitList[1];
                const QFileInfo fileInfo(cutText);

                if (fileInfo.exists()) {
                    const QImage image(cutText);
                    if (!image.isNull()) {
                        pixmap = QPixmap::fromImage(image);
                    } else {
                        // try to display custom image for non-image files
                        const QIcon icon = iconDB.icon(fileInfo);
                        if (!icon.isNull()) {
                            pixmap = icon.pixmap(IMAGEHEIGHT);
                            // TODO: also display text (=file path / filename?) on top
                            // https://forum.qt.io/topic/121994/adding-multi-line-multi-color-text-on-top-of-qimage
                            // TOOD: also open file in txt editor, or even better: on right-click show popup menu to either open in txt editor / open location in explorer
                        }
                    }
                }
            }
        }

        // if it is an image then display it, otherwise handle as text
        if (!pixmap.isNull()) {
            entry->setData(Qt::DecorationRole, pixmap.scaledToHeight(IMAGEHEIGHT));
        } else {
            if (clipboard_mimedata->hasHtml()) {
                // TODO: handle URL by showing a firefox button
            }

            entry->setText(clipboard->text());
            entry->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        }

        // store mime data for later retrieval
        QList<QByteArray> byteArrayList;
        foreach (QString format, clipboard_mimeformats) {
            QByteArray byteArray = clipboard_mimedata->data(format);
            byteArrayList.append(byteArray);
        }
        const QVariant variant_data = QVariant::fromValue(byteArrayList);
        entry->setData(clipboardID_formats, clipboard_mimeformats);
        entry->setData(clipboardID_data, variant_data);

        // actually insert our entry
        historyTable->setItem(0, 0, entry);

        // set timestamp for current item
        const QTime now = QTime::currentTime();
        QTableWidgetItem *timestamp = new QTableWidgetItem();
        timestamp->setText(now.toString());
        timestamp->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        timestamp->setFlags(timestamp->flags() ^ Qt::ItemIsEditable);
        historyTable->setItem(0, 1, timestamp);

        historyTable->selectRow(0);

        // enforce maximum height
        historyTable->resizeRowToContents(0);
        if (historyTable->rowHeight(0) > IMAGEHEIGHT)
            historyTable->setRowHeight(0, IMAGEHEIGHT);

        historyTable->scrollToTop();
    }
}

void Window::setNewClipboard()
{
    const QTableWidgetItem * const currentTableItem = historyTable->currentItem();

    QMimeData * const mimedata = new QMimeData();
    const QStringList formats = qvariant_cast<QStringList> (currentTableItem->data(clipboardID_formats));
    const QList<QByteArray> data = qvariant_cast<QList<QByteArray >> (currentTableItem->data(clipboardID_data));

    for (int i = 0; i < formats.size(); i++) {
        mimedata->setData(formats[i], data[i]);
    }

    clipboardUpdate = false;
    clipboard->setMimeData(mimedata, QClipboard::Clipboard);
    qDebug() << "updated clipboard!";
}

void Window::button_up_clicked()
{
    const int row = historyTable->currentRow();

    if (row > 0) {
        historyTable->selectRow(row - 1);
        setNewClipboard();
    }
}

void Window::button_down_clicked()
{
    const int row = historyTable->currentRow();

    if (row < (historyTable->rowCount() - 1)) {
        historyTable->selectRow(row + 1);
        setNewClipboard();
    }
}

void Window::button_delete_clicked()
{
    const int row = historyTable->currentRow();

    if (row > -1) {
        historyTable->removeRow(row);

        // only set new clipboard if this is not the last entry
        if (row > 0) {
            setNewClipboard();
        } else {
            clipboard->clear(QClipboard::Clipboard);
        }
    }
}

void Window::button_clear_clicked()
{
    const QMessageBox::StandardButton result = QMessageBox::question(this, tr("Clear"), tr("Clear all clipboard history?"));

    if (result == QMessageBox::Yes) {
        historyTable->clearContents();
        historyTable->setRowCount(0);
        clipboard->clear(QClipboard::Clipboard);
        clipboardUpdate = true;
    }
}

void Window::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.setValue("window_geometry", saveGeometry());
    QWidget::closeEvent(event);
    QCoreApplication::quit(); // make sure we quit, this does not happen automatically because we use window.setWindowFlags by setting Qt::Tool
}
