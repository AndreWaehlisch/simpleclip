QT += core widgets gui

TARGET        = simpleclip
TEMPLATE      = app

HEADERS       = window.h \
                global.h

SOURCES       = main.cpp \
                window.cpp

win32 {
    # required by: https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerhotkey
    LIBS += -luser32
    HEADERS += nativeevent_win.h
    SOURCES += nativeevent_win.cpp
}

unix {
    LIBS += -lxcb
    HEADERS += nativeevent_x11.h
    SOURCES += nativeevent_x11.cpp
}

#RESOURCES += ../resources/resources.qrc
