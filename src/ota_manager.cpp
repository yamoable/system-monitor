#include "ota_manager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>
#include <QStandardPaths>

OTAManager::OTAManager(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_currentVersion("1.0.0") 
    , m_downloadFile(nullptr)
{
    m_downloadPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
}

OTAManager::~OTAManager() {
    delete m_downloadFile;
}

void OTAManager::checkForUpdates(const QUrl &updateUrl) {
    QNetworkRequest request(updateUrl);
    QNetworkReply *reply = m_networkManager->get(request);
    
    connect(reply, &QNetworkReply::finished,
            this, [this, reply]() {
                handleNetworkReply(reply);
            });
}

void OTAManager::handleNetworkReply(QNetworkReply *reply) {
    reply->deleteLater();
    
    if (reply->error() != QNetworkReply::NoError) {
        emit downloadError(reply->errorString());
        return;
    }
    
    QByteArray data = reply->readAll();
    QString version;
    qint64 size;
    
    if (parseUpdateInfo(data, version, size)) {
        emit updateAvailable(version, size);
    }
}

bool OTAManager::parseUpdateInfo(const QByteArray &data, QString &version, qint64 &size) {
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) return false;
    
    QJsonObject obj = doc.object();
    version = obj["version"].toString();
    size = obj["size"].toInt();
    
    return !version.isEmpty();
}

void OTAManager::handleDownloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
    emit downloadProgress(bytesReceived, bytesTotal);
}

QString OTAManager::getCurrentVersion() const {
    return m_currentVersion;
}

bool OTAManager::installUpdate(const QString &filePath) {
    emit installationStarted();
    
    // 這裡實作更新安裝邏輯
    // 例如：解壓縮更新包、替換檔案等
    
    bool success = true; // 根據實際安裝結果設定
    
    emit installationFinished(success);
    return success;
}