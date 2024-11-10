#ifndef NATIVE_WIN_H
#define NATIVE_WIN_H

#include <qglobal.h>
#include <QAbstractNativeEventFilter>
#include <QByteArray>

#include "window.h"

#ifndef Q_OS_WIN
    #error Must be on Windows at this point
#endif

// hotkey ID's, used for MS Windows RegisterHotKey api
enum HotkeyID {
    hotkey_down,
    hotkey_up,
};

class nativeevent_win : public QAbstractNativeEventFilter
{
private:
    Window *window;
public:
    nativeevent_win(Window *const window);
    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;
};

void forceToFront(Window *window);

#endif // NATIVE_WIN_H
