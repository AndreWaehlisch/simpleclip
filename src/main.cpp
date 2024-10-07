#include <QCoreApplication>
#include <QApplication>
#include <QObject>

#include "window.h"

#ifdef Q_OS_WIN
#include <nativeevent_win.h>
#endif

void unregisterHotkey(){
    qDebug() << "TESTER";
    // TODO: https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-unregisterhotkey
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setOrganizationName("AndreWaehlisch");
    app.setApplicationName("simpleclip");

    nativeevent_win *filter = new nativeevent_win;
    app.installNativeEventFilter(filter);

    //const QIcon mainIcon(":/icon.ico");
    Window window;
    window.setWindowFlags(Qt::Dialog);
    //window.setWindowIcon(mainIcon);
    window.show();

    const BOOL register_hotkey_ok = RegisterHotKey(NULL, 1, MOD_ALT | MOD_NOREPEAT, 0x42);
    qDebug() << "register b ok:" << register_hotkey_ok;
    // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerhotkey
    // https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-hotkey
    // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getmessage
    // https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-msg

    // https://doc.qt.io/qt-6/qabstractnativeeventfilter.html#nativeEventFilter
    // https://doc.qt.io/qt-6/qcoreapplication.html#installNativeEventFilter

	// https://forum.qt.io/post/285795
    // https://github.com/Skycoder42/QHotkey/blob/master/QHotkey/qhotkey_win.cpp

    QObject::connect(&app, &QCoreApplication::aboutToQuit, unregisterHotkey);

    return app.exec();
}
