#include <xcb/xcb.h>

#include "native_x11.h"

#include <QDebug>

nativeevent_x11::nativeevent_x11(Window *window)
{
    this->window = window;
}

bool nativeevent_x11::nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result)
{
    Q_UNUSED(eventType)
    Q_UNUSED(result)

    const xcb_generic_event_t *msg = static_cast<xcb_generic_event_t *>(message);

    if (msg->response_type == XCB_KEY_PRESS) {
        const xcb_key_press_event_t keyEvent = *static_cast<xcb_key_press_event_t *>(message);

        switch (keyEvent.detail) {
hotkey_up:
            qDebug() << "Test EventFilter UPKEY" << msg;
            break;
hotkey_down:
            qDebug() << "Test EventFilter DOWNKEY" << msg;
            break;
        default:
            // should never happen
            qDebug() << "PANIC";
            break;
        }
    }

    return false;

    // TODO
    // https://xcb.freedesktop.org/tutorial/events/
    // https://github.com/Skycoder42/QHotkey/blob/master/QHotkey/qhotkey_x11.cpp
}


// check if Win key is down and if it is force the window to the front
void forceToFront(Window *const window)
{
    Q_UNUSED(window)
    // TODO
}
