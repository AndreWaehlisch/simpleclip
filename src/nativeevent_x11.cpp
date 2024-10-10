#include <xcb/xcb.h>

#include "global.h"
#include "nativeevent_x11.h"

#include <QDebug>

nativeevent_x11::nativeevent_x11(Window *window) {
    this->window = window;
}

bool nativeevent_x11::nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) {
    Q_UNUSED(eventType)
    Q_UNUSED(result)
    Q_UNUSED(message)

    // TODO
    // https://xcb.freedesktop.org/tutorial/events/
    // https://github.com/Skycoder42/QHotkey/blob/master/QHotkey/qhotkey_x11.cpp

    return false;
}
