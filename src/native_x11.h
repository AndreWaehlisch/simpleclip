#ifndef NATIVE_X11_H
#define NATIVE_X11_H

#include <qglobal.h>
#include <QAbstractNativeEventFilter>
#include <QByteArray>

#include "window.h"

#ifndef Q_OS_LINUX
    #error Must be on Linux at this point
#endif

#include <xcb/xproto.h>
#include <xcb/xcb_keysyms.h>

// hotkey ID's, see "xmodmap -pke"
enum HotkeyID {
    hotkey_down = XK_v,
    hotkey_up = XK_c,
};


class nativeevent_x11 : public QAbstractNativeEventFilter
{
private:
    Window *window;
public:
    nativeevent_x11(Window *const window);
    bool nativeEventFilter(const QByteArray &, void *, qintptr *) override;
};

void forceToFront(Window *window);

#endif // NATIVE_X11_H
