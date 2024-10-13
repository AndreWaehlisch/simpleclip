#ifndef NATIVEEVENT_WIN_H
#define NATIVEEVENT_WIN_H

#include <qglobal.h>
#include <QAbstractNativeEventFilter>
#include <QByteArray>

#include "window.h"

#ifndef Q_OS_WIN
    #error Must be on Windows at this point
#endif

class nativeevent_win : public QAbstractNativeEventFilter
{
private:
    Window *window;
public:
    nativeevent_win(Window *window);
    void forceToFront();
    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;
};

#endif // NATIVEEVENT_WIN_H
