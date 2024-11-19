#include "system_monitor.h"
#include <QDebug>

SystemMonitor::SystemMonitor(QObject *parent) 
    : QObject(parent)
    , m_updateTimer(new QTimer(this))
    , m_cpuUsage(0.0f)
    , m_memoryUsage(0.0f)
    , m_temperature(0.0f)
    , m_prevIdle(0)
    , m_prevTotal(0)
{
    connect(m_updateTimer, &QTimer::timeout, this, &SystemMonitor::updateStats);
}

SystemMonitor::~SystemMonitor() {
    stopMonitoring();
}

void SystemMonitor::startMonitoring() {
    m_updateTimer->start(1000);
}

void SystemMonitor::stopMonitoring() {
    m_updateTimer->stop();
}

QString SystemMonitor::readFile(const QString &path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit errorOccurred("Cannot open file: " + path);
        return QString();
    }
    
    QTextStream in(&file);
    QString content = in.readAll();
    file.close();
    return content;
}

float SystemMonitor::readCPUUsage() {
    QString content = readFile("/proc/stat");
    if (content.isEmpty()) return 0.0f;
    
    QStringList lines = content.split('\n');
    if (lines.isEmpty()) return 0.0f;
    
    QStringList values = lines[0].split(QRegExp("\\s+"), Qt::SkipEmptyParts);
    if (values.size() < 5) return 0.0f;
    
    unsigned long long user = values[1].toULongLong();
    unsigned long long nice = values[2].toULongLong();
    unsigned long long system = values[3].toULongLong();
    unsigned long long idle = values[4].toULongLong();
    
    unsigned long long total = user + nice + system + idle;
    unsigned long long totalDiff = total - m_prevTotal;
    unsigned long long idleDiff = idle - m_prevIdle;
    
    m_prevTotal = total;
    m_prevIdle = idle;
    
    if (totalDiff == 0) return 0.0f;
    return 100.0f * (1.0f - static_cast<float>(idleDiff) / totalDiff);
}

float SystemMonitor::readMemoryUsage() {
    QString content = readFile("/proc/meminfo");
    if (content.isEmpty()) return 0.0f;
    
    QStringList lines = content.split('\n');
    unsigned long total = 0, free = 0, buffers = 0, cached = 0;
    
    for (const QString &line : lines) {
        if (line.startsWith("MemTotal:"))
            total = line.split(QRegExp("\\s+"))[1].toULong();
        else if (line.startsWith("MemFree:"))
            free = line.split(QRegExp("\\s+"))[1].toULong();
        else if (line.startsWith("Buffers:"))
            buffers = line.split(QRegExp("\\s+"))[1].toULong();
        else if (line.startsWith("Cached:"))
            cached = line.split(QRegExp("\\s+"))[1].toULong();
    }
    
    if (total == 0) return 0.0f;
    unsigned long used = total - free - buffers - cached;
    return 100.0f * static_cast<float>(used) / total;
}

float SystemMonitor::readTemperature() {
    return 45.0f + (rand() % 10);
}

void SystemMonitor::updateStats() {
    m_cpuUsage = readCPUUsage();
    m_memoryUsage = readMemoryUsage();
    m_temperature = readTemperature();
    
    emit cpuUsageChanged(m_cpuUsage);
    emit memoryUsageChanged(m_memoryUsage);
    emit temperatureChanged(m_temperature);
}

QString SystemMonitor::getKernelVersion() {
    return readFile("/proc/version").trimmed();
}

QString SystemMonitor::getSystemUptime() {
    QFile file("/proc/uptime");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return "Unknown";

    QString uptime = file.readLine();
    double seconds = uptime.split(" ")[0].toDouble();
    int days = seconds / 86400;
    int hours = (seconds / 3600) - (days * 24);
    int minutes = (seconds / 60) - (days * 1440) - (hours * 60);

    return QString("%1 days, %2 hours, %3 minutes")
        .arg(days).arg(hours).arg(minutes);
}

QList<QPair<QString, QString>> SystemMonitor::getProcessList() {
    QList<QPair<QString, QString>> processes;
    QDir procDir("/proc");
    QStringList entries = procDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QString &entry : entries) {
        bool ok;
        entry.toInt(&ok);
        if (!ok) continue;

        QString cmdline = readFile(QString("/proc/%1/cmdline").arg(entry));
        QString status = readFile(QString("/proc/%1/status").arg(entry));
        
        QString name;
        QStringList lines = status.split('\n');
        for (const QString &line : lines) {
            if (line.startsWith("Name:")) {
                name = line.mid(5).trimmed();
                break;
            }
        }

        if (!name.isEmpty()) {
            processes.append(qMakePair(entry, name));
        }
    }
    return processes;
}

QMap<QString, QString> SystemMonitor::getDiskUsage() {
    QMap<QString, QString> usage;
    QString df = executeCommand("df -h");
    QStringList lines = df.split('\n', Qt::SkipEmptyParts);
    
    for (int i = 1; i < lines.size(); ++i) {
        QStringList parts = lines[i].split(QRegExp("\\s+"));
        if (parts.size() >= 6) {
            QString filesystem = parts[0];
            QString used = parts[4];
            usage[filesystem] = used;
        }
    }
    return usage;
}

QString SystemMonitor::executeCommand(const QString &cmd) {
    QProcess process;
    process.start("sh", QStringList() << "-c" << cmd);
    process.waitForFinished();
    return QString(process.readAllStandardOutput());
}