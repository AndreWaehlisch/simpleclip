#include <qt_windows.h>

#include "global.h"
#include "nativeevent_win.h"

#include <QDebug>

nativeevent_win::nativeevent_win(Window *window)
{
    this->window = window;
}

void nativeevent_win::forceToFront()
{
    const HWND hwnd = reinterpret_cast<HWND>(window->winId());
    const BOOL setOk = SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowpos

    qDebug() << "forced to front:" << setOk;
}

bool nativeevent_win::nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result)
{
    Q_UNUSED(eventType)
    Q_UNUSED(result)

    const PMSG msg = static_cast<PMSG>(message);

    if (msg->message == WM_HOTKEY) {
        switch (msg->wParam) {
        case hotkey_down:
            qDebug() << "DOWN Key";
            window->button_down_clicked();
            return true;
        case hotkey_up:
            qDebug() << "UP Key";
            window->button_up_clicked();
            return true;
        case hotkey_win:
            qDebug() << "WIN Key";
            // TODO
            forceToFront();
        default:
            // should never happen
            break;
        }
    }

    return false;
}
