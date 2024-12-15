#include <qt_windows.h>

#include "native_win.h"

#include <QDebug>

bool nativeevent_win::raisedWindow = false;

nativeevent_win::nativeevent_win(Window *window)
{
    this->window = window;
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
        }
    }

    return false;
}

// check if Win key is down and if it is force the window to the front
void forceToFront(Window *const window)
{
    const bool winPressed = QGuiApplication::queryKeyboardModifiers().testFlag(Qt::MetaModifier);
    const HWND foreGroundWindowID = GetForegroundWindow();
    const HWND hwnd = reinterpret_cast<HWND>(window->winId());
    const bool isForeground = (foreGroundWindowID == hwnd);

    if (!isForeground && winPressed) {
        nativeevent_win::raisedWindow = true;
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
        SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
        qDebug() << "Raised window";
    } else if (nativeevent_win::raisedWindow && !winPressed) {
        SetWindowPos(hwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        nativeevent_win::raisedWindow = false;
        qDebug() << "Lowered window";
    }
}
