#include <xcb/xcb.h>
#include <QGuiApplication>

#include "native_x11.h"

#include <QDebug>

bool nativeevent_x11::raisedWindow = false;

bool nativeevent_x11::nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result)
{
    Q_UNUSED(result)

    if (eventType == "xcb_generic_event_t") {
        const xcb_generic_event_t *msg = static_cast<xcb_generic_event_t *>(message);

        if (msg->response_type == XCB_KEY_PRESS) {
            const xcb_key_press_event_t *keyEvent = static_cast<xcb_key_press_event_t *>(message);

            if (keyEvent->detail == keyCode_down) {
                qDebug() << "Test EventFilter DOWNKKEY" << msg;
                window->button_down_clicked();
                return true;
            } else if (keyEvent->detail == keyCode_up) {
                qDebug() << "Test EventFilter UPKEY" << msg;
                window->button_up_clicked();
                return true;
            }
        }
    }

    return false;
}

// check if Win key is down and if it is force the window to the front
void forceToFront(myWindow *const window)
{
    const bool winPressed = QGuiApplication::queryKeyboardModifiers().testFlag(Qt::MetaModifier);
    const bool isForeground = window->isActiveWindow();

    if (!isForeground && winPressed) {
        nativeevent_x11::raisedWindow = true;
        window->setAttribute(Qt::WA_ShowWithoutActivating);
        window->setWindowFlags(window->windowFlags() | Qt::WindowStaysOnTopHint);
        window->show();
        qDebug() << "Raised X11 window!";
    } else if (nativeevent_x11::raisedWindow && !winPressed) {
        nativeevent_x11::raisedWindow = false;
        window->setAttribute(Qt::WA_ShowWithoutActivating, false);
        window->setWindowFlags(window->windowFlags() & ~Qt::WindowStaysOnTopHint);
        qDebug() << "UNDONE the raising of X11 window!";
    }
}
