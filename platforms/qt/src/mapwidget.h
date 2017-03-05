#ifndef MAPWIDGET_H
#define MAPWIDGET_H
#include <QOpenGLWidget>
#include <QString>

namespace Tangram{
class Map;

class MapWidget: public QOpenGLWidget
{
public:
    MapWidget(QWidget* parent);
    ~MapWidget();

public:
    void initialize(const QString& scene);
    Map* getMap();

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void resizeGL(int width, int height) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    // QWidget interface
protected:
    enum GestureType{
        GESTURE_NONE,
        GESTURE_TAP,
        GESTURE_DOUBLE_TAP,
        GESTURE_PAN,
        GESTURE_FLING,
        GESTURE_PINCH,
        GESTURE_ROTATE,
        GESTURE_SHOVE
    };

    Map*            m_map;
    qint64          m_lastRenderTime;

    bool            m_gestureInprogress;
    QPoint          m_gestureStartPoint;
    QPoint          m_gesturePreviousPoint;
    GestureType     m_gestureType;
};

}
#endif // MAPWIDGET_H
