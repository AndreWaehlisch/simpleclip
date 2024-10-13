#ifndef NATIVEEVENT_X11_H
#define NATIVEEVENT_X11_H

#include <qglobal.h>
#include <QAbstractNativeEventFilter>
#include <QByteArray>

#include "window.h"

#ifndef Q_OS_LINUX
    #error Must be on Linux at this point
#endif

class nativeevent_x11 : public QAbstractNativeEventFilter
{
private:
    Window *window;
public:
    nativeevent_x11(Window *window);
    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;
};

#endif // NATIVEEVENT_X11_H
