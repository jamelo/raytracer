#ifndef RaytracerWindow_H
#define RaytracerWindow_H

#include <QImage>
#include <QMainWindow>

#include <boost/timer/timer.hpp>

#include <threading/ThreadPool.hpp>

class Canvas;
class QProgressBar;
class QTimer;

class RaytracerWindow : public QMainWindow
{
    Q_OBJECT

signals:
    void renderStart(int size);
    void renderComplete(bool success);
    void lineComplete(int line);

private slots:
    void renderStarted(int size);
    void renderCompleted(bool success);
    void lineCompleted(int line);
    void refreshTimerTick();

public:
    RaytracerWindow(QWidget* parent = nullptr);

    virtual ~RaytracerWindow();

protected:
    virtual void closeEvent(QCloseEvent* event);

private:
    Canvas* m_canvas;
    QProgressBar* m_progressBar;
    QTimer* m_refreshTimer;
    QImage m_image;
    std::unique_ptr<threading::ThreadPool> m_threadPool;
    boost::timer::auto_cpu_timer m_autoTimer;
    std::unique_ptr<threading::TaskHandle> m_task;

};

#endif
