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
#include <QTimer>

#include "Canvas.h"

#include <Camera.hpp>
#include <Image.hpp>
#include <Raytracer.hpp>
#include <ThreadPool.hpp>

#include <QProgressBar>
#include <QStatusBar>

RaytracerWindow::RaytracerWindow(QWidget* parent) : QMainWindow(parent),
    m_canvas(new Canvas(this)),
    m_progressBar(new QProgressBar(this)),
    m_refreshTimer(new QTimer(this)),
    m_threadPool(std::make_unique<ThreadPool>())
{
    QAction* action = new QAction(this);
    action->setText("&Close");
    connect(action, SIGNAL(triggered()), SLOT(close()));
    menuBar()->addMenu("&File")->addAction(action);

    this->setCentralWidget(m_canvas);
    this->setGeometry(50, 50, 1280, 720);

    m_progressBar->setMinimumWidth(150);
    m_progressBar->setMaximumWidth(150);
    m_progressBar->setVisible(false);
    this->statusBar()->addPermanentWidget(m_progressBar);

    m_refreshTimer->setInterval(400);

    connect(this, SIGNAL(renderStart(int)), this, SLOT(renderStarted(int)));
    connect(this, SIGNAL(renderComplete(bool)), this, SLOT(renderCompleted(bool)));
    connect(this, SIGNAL(lineComplete(int)), this, SLOT(lineCompleted(int)));

    connect(m_refreshTimer, SIGNAL(timeout()), this, SLOT(refreshTimerTick()));

    //Camera camera(1280, 720, {1.4, 1.7, 5}, {-1.5, -2.0, -5}, 1.3, {0.3, 1, 0});

    //SurfaceDescription planeSurface({1, 1, 1}, 0, 0, 0, 5.0);
    //SurfaceDescription sphereSurface({0.6, 0.8, 1}, 1);

    //std::vector<std::unique_ptr<Shape>> shapes;
    //shapes.push_back(std::unique_ptr<Shape>(new Plane({0, 0, -3}, {1, 0, 0}, {0, 1, 0}, planeSurface)));
    //shapes.push_back(std::unique_ptr<Shape>(new Sphere({0, 0, 1}, {0, 1, 0}, 1.0, sphereSurface)));
    //shapes.push_back(std::unique_ptr<Shape>(new Chmutov({0, 0, 1}, {0, 1, 0}, 1.0, 0.5, sphereSurface)));

    Camera camera(1280, 720, {0, 0, -9}, {0, 0, 1}, 1, {0, 1, 0}, 256);

    SurfaceDescription redWall({1, 0, 0}, 1, 0, 0, 0);
    SurfaceDescription greenWall({0, 1, 0}, 1, 0, 0, 0);
    SurfaceDescription whiteWall({1, 1, 1}, 1, 0, 0, 0);
    SurfaceDescription lightSource({1, 1, 1}, 0, 0, 0, 6);
    SurfaceDescription reflective({1, 1, 1}, 0, 0.9, 0, 0);
    SurfaceDescription glass({1, 1, 1}, 0, 0, 1.0, 0, 1.5);

    std::vector<std::shared_ptr<Shape>> shapes;
    shapes.emplace_back(std::make_shared<Plane>(Plane({-4, 0, 0}, {0, 1, 0}, {0, 0, 1}, redWall)));
    shapes.emplace_back(std::make_shared<Plane>(Plane({+4, 0, 0}, {0, -1, 0}, {0, 0, 1}, greenWall)));
    shapes.emplace_back(std::make_shared<Plane>(Plane({0, +3, 0}, {1, 0, 0}, {0, 0, 1}, whiteWall)));
    shapes.emplace_back(std::make_shared<Plane>(Plane({0, -3, 0}, {-1, 0, 0}, {0, 0, 1}, whiteWall)));
    shapes.emplace_back(std::make_shared<Plane>(Plane({0, 0, +3}, {-1, 0, 0}, {0, 1, 0}, whiteWall)));
    shapes.emplace_back(std::make_shared<Plane>(Plane({0, 0, -10}, {1, 0, 0}, {0, 1, 0}, whiteWall)));
    shapes.emplace_back(std::make_shared<Rectangle>(Rectangle({-1, 2.99, -1}, {1, 2.99, -1}, {-1, 2.99, 1}, lightSource)));
    shapes.emplace_back(std::make_shared<Box>(Box({2, 2, 2}, {0, -1, 0}, {pi()/4, pi()/5, pi()/4}, whiteWall)));
    shapes.emplace_back(std::make_shared<Sphere>(Sphere({-2, -2, -1.5}, {0, 1, 0}, 1, reflective)));
    shapes.emplace_back(std::make_shared<Sphere>(Sphere({2, -2.1, -1.6}, {0, 1, 0}, 0.9, glass)));
    //shapes.emplace_back(std::make_shared<Sphere>(Sphere({0, 0, 0}, {0, 1, 0}, 0.5, ceiling)));
    //shapes.push_back(std::unique_ptr<Shape>(new Sphere({0, 0, 0}, {0, 1, 0}, 1.6, reflectiveSphere)));
    //shapes.push_back(std::make_shared<Shape>(new Sphere({-2, -2, 0}, {0, 1, 0}, 1.0, reflectiveSphere)));
    //shapes.push_back(std::unique_ptr<Shape>(new Chmutov({2, 0, 0}, {0, 1, 0}, 3, 1, reflectiveSphere)));

    Scene scene(camera, std::move(shapes));

    m_task = std::make_unique<TaskHandle>(std::move(scene.render(*m_threadPool)));

    m_task->setStartCallback([this](const Image<ColourRgb<float>>& result) {
        m_image = QImage(QSize(result.width(), result.height()), QImage::Format_ARGB32);
        emit renderStart(result.height());
    });

    m_task->setCompleteCallback([this](const Image<ColourRgb<float>>& result, bool success) {
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

    m_task->setProblemCallback([this](const Image<ColourRgb<float>>& image, const Problem& p) {
        auto row = *(image.begin() + p[0]);
        std::transform(row.begin(), row.end(), (QRgb*)m_image.scanLine(p[0]), [](const ColourRgb<float>& col) {
            auto temp = colour_cast<ColourRgb<std::uint8_t>>(col);
            return qRgb(temp.red(), temp.green(), temp.blue());
        });

        emit lineComplete(p[0]);
    });
}

RaytracerWindow::~RaytracerWindow()
{

}

void RaytracerWindow::closeEvent(QCloseEvent* event)
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
    m_progressBar->setVisible(false);
    m_refreshTimer->start();

    emit refreshTimerTick();
}

void RaytracerWindow::lineCompleted(int line)
{
    m_progressBar->setValue(m_progressBar->value() + 1);
}

void RaytracerWindow::refreshTimerTick()
{
    m_canvas->setPixmap(QPixmap::fromImage(m_image));
}

#include "RaytracerWindow.moc"
