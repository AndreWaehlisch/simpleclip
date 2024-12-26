#ifndef NATIVE_X11_H
#define NATIVE_X11_H

#include <qglobal.h>
#include <QAbstractNativeEventFilter>
#include <QByteArray>

#ifndef Q_OS_LINUX
    #error Must be on Linux at this point
#endif

#include <X11/Xlib.h>
#include <X11/keysym.h>

#include "myWindow.h"

// hotkey ID's, see "xmodmap -pke"
enum HotkeyID {
    hotkey_down = XK_v,
    hotkey_up = XK_c,
};


class nativeevent_x11 : public QAbstractNativeEventFilter
{
private:
    myWindow *const window;

public:
    nativeevent_x11(myWindow *const window) : window(window) {}

    bool nativeEventFilter(const QByteArray &, void *, qintptr *) override;
    static bool raisedWindow;
    KeyCode keyCode_down;
    KeyCode keyCode_up;

};

void forceToFront(myWindow *window);

#endif // NATIVE_X11_H
