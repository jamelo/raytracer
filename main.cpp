#include "config.h"

#include <QApplication>

#include "RaytracerWindow.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    RaytracerWindow mainWindow;
    mainWindow.show();
    app.exec();

    return 0;
}
