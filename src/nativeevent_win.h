#ifndef NATIVEEVENT_WIN_H
#define NATIVEEVENT_WIN_H

#include <qglobal.h>
#include <QAbstractNativeEventFilter>
#include <QByteArray>

#ifndef Q_OS_WIN
#error Must be on Windows at this point
#endif

#include <qt_windows.h>

class nativeevent_win : public QAbstractNativeEventFilter
{
    public:
        bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;
};

#endif // NATIVEEVENT_WIN_H
