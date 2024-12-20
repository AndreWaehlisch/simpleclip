QT += core widgets gui

TARGET        = simpleclip
TEMPLATE      = app

HEADERS       = myWindow.h

SOURCES       = main.cpp \
                myWindow.cpp

win32 {
    LIBS += -luser32 # required by: https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerhotkey
    HEADERS += native_win.h
    SOURCES += native_win.cpp
} else:linux {
    LIBS += -lX11 # required by: https://linux.die.net/man/3/xgrabkey
    HEADERS += native_x11.h
    SOURCES += native_x11.cpp
} else {
    error("Currenlty only Windows and Linux (X11) are supported, but you are running something else.")
}

RESOURCES += ../resources/resources.qrc
CONFIG += c++11
