#include <QCoreApplication>
#include <QApplication>

#include "window.h"

#include <qt_windows.h>
#include <QAbstractNativeEventFilter>

void tester(){
    qDebug() << "TESTER";
}

int main(int argc, char *argv[]) {
    QCoreApplication::setOrganizationName("AndreWaehlisch");
    QCoreApplication::setApplicationName("simpleclip");
    const QApplication app(argc, argv);
    //const QIcon mainIcon(":/icon.ico");
    Window window;
    window.setWindowFlags(Qt::Dialog);
    //window.setWindowIcon(mainIcon);
    window.show();

    const BOOL register_hotkey_ok = RegisterHotKey(NULL, 1, MOD_ALT, 0x42);

    return app.exec();
}
