#include <QCoreApplication>
#include <QApplication>
#include <QObject>
#include <QSettings>

#include "global.h"
#include "window.h"

#ifdef Q_OS_WIN
#include <nativeevent_win.h>
#include <qt_windows.h>
#endif

void cleanUp(){
    #ifdef Q_OS_WIN
        // unregister all hotkeys we requested with MS Windows
        for ( int i_hotkey = hotkey_down; i_hotkey != hotkey_LAST; i_hotkey++ ) {
            UnregisterHotKey(NULL, i_hotkey);
        }
    #endif
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setOrganizationName("AndreWaehlisch");
    app.setApplicationName("simpleclip");

    Window window;
    //const QIcon mainIcon(":/icon.ico");
    window.setWindowFlags(Qt::Dialog);
    //window.setWindowIcon(mainIcon);
    window.show();

    QSettings settings;
    window.restoreGeometry(settings.value("window_geometry").toByteArray());

    #ifdef Q_OS_WIN
        // install native event filter for MS Windows so we get every hotkey message (even if we are for example minimized), for details see:
        // https://doc.qt.io/qt-6/qabstractnativeeventfilter.html
        // https://forum.qt.io/topic/57372
        nativeevent_win filter = nativeevent_win(&window);
        app.installNativeEventFilter(&filter);

        const BOOL register_hotkey_down_ok = RegisterHotKey(NULL, hotkey_down, MOD_WIN | MOD_ALT | MOD_NOREPEAT, 0x56); // WIN + ALT + v
        const BOOL register_hotkey_up_ok = RegisterHotKey(NULL, hotkey_up, MOD_WIN | MOD_ALT | MOD_NOREPEAT, 0x43); // WIN + ALT + c
        // TODO: add win-key (without modifier/additional key) to display our window temporarily
        // perhaps with a QTimer and checking if win-key is still pressed with: https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getkeystate

        if ( (!register_hotkey_down_ok) || (!register_hotkey_up_ok)) {
            qDebug() << "RegisterHotKey FAILED!";
            // TODO: handle this error (may probably happen when other program already has this hotkey registered?!)
            app.quit();
        }
    #endif

    QObject::connect(&app, &QCoreApplication::aboutToQuit, cleanUp);

    return app.exec();
}
