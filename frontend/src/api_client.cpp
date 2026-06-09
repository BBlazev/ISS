#include <QFile>
#include <QHttpMultiPart>
#include <QNetworkReply>
#include <cstdio>
#include <cstdlib>
#include <qcborvalue.h>
#include <qcoreapplication.h>
#include <qevent.h>
#include <qhashfunctions.h>
#include <qnetworkaccessmanager.h>
#include <qnetworkrequest.h>
#include <qobject.h>
#include <qstringview.h>
#include <sys/wait.h>

#include <array>
#include <functional>
#include <string>

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

void ApiClient::soapFile(const QByteArray &xml, std::function<void(bool, QString, QString)> cb) {
    QNetworkRequest req(QUrl("http://localhost:8081"));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "text/xml; charset=utf-8");
    req.setRawHeader("SOAPAction", "\"\"");

    QNetworkReply *reply = net_.post(req, xml);

    connect(reply, &QNetworkReply::finished, this, [reply, cb]() {
        QByteArray data = reply->readAll();
        if (reply->error() != QNetworkReply::NoError) {
            cb(false, {}, "SOAP error: " + reply->errorString());
        } else {
            cb(true, QString::fromUtf8(data), "");
        }
        reply->deleteLater();
    });
}

Result ApiClient::java_validate() {

    static const std::string JAR_PATH = "validator/target/validator.jar";
    static const std::string XSD_PATH = "schemas/files.xsd";

    Result result;
    std::string cmd = "java -jar " + JAR_PATH + " " + "files.xml" + " " + XSD_PATH + " 2>&1";

    FILE *p = popen(cmd.c_str(), "r");
    if (!p) {
        result.code = -1;
        return {result.code, "{\"valid\":false,\"errors\":[],\"error\":\"failed to start java\"}"};
    }
    std::array<char, 256> buffer;

    while (fgets(buffer.data(), buffer.size(), p) != nullptr) {
        result.output += buffer.data();
    }
    int status = pclose(p);
    if (status == -1)
        result.code = -1;
    else
        result.code = WEXITSTATUS(status);

    return result;
}
