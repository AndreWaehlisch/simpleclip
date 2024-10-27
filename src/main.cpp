#include <QCoreApplication>
#include <QApplication>
#include <QObject>
#include <QSettings>

#include "window.h"

#ifdef Q_OS_WIN
    #include <qt_windows.h>
    #include "native_win.h"

    // TODO: handle variable type for Linux
    static BOOL register_hotkey_down_ok = false;
    static BOOL register_hotkey_up_ok = false;
#else
    #include "native_x11.h"
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
    if (settings.contains("window_geometry"))
        window.restoreGeometry(settings.value("window_geometry").toByteArray());

#ifdef Q_OS_WIN
    // install native event filter for MS Windows so we get every hotkey message (even if we are for example minimized)
    nativeevent_win filter = nativeevent_win(&window);
    app.installNativeEventFilter(&filter);

    register_hotkey_down_ok = RegisterHotKey(NULL, hotkey_down, MOD_WIN | MOD_ALT | MOD_NOREPEAT, 0x56); // WIN + ALT + v
    register_hotkey_up_ok = RegisterHotKey(NULL, hotkey_up, MOD_WIN | MOD_ALT | MOD_NOREPEAT, 0x43); // WIN + ALT + c

    if ((!register_hotkey_down_ok) || (!register_hotkey_up_ok)) {
        DWORD error = GetLastError();
        qDebug() << "RegisterHotKey FAILED!" << error;
        // TODO: handle this error (may probably happen when other program already has this hotkey registered?)
    }
#endif

    QObject::connect(&app, &QCoreApplication::aboutToQuit, cleanUp);

    return app.exec();
}
