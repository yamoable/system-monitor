#include "mainwindow.h"
#include "ota_manager.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_monitor(new SystemMonitor(this))
    , m_otaManager(new OTAManager(this))
    , m_timeCounter(0)
{
    setupUI();
    setupCharts();
    setupMonitor();
}

MainWindow::~MainWindow() {
    m_monitor->stopMonitoring();
}

void MainWindow::setupUI() {
    // Create central widget and layout
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    // Create labels
    m_tempLabel = new QLabel("Temperature: N/A", this);

    QFont tempFont = m_tempLabel->font();
    tempFont.setPointSize(12);
    m_tempLabel->setFont(tempFont);

    connect(m_monitor, &SystemMonitor::temperatureChanged,
            this, [this](float temp) {
                QString style;
                if (temp > 70) {
                    style = "color: red; font-weight: bold;";
                } else if (temp > 50) {
                    style = "color: orange;";
                } else {
                    style = "color: green;";
                }
                m_tempLabel->setStyleSheet(style);
                updateTemperature(temp);
            });

    m_cpuLabel = new QLabel("CPU Usage: N/A", this);
    m_memLabel = new QLabel("Memory Usage: N/A", this);
    
    // Create charts layouts
    QHBoxLayout *chartsLayout = new QHBoxLayout();
    
    // Setup charts
    m_cpuChartView = new QChartView(this);
    m_memChartView = new QChartView(this);
    m_cpuChartView->setMinimumSize(400, 300);
    m_memChartView->setMinimumSize(400, 300);
    
    chartsLayout->addWidget(m_cpuChartView);
    chartsLayout->addWidget(m_memChartView);
    
    // Add widgets to main layout
    mainLayout->addWidget(m_tempLabel);
    mainLayout->addWidget(m_cpuLabel);
    mainLayout->addWidget(m_memLabel);
    mainLayout->addLayout(chartsLayout);
    
    setCentralWidget(centralWidget);
    setWindowTitle("System Monitor");
    resize(900, 600);

    // System Information
    QVBoxLayout *sysInfoLayout = new QVBoxLayout();
    
    m_kernelLabel = new QLabel(this);
    m_uptimeLabel = new QLabel(this);
    m_diskLabel = new QLabel(this);
    
    m_processText = new QTextEdit(this);
    m_processText->setReadOnly(true);
    m_processText->setMaximumHeight(150);
    
    sysInfoLayout->addWidget(new QLabel("System Information:", this));
    sysInfoLayout->addWidget(m_kernelLabel);
    sysInfoLayout->addWidget(m_uptimeLabel);
    sysInfoLayout->addWidget(m_diskLabel);
    sysInfoLayout->addWidget(new QLabel("Running Processes:", this));
    sysInfoLayout->addWidget(m_processText);
    
    mainLayout->addLayout(sysInfoLayout);
    
    // Timer
    QTimer *sysInfoTimer = new QTimer(this);
    connect(sysInfoTimer, &QTimer::timeout, this, &MainWindow::updateSystemInfo);
    sysInfoTimer->start(5000); //
    
    updateSystemInfo(); //
}

void MainWindow::setupCharts() {
    // Setup CPU chart
    m_cpuSeries = new QLineSeries(this);
    QChart *cpuChart = new QChart();
    cpuChart->addSeries(m_cpuSeries);
    cpuChart->setTitle("CPU Usage");
    
    QValueAxis *cpuAxisX = new QValueAxis;
    cpuAxisX->setRange(0, MAX_DATA_POINTS);
    cpuAxisX->setLabelFormat("%d");
    cpuAxisX->setTitleText("Time (s)");
    
    QValueAxis *cpuAxisY = new QValueAxis;
    cpuAxisY->setRange(0, 100);
    cpuAxisY->setLabelFormat("%.1f%%");
    cpuAxisY->setTitleText("Usage");
    
    cpuChart->addAxis(cpuAxisX, Qt::AlignBottom);
    cpuChart->addAxis(cpuAxisY, Qt::AlignLeft);
    m_cpuSeries->attachAxis(cpuAxisX);
    m_cpuSeries->attachAxis(cpuAxisY);
    
    m_cpuChartView->setChart(cpuChart);
    m_cpuChartView->setRenderHint(QPainter::Antialiasing);
    
    // Setup Memory chart
    m_memSeries = new QLineSeries(this);
    QChart *memChart = new QChart();
    memChart->addSeries(m_memSeries);
    memChart->setTitle("Memory Usage");
    
    QValueAxis *memAxisX = new QValueAxis;
    memAxisX->setRange(0, MAX_DATA_POINTS);
    memAxisX->setLabelFormat("%d");
    memAxisX->setTitleText("Time (s)");
    
    QValueAxis *memAxisY = new QValueAxis;
    memAxisY->setRange(0, 100);
    memAxisY->setLabelFormat("%.1f%%");
    memAxisY->setTitleText("Usage");
    
    memChart->addAxis(memAxisX, Qt::AlignBottom);
    memChart->addAxis(memAxisY, Qt::AlignLeft);
    m_memSeries->attachAxis(memAxisX);
    m_memSeries->attachAxis(memAxisY);
    
    m_memChartView->setChart(memChart);
    m_memChartView->setRenderHint(QPainter::Antialiasing);
}

void MainWindow::setupMonitor() {
    // Connect signals from system monitor
    connect(m_monitor, &SystemMonitor::cpuUsageChanged,
            this, &MainWindow::updateCPUChart);
    connect(m_monitor, &SystemMonitor::memoryUsageChanged,
            this, &MainWindow::updateMemoryChart);
    connect(m_monitor, &SystemMonitor::temperatureChanged,
            this, &MainWindow::updateTemperature);
    connect(m_monitor, &SystemMonitor::errorOccurred,
            this, &MainWindow::handleError);
            
    // Start monitoring
    m_monitor->startMonitoring();
}

void MainWindow::updateCPUChart(float usage) {
    m_cpuLabel->setText(QString("CPU Usage: %1%").arg(usage, 0, 'f', 1));
    
    m_cpuData.enqueue(QPointF(m_timeCounter, usage));
    if (m_cpuData.size() > MAX_DATA_POINTS)
        m_cpuData.dequeue();
        
    m_cpuSeries->clear();
    for (int i = 0; i < m_cpuData.size(); ++i) {
        m_cpuSeries->append(i, m_cpuData[i].y());
    }
}

void MainWindow::updateMemoryChart(float usage) {
    m_memLabel->setText(QString("Memory Usage: %1%").arg(usage, 0, 'f', 1));
    
    m_memData.enqueue(QPointF(m_timeCounter, usage));
    if (m_memData.size() > MAX_DATA_POINTS)
        m_memData.dequeue();
        
    m_memSeries->clear();
    for (int i = 0; i < m_memData.size(); ++i) {
        m_memSeries->append(i, m_memData[i].y());
    }
    
    m_timeCounter++;
}

void MainWindow::updateTemperature(float temp) {
    m_tempLabel->setText(QString("Temperature: %1\u2103").arg(temp, 0, 'f', 1));
}

void MainWindow::handleError(const QString &error) {
    QMessageBox::warning(this, "Error", error);
}

void MainWindow::updateSystemInfo() {
    m_kernelLabel->setText("Kernel: " + m_monitor->getKernelVersion());
    
    m_uptimeLabel->setText("Uptime: " + m_monitor->getSystemUptime());

    QString diskInfo = "Disk Usage:\n";
    auto diskUsage = m_monitor->getDiskUsage();
    for (auto it = diskUsage.begin(); it != diskUsage.end(); ++it) {
        diskInfo += QString("%1: %2\n").arg(it.key(), it.value());
    }
    m_diskLabel->setText(diskInfo);
    
    QString processInfo;
    auto processes = m_monitor->getProcessList();
    for (const auto &proc : processes) {
        processInfo += QString("PID: %1 - %2\n").arg(proc.first, proc.second);
    }
    m_processText->setText(processInfo);
}

void MainWindow::setupOTA() {
    m_otaManager = new OTAManager(this);
    
    // connect signal
    connect(m_otaManager, &OTAManager::updateAvailable,
            this, [this](const QString &version, qint64 size) {
                QString message = QString("New version %1 available (%2 MB)")
                    .arg(version)
                    .arg(size / 1024.0 / 1024.0, 0, 'f', 2);
                    
                if (QMessageBox::question(this, "Update Available", 
                    message + "\nDo you want to update?") == QMessageBox::Yes) {
                    // download update
                }
            });
            
    connect(m_otaManager, &OTAManager::downloadProgress,
            this, [this](qint64 received, qint64 total) {
                // update progress
            });
            
    m_otaManager->checkForUpdates(QUrl("https://your-update-server.com/updates.json"));
}