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
#include <QTextStream>

#include "window.h"

#ifdef Q_OS_WIN
    #include "native_win.h"
#else
    #include "native_x11.h"
#endif

#define WINDOW_HEIGHT 500 // total height of window (in px)
#define WINDOW_WIDTH 400 // total width of window
#define TABLE_WIDTH1 250 // width of first column
#define TABLE_WIDTH2 (WINDOW_WIDTH - TABLE_WIDTH1) // width of second column
#define IMAGEHEIGHT 120 // default height of images; also the maximum height of each row
#define MAXROWS 55 // maximum number of rows our QTableWidget will grow to

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

    historyTable = new QTableWidget(0, 2, this);
    historyTable->setSortingEnabled(false);
    historyTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    historyTable->setSelectionMode(QAbstractItemView::SingleSelection);
    const QStringList labels = { tr("Content"), tr("Timestamp") };
    historyTable->setHorizontalHeaderLabels(labels);
    historyTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    historyTable->horizontalHeader()->resizeSection(0, TABLE_WIDTH1);
    historyTable->horizontalHeader()->resizeSection(1, TABLE_WIDTH2);

    trayMenu = new QMenu(this);
    trayMenu->addAction(windowTitle())->setDisabled(true); // first (disabled) action is a header (=our name)
    trayMenu->addAction(tr("Clear"), this, &Window::button_clear_clicked);
    trayMenu->addSeparator();

    trayMenu->addAction("");
    trayMenu->addAction("")->setDisabled(true);
    trayMenu->addAction("");

    trayMenu->addSeparator();
    trayMenu->addAction(tr("Exit"), this, &Window::close);

    tray = new QSystemTrayIcon(mainIcon, this);
    connect(tray, &QSystemTrayIcon::activated, this, &Window::tray_clicked);
    tray->setContextMenu(trayMenu);
    tray->show();

    button_up = new QPushButton("^", this);
    button_down = new QPushButton("v", this);
    button_delete = new QPushButton("x", this);
    button_clear = new QPushButton(tr("clear"), this);

    connect(button_up, &QAbstractButton::clicked, this, &Window::button_up_clicked);
    connect(button_down, &QAbstractButton::clicked, this, &Window::button_down_clicked);
    connect(button_delete, &QAbstractButton::clicked, this, &Window::button_delete_clicked);
    connect(button_clear, &QAbstractButton::clicked, this, &Window::button_clear_clicked);

    QGridLayout *mainLayout = new QGridLayout(this);

    mainLayout->addWidget(historyTable, 0, 0, 1, 4);
    mainLayout->addWidget(button_up, 1, 0);
    mainLayout->addWidget(button_down, 1, 1);
    mainLayout->addWidget(button_delete, 1, 2);
    mainLayout->addWidget(button_clear, 1, 3);

    setLayout(mainLayout);

    clipboard = QGuiApplication::clipboard();
    clipboard_updated();
    connect(clipboard, &QClipboard::dataChanged, this, &Window::clipboard_updated);

    connect(&myTimer, &QTimer::timeout, this, &Window::myTimerEvent);
    myTimer.setInterval(200);
    myTimer.start();
}

inline void Window::myTimerEvent()
{
    forceToFront(this);
}

QString Window::trimText(const QString fullStr)
{
    QString result = fullStr.simplified();

    if (result.length() > 30) {
        result = result.left(27) + "...";
    }

    return result;
}

void Window::tray_clicked(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger) {
        activateWindow();
    } else if (reason == QSystemTrayIcon::Context) {
        qDebug() << "right-click on tray";
        const int row = historyTable->currentRow();
        const int numRows = historyTable->rowCount();
        QList<QAction *> actions = trayMenu->actions();

        QAction *action1 = actions[3];
        QAction *action2 = actions[4];
        QAction *action3 = actions[5];

        qDebug() << "row:" << row;

        if (numRows == 0) {
            action1->setVisible(false);
            action2->setVisible(false);
            action3->setVisible(false);
        } else if (numRows == 1) {
            action1->setVisible(true);
            action2->setVisible(false);
            action3->setVisible(false);

            QFont font1 = action1->font();
            font1.setBold(true);
            action1->setFont(font1);
            action1->setText(trimText(historyTable->item(row, 0)->text()));
            action1->setDisabled(true);
        } else if (numRows == 2) {
            action1->setVisible(true);
            action2->setVisible(true);
            action3->setVisible(false);

            if (row == 0) {
                QFont font1 = action1->font();
                font1.setBold(true);
                action1->setFont(font1);
                action1->setText(trimText(historyTable->item(row, 0)->text()));
                action1->setDisabled(true);

                QFont font2 = action2->font();
                font2.setBold(false);
                action2->setFont(font2);
                action2->setText(trimText(historyTable->item(row + 1, 0)->text()));
                action2->setDisabled(false);
                disconnect(action2, &QAction::triggered, nullptr, nullptr);
                connect(action2, &QAction::triggered, this, &Window::button_down_clicked);
            } else {
                QFont font1 = action1->font();
                font1.setBold(false);
                action1->setFont(font1);
                action1->setText(trimText(historyTable->item(row - 1, 0)->text()));
                action1->setDisabled(false);
                disconnect(action1, &QAction::triggered, nullptr, nullptr);
                connect(action1, &QAction::triggered, this, &Window::button_up_clicked);

                QFont font2 = action2->font();
                font2.setBold(true);
                action2->setFont(font2);
                action2->setText(trimText(historyTable->item(row, 0)->text()));
                action2->setDisabled(true);
            }
        } else {
            action1->setVisible(true);
            action2->setVisible(true);
            action3->setVisible(true);

            if (row == 0) {
                QFont font1 = action1->font();
                font1.setBold(true);
                action1->setFont(font1);
                action1->setText(trimText(historyTable->item(row, 0)->text()));
                action1->setDisabled(true);

                QFont font2 = action2->font();
                font2.setBold(false);
                action2->setFont(font2);
                action2->setText(trimText(historyTable->item(row + 1, 0)->text()));
                action2->setDisabled(false);
                disconnect(action2, &QAction::triggered, nullptr, nullptr);
                connect(action2, &QAction::triggered, this, &Window::button_down_clicked);

                action3->setVisible(false);
            } else if ((row > 0) && (row + 1 < numRows)) {
                QFont font1 = action1->font();
                font1.setBold(false);
                action1->setFont(font1);
                action1->setText(trimText(historyTable->item(row - 1, 0)->text()));
                action1->setDisabled(false);
                disconnect(action1, &QAction::triggered, nullptr, nullptr);
                connect(action1, &QAction::triggered, this, &Window::button_up_clicked);

                QFont font2 = action2->font();
                font2.setBold(true);
                action2->setFont(font2);
                action2->setText(trimText(historyTable->item(row, 0)->text()));
                action2->setDisabled(true);

                QFont font3 = action3->font();
                font3.setBold(false);
                action3->setFont(font3);
                action3->setText(trimText(historyTable->item(row + 1, 0)->text()));
                action3->setDisabled(false);
                disconnect(action3, &QAction::triggered, nullptr, nullptr);
                connect(action3, &QAction::triggered, this, &Window::button_down_clicked);
            } else {
                QFont font1 = action1->font();
                font1.setBold(false);
                action1->setFont(font1);
                action1->setText(trimText(historyTable->item(row - 1, 0)->text()));
                action1->setDisabled(false);
                disconnect(action1, &QAction::triggered, nullptr, nullptr);
                connect(action1, &QAction::triggered, this, &Window::button_up_clicked);

                QFont font2 = action2->font();
                font2.setBold(true);
                action2->setFont(font2);
                action2->setText(trimText(historyTable->item(row, 0)->text()));
                action2->setDisabled(true);

                action3->setVisible(false);
            }
        }
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
        return; //TODO: if last was not empty, but the system cleared the clipboard (it is now empty), should we display that?

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


        QPixmap pixmap; // set to an image when we find one (otherwise this remains empty)
        QString fileLabel; // text displayed next to an item if it has an icon (i.e. for files)
        QString filesTooltip; // optional tooltip with list of file names
        QList<QString> matchesList;

        // first, check if image
        if (clipboard_mimedata->hasImage()) {
            const QImage image = clipboard->image(QClipboard::Clipboard);
            pixmap = QPixmap::fromImage(image);
        } else if (clipboard_mimedata->hasText()) {
            const QString clipboardtext = clipboard->text();
            QRegularExpressionMatchIterator matches = regex.globalMatch(clipboardtext);

            while (matches.hasNext()) {
                matchesList.append(matches.next().captured(1));

                QTextStream(&filesTooltip) << matchesList.last();

                if (matches.hasNext())
                    QTextStream(&filesTooltip) << "\n";
            }

            const int numMatches = matchesList.length();

            if (numMatches > 1) {
                // handle a list of files in the format "file:///C:/....file:///C:/...."
                pixmap = foldersPixmap;
                QTextStream(&fileLabel) << "(" << numMatches << " files)";
            } else if (numMatches == 1) {
                // handle (single) files in the format "file:///C:/...."
                const QString cutText = matchesList[0];
                const QFileInfo fileInfo(cutText);

                if (fileInfo.exists()) {
                    // TOOD: open file in txt editor, or even better: on right-click show popup menu to either open in txt editor / open location in explorer
                    const QImage image(cutText);

                    if (!image.isNull()) {
                        pixmap = QPixmap::fromImage(image);
                    } else {
                        // try to display custom image for non-image files
                        const QIcon icon = iconDB.icon(fileInfo);

                        if (!icon.isNull()) {
                            pixmap = icon.pixmap(IMAGEHEIGHT);
                        }
                    }

                    if (!pixmap.isNull())
                        fileLabel = fileInfo.fileName();
                }
            }
        }

        // if it is an image then display it, otherwise handle as text
        if (!pixmap.isNull()) {
            entry->setData(Qt::DecorationRole, pixmap.scaledToHeight(IMAGEHEIGHT));

            if (!fileLabel.isEmpty())
                entry->setData(Qt::DisplayRole, fileLabel);

            if (!filesTooltip.isEmpty())
                entry->setData(Qt::ToolTipRole, filesTooltip);
        } else {
            if (clipboard_mimedata->hasHtml()) {
                // TODO: handle URL by showing a firefox button or similiar
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
    const int numRows = historyTable->rowCount();

    if (numRows > 0) {
        const QMessageBox::StandardButton result = QMessageBox::question(this, tr("Clear"), tr("Clear all clipboard history?"));

        if (result == QMessageBox::Yes) {
            historyTable->clearContents();
            historyTable->setRowCount(0);
            clipboard->clear(QClipboard::Clipboard);
            clipboardUpdate = true;
        }
    }
}

void Window::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.setValue("window_geometry", saveGeometry());
    QWidget::closeEvent(event);
    QCoreApplication::quit(); // make sure we quit, this does not happen automatically because we use window.setWindowFlags by setting Qt::Tool
}
