#ifndef URLWORKER_H
#define URLWORKER_H

#include <QNetworkAccessManager>
#include <QByteArray>
#include <string>
#include "platform.h"

class QNetworkReply;

class URLTask {
public:
    URLTask(const std::string& url, UrlCallback callback);

public:
    const std::string& GetURL(void) const;
    UrlCallback GetCallback(void) const;

private:
    std::string             url;
    UrlCallback             callback;
};

class URLTaskWorker : public QObject {
    Q_OBJECT

public:
    URLTaskWorker();
    ~URLTaskWorker();

public:
    const std::string GetURL(void) const;
    void HandleTask(const URLTask* task);
    void CancelRequest();
    bool IsDownloading(void) const;

signals:
    void RequestComplete(URLTaskWorker *task);

private slots:
    void httpFinished();
    void httpReadyRead();
    void sslErrors(QNetworkReply* reply, const QList<QSslError>& errors);

private:
    bool                        downloading;
    bool                        httpRequestAborted;
    const URLTask               *task;

    QNetworkAccessManager       qnam;
    QNetworkReply               *reply;
    QByteArray                  receivedData;
};

#endif
