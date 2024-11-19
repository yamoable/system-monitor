#pragma once

#include <QObject>
#include <QTimer>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDir>
#include <QProcess>   
#include <QStringList>
#include <QPair>
#include <QMap>

class SystemMonitor : public QObject {
    Q_OBJECT

public:
    explicit SystemMonitor(QObject *parent = nullptr);
    ~SystemMonitor();

    float getCPUUsage() const { return m_cpuUsage; }
    float getMemoryUsage() const { return m_memoryUsage; }
    float getTemperature() const { return m_temperature; }
    
    void startMonitoring();
    void stopMonitoring();

    QString getKernelVersion();
    QList<QPair<QString, QString>> getProcessList();
    QString getSystemUptime();
    QMap<QString, QString> getDiskUsage();


signals:
    void cpuUsageChanged(float usage);
    void memoryUsageChanged(float usage);
    void temperatureChanged(float temp);
    void errorOccurred(const QString &error);

private slots:
    void updateStats();

private:
    QTimer *m_updateTimer;
    float m_cpuUsage;
    float m_memoryUsage;
    float m_temperature;
    
    unsigned long long m_prevIdle;
    unsigned long long m_prevTotal;
    
    float readCPUUsage();
    float readMemoryUsage();
    float readTemperature();
    QString readFile(const QString &path);
    QString executeCommand(const QString &cmd);
};