#include <QMouseEvent>
#include <QDateTime>
#include <QPoint>
#include <QDebug>
#include "tangram.h"
#include "mapwidget.h"
#include "platform_qt.h"

using namespace Tangram;
Map* map = NULL;

static float Max_Zoom_Level = 19.0;
static float Min_Zoom_Level = 4.0;

MapWidget::MapWidget(QWidget *parent):
    QOpenGLWidget(parent),
    m_map(NULL),
    m_gestureInprogress(false)
{
    QSurfaceFormat fmt = this->format();
    int smplers = fmt.samples();
    qWarning()<<"smplers"<< smplers;
    fmt.setSamples(32);
    this->setFormat(fmt);
}

MapWidget::~MapWidget()
{

}

void MapWidget::initialize(const QString &scene)
{
    m_map = new Tangram::Map(std::make_shared<Platform_Qt>(*this));
    m_map->loadSceneAsync(scene.toStdString().c_str(), false);
    m_map->setPosition(117.13788604736328, 39.24948501586914);
    m_map->setZoom(16);
}

Map *MapWidget::getMap()
{
    return m_map;
}

void MapWidget::initializeGL()
{
    glClearColor(0, 0, 0, 1);
    m_lastRenderTime = QDateTime::currentMSecsSinceEpoch();
    m_map->setupGL();
    glEnable(GL_MULTISAMPLE);
}

void MapWidget::paintGL()
{
    qint64 curr = QDateTime::currentMSecsSinceEpoch();
    m_map->update(curr - m_lastRenderTime);
    m_map->render();
    m_lastRenderTime = curr;
}

void MapWidget::resizeGL(int width, int height)
{
    m_map->resize(width, height);
}

void MapWidget::mousePressEvent(QMouseEvent *event)
{
    m_gestureStartPoint = event->pos();
    m_gesturePreviousPoint = event->pos();

    switch (event->button()) {
        case Qt::MouseButton::LeftButton: {
            if(m_gestureType == GESTURE_NONE){
                m_gestureType = GESTURE_PAN;
            }
            m_gestureInprogress = true;
            break;
        }
        default:
            break;
    }
}

void MapWidget::mouseReleaseEvent(QMouseEvent */*event*/)
{
    m_gestureInprogress = false;
}

#define MY_PI 3.141592f
#define toRadians(x) (x)*MY_PI/180.f
#define toDegree(x) (x)*180.f/MY_PI

static float getTilt(QPoint p0, QPoint p1){
    return p1.y() - p0.x();
}

static float getRotation(QPoint p0, QPoint p1, QPoint center){
    return QLineF(p1, center).angle() - QLineF(p0, center).angle();
}

void MapWidget::mouseMoveEvent(QMouseEvent *event)
{
    auto pos = event->pos();
    if (m_gestureInprogress) {
        switch(m_gestureType) {
        case GESTURE_TAP:
            break;
        case GESTURE_PAN: {
            m_map->handlePanGesture(m_gestureStartPoint.x(), m_gestureStartPoint.y(), pos.x(), pos.y());
            m_gestureStartPoint = pos;
        }
        break;
        case GESTURE_SHOVE:{
            float currentTilt = toDegree(m_map->getTilt());
            float newTilt =  currentTilt + (pos.y() - m_gesturePreviousPoint.y());
            if(newTilt>60){
               newTilt = 60;
            } else if(newTilt<0) {
                newTilt = 0;
            }
            m_map->setTilt(toRadians(newTilt));
        }
        break;
        case GESTURE_ROTATE:{
            float currentRotation = m_map->getRotation();
            m_map->setRotation(currentRotation +
                               toRadians(getRotation(pos, m_gesturePreviousPoint, rect().center())));
        }
        break;
        default:
            break;
        }
    }
    m_gesturePreviousPoint = pos;
    update();
}

void MapWidget::wheelEvent(QWheelEvent *event)
{
    float zoom = m_map->getZoom();
    if(event->delta() > 0){
        zoom += 0.5;
    }else{
        zoom -= 0.5;
    }

    if (zoom > Max_Zoom_Level) {
        zoom = Max_Zoom_Level;
    }
    if (zoom < Min_Zoom_Level) {
        zoom = Min_Zoom_Level;
    }

    m_map->setZoom(zoom);
    update();
}

void MapWidget::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()){
    case Qt::Key_Shift:
        m_gestureType = GESTURE_SHOVE;
        break;
    case Qt::Key_Control:
        m_gestureType = GESTURE_ROTATE;
        break;
    default:
        m_gestureType = GESTURE_NONE;
        break;
    }
}

void MapWidget::keyReleaseEvent(QKeyEvent */*event*/)
{
    m_gestureType = GESTURE_NONE;
}
