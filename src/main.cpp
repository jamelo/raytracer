#include "config.h"
#include "RaytracerWindow.h"

#include <iostream>
#include <QApplication>

#include <Exceptions.hpp>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    RaytracerWindow mainWindow;
    mainWindow.show();
    app.exec();

    return 0;
}
