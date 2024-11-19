#ifndef OTA_MANAGER_H
#define OTA_MANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QProcess>

class OTAManager : public QObject {
    Q_OBJECT
    
public:
    explicit OTAManager(QObject *parent = nullptr);
    ~OTAManager();
    
    void checkForUpdates(const QUrl &updateUrl);
    QString getCurrentVersion() const;
    bool installUpdate(const QString &filePath);
    
signals:
    void updateAvailable(const QString &version, qint64 size);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadFinished(const QString &filePath);
    void downloadError(const QString &error);
    void installationStarted();
    void installationFinished(bool success);
    
private slots:
    void handleNetworkReply(QNetworkReply *reply);
    void handleDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    
private:
    QNetworkAccessManager *m_networkManager;
    QString m_currentVersion;
    QString m_downloadPath;
    QFile *m_downloadFile;
    
    bool parseUpdateInfo(const QByteArray &data, QString &version, qint64 &size);
};

#endif // OTA_MANAGER_H