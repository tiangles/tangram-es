#include <QtNetwork>
#include <QNetworkReply>
#include <QUrl>
#include "urlWorker.h"


//
// class URLTask
//


URLTask::URLTask(const std::string& url, UrlCallback callback) {
    this->url = url;
    this->callback = callback;
}

const std::string& URLTask::GetURL(void) const {
    return url;
}

UrlCallback URLTask::GetCallback(void) const {
    return callback;
}


//
// class URLTaskWorker
//


URLTaskWorker::URLTaskWorker() {
    downloading = false;
    connect(&qnam, &QNetworkAccessManager::sslErrors, this, &URLTaskWorker::sslErrors);
}

URLTaskWorker::~URLTaskWorker() {

}

const std::string URLTaskWorker::GetURL(void) const {
    if (task != Q_NULLPTR) {
        return task->GetURL();
    }
    else {
        return "";
    }
}

void URLTaskWorker::HandleTask(const URLTask *task) {
    if (IsDownloading()) {
        return;
    }

    this->task = task;
    httpRequestAborted = false;
    QUrl url = QUrl(task->GetURL().c_str());
    receivedData.clear();
    reply = qnam.get(QNetworkRequest(url));
    connect(reply, &QIODevice::readyRead, this, &URLTaskWorker::httpReadyRead);
    connect(reply, &QNetworkReply::finished, this, &URLTaskWorker::httpFinished);
    downloading = true;
}

void URLTaskWorker::CancelRequest() {
    httpRequestAborted = true;
    reply->abort();
}

bool URLTaskWorker::IsDownloading(void) const {
    return downloading;
}

//
// Private slots
//

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
            task->GetCallback()(std::move(data));
        }
    }

    delete task;
    task = Q_NULLPTR;
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
