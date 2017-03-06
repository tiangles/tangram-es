#include "gl/hardware.h"
#include "platform_qt.h"
#include "urlWorker.h"

#include <QThread>
#include <pthread.h>
#include <fstream>
#include <QDebug>

#include <libgen.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/syscall.h>


void logMsg(const char* fmt, ...){
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}

void initGLExtensions(){
    Tangram::Hardware::supportsMapBuffer = true;
}

// Set the priority of the current thread. Priority is equivalent to pthread niceness
void setCurrentThreadPriority(int priority){
    int tid = syscall(SYS_gettid);
    setpriority(PRIO_PROCESS, tid, priority);
}

Platform_Qt::Platform_Qt(QWidget &hostWidget)
    :m_hostWidget(hostWidget)
{
    for(int i=0; i<10; ++i){
        auto worker = new URLTaskWorker();
        connect(worker, SIGNAL(RequestComplete(URLTaskWorker*)), this, SLOT(TaskRequestComplete(URLTaskWorker*)));
        m_workerList.push_back(worker);
    }
}

Platform_Qt::~Platform_Qt()
{
    for(auto worker: m_workerList){
        delete worker;
    }
}

void Platform_Qt::requestRender() const{
    m_hostWidget.update();
}

bool Platform_Qt::startUrlRequest(const std::string &url, UrlCallback callback){
    auto task = std::make_shared<URLTask>(url, callback);
    bool hasIdleWorker = false;
    QList<URLTaskWorker*>::iterator i = m_workerList.begin();
    for (; i != m_workerList.end(); ++i) {
        URLTaskWorker *worker = *i;
        if (!worker->IsDownloading()) {
            worker->HandleTask(task);
            hasIdleWorker = true;
            break;
        }
    }

    if (!hasIdleWorker) {
        m_taskList.append(task);
    }
    return true;
}

void Platform_Qt::cancelUrlRequest(const std::string &/*_url*/){

}

void Platform_Qt::TaskRequestComplete(URLTaskWorker *worker) {
    if (!m_taskList.empty()) {
        auto task = m_taskList.front();
        m_taskList.pop_front();
        worker->HandleTask(task);
    }
}

#define DEFAULT "fonts/NotoSans-Regular.ttf"
#define FONT_AR "fonts/NotoNaskh-Regular.ttf"
#define FONT_HE "fonts/NotoSansHebrew-Regular.ttf"
#define FONT_JA "fonts/DroidSansJapanese.ttf"
#define FALLBACK "fonts/DroidSansFallback.ttf"

std::vector<FontSourceHandle> Platform_Qt::systemFontFallbacksHandle() const {
    std::vector<FontSourceHandle> handles;

    handles.emplace_back(DEFAULT);
    handles.emplace_back(FONT_AR);
    handles.emplace_back(FONT_HE);
    handles.emplace_back(FONT_JA);
    handles.emplace_back(FALLBACK);

    return handles;
}
