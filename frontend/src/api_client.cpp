#include <QHttpMultiPart>
#include <QNetworkReply>
#include <qcborvalue.h>
#include <qcoreapplication.h>
#include <qhashfunctions.h>
#include <qnetworkaccessmanager.h>
#include <qnetworkrequest.h>
#include <qobject.h>
#include <qstringview.h>

#include <functional>

#include "api_client.hpp"

ApiClient::ApiClient(QObject *parent) : QObject(parent) {}

void ApiClient::importFiles(const QByteArray &xml, const QByteArray &json,
                            std::function<void(bool, QString, QString)> cb) {
    auto *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    auto addPart = [&](const QString &name, const QString &filename, const QByteArray &content) {
        QHttpPart part;

        part.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
        part.setHeader(QNetworkRequest::ContentDispositionHeader,
                       QString("form-data; name=\"%1\"; filename=\"%2\"").arg(name, filename));
        part.setBody(content);
        multiPart->append(part);
    };

    addPart("xml", "file.xml", xml);
    addPart("json", "file.json", json);

    QNetworkRequest req(QUrl(customUrl_ + "/api/import"));
    QNetworkReply *reply = net_.post(req, multiPart);
    multiPart->setParent(reply);

    connect(reply, &QNetworkReply::finished, this, [reply, cb]() {
        reply->deleteLater();
        QByteArray data = reply->readAll();
        int code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        bool ok(code == 200 || code == 201);
        cb(ok, QString::fromUtf8(data), ok ? "" : QString("HTTP %1").arg(code));
    });
}
