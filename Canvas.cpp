#include "Canvas.h"

#include <QTimer>

const int REFRESH_DELAY = 100;

Canvas::Canvas(QWidget* parent) : QLabel(parent),
    m_refreshTimer(new QTimer(this))
{
    this->setScaledContents(true);
}

Canvas::~Canvas()
{

}

void Canvas::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
}

#include "Canvas.moc"
