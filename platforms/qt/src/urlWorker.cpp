#include <QtNetwork>
#include <QNetworkReply>
#include <QUrl>
#include "urlWorker.h"

//
// class URLTaskWorker
//

URLTaskWorker::URLTaskWorker() {
    downloading = false;
    connect(&qnam, &QNetworkAccessManager::sslErrors, this, &URLTaskWorker::sslErrors);
    connect(this,SIGNAL(StartNewRequest()),this, SLOT(onNewRequest()), Qt::QueuedConnection);
}

URLTaskWorker::~URLTaskWorker() {

}

void URLTaskWorker::HandleTask(URLTaskPtr task) {
    if (IsDownloading()) {
        return;
    }
    this->task = task;
    downloading = true;
    emit StartNewRequest();
}

void URLTaskWorker::CancelRequest() {
    httpRequestAborted = true;
    reply->abort();
}

bool URLTaskWorker::IsDownloading(void) const {
    return downloading || task;
}

void URLTaskWorker::onNewRequest()
{
    httpRequestAborted = false;
    QUrl url = QUrl(task->url.c_str());
    receivedData.clear();
    reply = qnam.get(QNetworkRequest(url));
    connect(reply, &QIODevice::readyRead, this, &URLTaskWorker::httpReadyRead);
    connect(reply, &QNetworkReply::finished, this, &URLTaskWorker::httpFinished);
}

void URLTaskWorker::httpFinished() {
    if (httpRequestAborted) {
        // User cancel the request. No need to invoke the callback.
    }
    else {
        if (reply->error()) {
            // TODO: Request failed with error.
        }
        else {
            // Request success with received data.
            std::vector<char> data;
            int size = receivedData.size();
            data.resize(size);
            memcpy(data.data(), receivedData.constData(), size);
            task->callback(std::move(data));
        }
    }

    task.reset();
    reply->deleteLater();
    reply = Q_NULLPTR;
    downloading = false;

    emit RequestComplete(this);
}

void URLTaskWorker::httpReadyRead() {
    if (reply) {
        receivedData.append(reply->readAll());
    }
}

void URLTaskWorker::sslErrors(QNetworkReply*, const QList<QSslError>& errors) {
    QString errorString;
    foreach (const QSslError &error, errors) {
        if (!errorString.isEmpty())
            errorString += '\n';
        errorString += error.errorString();
    }
    qWarning()<< errorString;
    reply->ignoreSslErrors();
}
