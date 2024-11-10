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
    using namespace QNativeInterface;
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
    // install native event filter for MS Windows so we get every hotkey message (even if we are, for example, minimized)
    nativeevent_win filter = nativeevent_win(&window);
    app.installNativeEventFilter(&filter);

    register_hotkey_down_ok = RegisterHotKey(NULL, hotkey_down, MOD_WIN | MOD_ALT | MOD_NOREPEAT, 0x56); // WIN + ALT + v
    register_hotkey_up_ok = RegisterHotKey(NULL, hotkey_up, MOD_WIN | MOD_ALT | MOD_NOREPEAT, 0x43); // WIN + ALT + c

    if ((!register_hotkey_down_ok) || (!register_hotkey_up_ok)) {
        qDebug() << "RegisterHotKey FAILED!";
        // TODO: handle this error (may happen when other program already has this hotkey registered)
    }
#else
    nativeevent_x11 filter(&window);
    app.installNativeEventFilter(&filter);

    if (auto *x11Application = app.nativeInterface<QX11Application>()) {
        xcb_connection_t *connection = x11Application->connection();
        xcb_screen_t *screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;
        xcb_window_t root = screen->root;
        xcb_key_symbols_t *keysyms = xcb_key_symbols_alloc(connection);
        xcb_keycode_t keycode_down = *xcb_key_symbols_get_keycode(keysyms, hotkey_down);
        xcb_keycode_t keycode_up = *xcb_key_symbols_get_keycode(keysyms, hotkey_up);
        xcb_key_symbols_free(keysyms);

        xcb_grab_key(connection, 1, root, XCB_MOD_MASK_1 | XCB_MOD_MASK_4, keycode_down, XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC);
        xcb_grab_key(connection, 1, root, XCB_MOD_MASK_1 | XCB_MOD_MASK_4, keycode_up, XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC);
    } else {
        qDebug() << "COULD NOT GRAB X11 native interface!";
    }
#endif

    QObject::connect(&app, &QCoreApplication::aboutToQuit, cleanUp);

    return app.exec();
}
