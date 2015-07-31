#include "RaytracerWindow.h"

#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QAction>

RaytracerWindow::RaytracerWindow() : QMainWindow()
{
    QAction* action = new QAction(this);
    action->setText("&Close");
    connect(action, SIGNAL(triggered()), SLOT(close()));
    menuBar()->addMenu("&File")->addAction(action);
}

RaytracerWindow::~RaytracerWindow()
{ }

#include "RaytracerWindow.moc"
