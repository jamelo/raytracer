#include "RaytracerWindow.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>

#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QProgressBar>
#include <QTimer>
#include <QStatusBar>

#include "Canvas.h"

//TODO: find a more appropriate place to define pi

#include <SceneLoaderJson.hpp>
#include <builders/SceneBuilder.hpp>
#include <threading/ThreadPool.hpp>
#include <graphics/Image.hpp>
#include <Camera.hpp>
#include <Raytracer.hpp>
#include <Shapes.hpp>

RaytracerWindow::RaytracerWindow(QWidget* parent) : QMainWindow(parent),
    m_canvas(new Canvas(this)),
    m_progressBar(new QProgressBar(this)),
    m_refreshTimer(new QTimer(this)),
    m_threadPool(std::make_unique<threading::ThreadPool>())
{
    QAction* action = new QAction(this);
    action->setText("&Close");
    connect(action, SIGNAL(triggered()), SLOT(close()));
    menuBar()->addMenu("&File")->addAction(action);

    this->setCentralWidget(m_canvas);
    this->setGeometry(50, 50, 1920, 1080);

    m_progressBar->setMinimumWidth(150);
    m_progressBar->setMaximumWidth(150);
    m_progressBar->setVisible(false);
    this->statusBar()->addPermanentWidget(m_progressBar);

    m_refreshTimer->setInterval(400);

    connect(this, SIGNAL(renderStart(int)), this, SLOT(renderStarted(int)));
    connect(this, SIGNAL(renderComplete(bool)), this, SLOT(renderCompleted(bool)));
    connect(this, SIGNAL(lineComplete(int)), this, SLOT(lineCompleted(int)));

    connect(m_refreshTimer, SIGNAL(timeout()), this, SLOT(refreshTimerTick()));

    SceneLoaderJson loader;

    builders::BuilderArgs args = loader.load("../../scenes/cornell-box.json");
    auto scene = builders::SceneBuilder().build(args);

    m_task = std::make_unique<threading::TaskHandle>(std::move(::render(*m_threadPool, scene)));

    m_task->setStartCallback([this](const graphics::Image<graphics::ColourRgb<float>>& result) {
        m_image = QImage(QSize(result.width(), result.height()), QImage::Format_ARGB32);
        emit renderStart(result.height());
    });

    m_task->setCompleteCallback([this](const graphics::Image<graphics::ColourRgb<float>>& result, bool success) {
        char filename[256];
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::strftime(filename, sizeof(filename), "render_%Y%m%d_%H%M%S.png", std::localtime(&in_time_t));

        if (success) {
            std::cout << "Render complete." << std::endl;
            m_autoTimer.stop();
            m_autoTimer.report();
            std::cout << std::endl;
            result.save(filename);
        } else {
            std::cout << "Render cancelled." << std::endl;
        }

        std::cout << "Render saved." << std::endl;

        emit renderComplete(success);
    });

    m_task->setProblemCallback([this](const graphics::Image<graphics::ColourRgb<float>>& image, const threading::Problem& p) {
        auto row = *(image.begin() + p[0]);
        std::transform(row.begin(), row.end(), (QRgb*)m_image.scanLine(p[0]), [](const graphics::ColourRgb<float>& col) {
            auto temp = graphics::colour_cast<graphics::ColourRgb<std::uint8_t>>(col);
            return qRgb(temp.red(), temp.green(), temp.blue());
        });

        emit lineComplete(p[0]);
    });
}

RaytracerWindow::~RaytracerWindow()
{

}

void RaytracerWindow::closeEvent(QCloseEvent*)
{
    m_task->cancel();
    m_threadPool->wait();
}

void RaytracerWindow::renderStarted(int size)
{
    m_progressBar->setVisible(true);
    m_progressBar->setValue(1);
    m_progressBar->setMaximum(size);
    m_refreshTimer->start();
}

void RaytracerWindow::renderCompleted(bool success)
{
    (void)success;
    m_progressBar->setVisible(false);
    m_refreshTimer->start();

    emit refreshTimerTick();
}

void RaytracerWindow::lineCompleted(int line)
{
    (void)line;
    m_progressBar->setValue(m_progressBar->value() + 1);
}

void RaytracerWindow::refreshTimerTick()
{
    m_canvas->setPixmap(QPixmap::fromImage(m_image));
}

#include "RaytracerWindow.moc"
