#ifndef Canvas_H
#define Canvas_H

#include <QImage>
#include <QLabel>

class Canvas : public QLabel
{
    Q_OBJECT

private:
    QTimer* m_refreshTimer;
    QImage m_image;

public:
    Canvas(QWidget* parent);
    virtual ~Canvas();

protected:
    virtual void resizeEvent(QResizeEvent* event);
};

#endif // Canvas_H
