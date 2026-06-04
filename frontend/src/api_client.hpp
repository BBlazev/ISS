#pragma once

#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QObject>
#include <QString>
#include <qobject.h>
#include <qstringview.h>
#include <qtmetamacros.h>

#include <functional>

enum class DataSource { Directus, Custom };

struct Session {
    DataSource source = DataSource::Custom;
    QString username;
    QString role;
    QString accessToken;
    QString refreshToken;
    bool isFullAccess() const { return role == "full_access"; }
};

class ApiClient : public QObject {
    Q_OBJECT
  public:
    explicit ApiClient(QObject *parent = nullptr);

    void setSource(DataSource s) { session_.source = s; }
    DataSource source() const { return session_.source; }
    Session &session() { return session_; }

    void importFiles(const QByteArray &xml, const QByteArray &json,
                     std::function<void(bool, QString, QString)> cb);

  private:
    QNetworkAccessManager net_;
    Session session_;

    QString directusUrl_ = "https://bblazeviss.directus.app";
    QString directusToken_ = "StQpc5gixGoV0IpccXBVpO22tfQRwwGr";
    QString customUrl_ = "http://localhost:8080";
    QString soapUrl_ = "http://localhost:8081";
};
