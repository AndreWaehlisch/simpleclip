#ifndef NATIVE_X11_H
#define NATIVE_X11_H

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

void forceToFront(Window *window);

#endif // NATIVE_X11_H
