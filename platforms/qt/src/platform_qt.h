#pragma once

#include "platform.h"
#include <QWidget>
#include <memory>

class URLTaskWorker;
class URLTask;

class Platform_Qt: public QObject, public Platform{
    Q_OBJECT
public:
    Platform_Qt(QWidget& hostWidget);
    ~Platform_Qt();
    // Platform interface
public:
    void requestRender() const;
    bool startUrlRequest(const std::string &_url, UrlCallback _callback);
    void cancelUrlRequest(const std::string &_url);
    std::vector<FontSourceHandle> systemFontFallbacksHandle() const;

private slots:
    void TaskRequestComplete(URLTaskWorker *worker);
private:
    QWidget& m_hostWidget;
    QList<URLTaskWorker*> m_workerList;
    QList<std::shared_ptr<URLTask> > m_taskList;
};

