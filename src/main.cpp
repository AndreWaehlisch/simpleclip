#include <QCoreApplication>
#include <QApplication>
#include <QObject>
#include <QSettings>
#include <QProcessEnvironment>
#include <QTimer>

#include "myWindow.h"

#ifdef Q_OS_WIN
#include <qt_windows.h>
#include "native_win.h"

static BOOL register_hotkey_down_ok = false;
static BOOL register_hotkey_up_ok = false;

void cleanUp()
{
    // unregister hotkeys we requested with MS Windows
    if (register_hotkey_down_ok) {
        UnregisterHotKey(NULL, hotkey_down);
    }
    if (register_hotkey_up_ok) {
        UnregisterHotKey(NULL, hotkey_up);
    }
}
#else
#include "native_x11.h"
using namespace QNativeInterface;

void cleanUp()
{
    //TODO
    //if (register_hotkey_down_ok)
    //    XUngrabKey()
}
#endif

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName("AndreWaehlisch");
    app.setApplicationName("simpleclip");

    const QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    const bool mod_alt = (environment.value("SIMPLECAL_MODALT", "1") == "1"); // set to anything else than "1" to disable using alt

    const QIcon mainIcon(":/icon.ico");
    myWindow window;
    window.setWindowFlags(Qt::Tool); // dont show us on the taskbar
    window.setWindowIcon(mainIcon);
    window.show();

    // restore window position (with a delay to make sure we really restore on the correct monitor)
    auto restoreWindow = [&window]() {
        const QSettings settings;

        if (settings.contains("window_geometry")) {
            window.restoreGeometry(settings.value("window_geometry").toByteArray());
        }
    };

    QTimer restoreTimer1, restoreTimer2;
    restoreTimer1.setSingleShot(true);
    restoreTimer2.setSingleShot(true);
    QObject::connect(&restoreTimer1, &QTimer::timeout, &window, restoreWindow);
    QObject::connect(&restoreTimer2, &QTimer::timeout, &window, restoreWindow);
    restoreTimer1.start(2);
    restoreTimer2.start(500);

// install native event filter so we get every hotkey message (even if we are, for example, minimized)
#ifdef Q_OS_WIN
    nativeevent_win filter = nativeevent_win(&window);
    app.installNativeEventFilter(&filter);

    UINT modifiers = MOD_WIN;

    if (mod_alt) {
        modifiers |= MOD_ALT;
    }

    register_hotkey_down_ok = RegisterHotKey(NULL, hotkey_down, modifiers, 0x56); // WIN + ALT + v
    register_hotkey_up_ok = RegisterHotKey(NULL, hotkey_up, modifiers, 0x43); // WIN + ALT + c

    if ((!register_hotkey_down_ok) || (!register_hotkey_up_ok)) {
        qDebug() << "RegisterHotKey FAILED!";
        // TODO: handle this error? (may happen when other program already has this hotkey registered)
    }
#else
    nativeevent_x11 filter(&window);
    app.installNativeEventFilter(&filter);

    if (const QX11Application *x11Application = app.nativeInterface<QX11Application>()) {
        Display *x11Display = x11Application->display();

        filter.keyCode_down = XKeysymToKeycode(x11Display, hotkey_down);
        filter.keyCode_up = XKeysymToKeycode(x11Display, hotkey_up);

        const Window root = DefaultRootWindow(x11Display);

        unsigned int modifiers = Mod4Mask;

        if (mod_alt)
            modifiers |= Mod1Mask;

        XGrabKey(x11Display, filter.keyCode_down, modifiers, root, True, GrabModeAsync, GrabModeAsync);
        XGrabKey(x11Display, filter.keyCode_up, modifiers, root, True, GrabModeAsync, GrabModeAsync);
    } else {
        qDebug() << "COULD NOT GRAB X11 native interface!";
    }
#endif

    QObject::connect(&app, &QCoreApplication::aboutToQuit, cleanUp);

    return app.exec();
}
