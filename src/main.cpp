#include <QCoreApplication>
#include <QApplication>
#include <QObject>
#include <QSettings>

#include "global.h"
#include "window.h"

#ifdef Q_OS_WIN
    #include <nativeevent_win.h>
    #include <qt_windows.h>

    // TODO: handle variable type for Linux
    static BOOL register_hotkey_down_ok = false;
    static BOOL register_hotkey_up_ok = false;
#endif

void cleanUp()
{
#ifdef Q_OS_WIN
    // unregister hotkeys we requested with MS Windows
    if (register_hotkey_down_ok)
        UnregisterHotKey(NULL, hotkey_down);
    if (register_hotkey_up_ok)
        UnregisterHotKey(NULL, hotkey_up);
#endif
}

// TODO: implement a simple search
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName("AndreWaehlisch");
    app.setApplicationName("simpleclip");

    const QIcon mainIcon(":/icon.ico");
    Window window;
    window.setWindowFlags(Qt::Tool); // dont show us on the taskbar
    window.setWindowIcon(mainIcon);
    window.show();

    QSettings settings;
    window.restoreGeometry(settings.value("window_geometry").toByteArray());

#ifdef Q_OS_WIN
    // install native event filter for MS Windows so we get every hotkey message (even if we are for example minimized), for details see:
    // https://doc.qt.io/qt-6/qabstractnativeeventfilter.html
    // https://forum.qt.io/topic/57372
    nativeevent_win filter = nativeevent_win(&window);
    app.installNativeEventFilter(&filter);

    register_hotkey_down_ok = RegisterHotKey(NULL, hotkey_down, MOD_WIN | MOD_ALT | MOD_NOREPEAT, 0x56); // WIN + ALT + v
    register_hotkey_up_ok = RegisterHotKey(NULL, hotkey_up, MOD_WIN | MOD_ALT | MOD_NOREPEAT, 0x43); // WIN + ALT + c
    // TODO: add win-key (without modifier/additional key) to display our window temporarily
    // perhaps with a QTimer and checking if win-key is still pressed with: https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getkeystate

    if ((!register_hotkey_down_ok) || (!register_hotkey_up_ok)) {
        qDebug() << "RegisterHotKey FAILED!";
        // TODO: handle this error (may probably happen when other program already has this hotkey registered?)
    }
#endif

    QObject::connect(&app, &QCoreApplication::aboutToQuit, cleanUp);

    return app.exec();
}
