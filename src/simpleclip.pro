QT += core widgets gui

TARGET        = simpleclip
TEMPLATE      = app

HEADERS       = window.h \
                global.h

SOURCES       = main.cpp \
                window.cpp

win32 {
    LIBS += -luser32 # required by: https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerhotkey
    HEADERS += nativeevent_win.h
    SOURCES += nativeevent_win.cpp
} else:linux {
    LIBS += -lxcb
    HEADERS += nativeevent_x11.h
    SOURCES += nativeevent_x11.cpp
} else {
    error("Only Windows and Linux (X11) are supported.")
}

RESOURCES += ../resources/resources.qrc
