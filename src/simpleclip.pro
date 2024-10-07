QT += core widgets gui

TARGET        = simpleclip
TEMPLATE      = app

HEADERS       = window.h \
                nativeevent_win.h

SOURCES       = main.cpp \
                nativeevent_win.cpp \
                window.cpp

win32 {
    # required by: https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerhotkey
    LIBS += -luser32
}

#RESOURCES += ../resources/resources.qrc
