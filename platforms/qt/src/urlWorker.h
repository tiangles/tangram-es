#ifndef URLWORKER_H
#define URLWORKER_H

#include <QNetworkAccessManager>
#include <QByteArray>
#include <string>
#include <memory>
#include "platform.h"

class QNetworkReply;

struct URLTask {
    URLTask(const std::string& url, UrlCallback& callback):
        url(url), callback(callback)
    {
    }

    std::string             url;
    UrlCallback             callback;
};
typedef std::shared_ptr<URLTask> URLTaskPtr;

class URLTaskWorker : public QObject {
    Q_OBJECT

public:
    URLTaskWorker();
    ~URLTaskWorker();

public:
    void HandleTask(URLTaskPtr task);
    void CancelRequest();
    bool IsDownloading(void) const;

signals:
    void StartNewRequest();
    void RequestComplete(URLTaskWorker *task);

private slots:
    void onNewRequest();
    void httpFinished();
    void httpReadyRead();
    void sslErrors(QNetworkReply* reply, const QList<QSslError>& errors);

private:
    QNetworkAccessManager       qnam;

    bool                        downloading;
    bool                        httpRequestAborted;
    URLTaskPtr                  task;

    QNetworkReply           *reply;
    QByteArray              receivedData;
};

#endif
