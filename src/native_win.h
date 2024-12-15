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
    nativeevent_win(Window *const);
    bool nativeEventFilter(const QByteArray &, void *, qintptr *) override;
    static bool raisedWindow;
};

void forceToFront(Window *);

#endif // NATIVE_WIN_H
