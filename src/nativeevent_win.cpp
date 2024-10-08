#include <qt_windows.h>

#include "global.h"
#include "nativeevent_win.h"
#include "window.h"

#include <QDebug>

nativeevent_win::nativeevent_win(Window *window) {
    this->window = window;
}

bool nativeevent_win::nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) {
    Q_UNUSED(eventType)
    Q_UNUSED(result)

    const PMSG msg = static_cast<PMSG>(message);

    if ( msg->message == WM_HOTKEY ) {
        switch (msg->wParam) {
            case hotkey_down:
                qDebug() << "DOWN Key";
                window->button_down_clicked();
                break;
            case hotkey_up:
                qDebug() << "UP Key";
                window->button_up_clicked();
                break;
            case hotkey_win:
                qDebug() << "WIN Key";
                // TODO
            default:
                // should never happen
                break;
        }
    }

    return false;
}
