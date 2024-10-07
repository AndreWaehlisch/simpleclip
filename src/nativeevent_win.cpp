#include "nativeevent_win.h"

#include <QDebug>

bool nativeevent_win::nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) {
    Q_UNUSED(eventType)
    Q_UNUSED(result)

    const PMSG msg = static_cast<PMSG>(message);

    if ( msg->message == WM_HOTKEY ) {
        qDebug() << "HOT HOT HOTKEY";
    }

    return false;
}
