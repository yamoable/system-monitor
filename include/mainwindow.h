#pragma once

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QtCharts>
#include <QLineSeries>
#include <QTextEdit>
#include <QTimer>
#include "system_monitor.h"
#include "ota_manager.h" 

QT_CHARTS_USE_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateCPUChart(float usage);
    void updateMemoryChart(float usage);
    void updateTemperature(float temp);
    void handleError(const QString &error);
    void updateSystemInfo();

private:
    void setupUI();
    void setupCharts();
    void setupMonitor();
    void setupOTA();

    SystemMonitor *m_monitor;
    OTAManager *m_otaManager;
    
    // Charts
    QChartView *m_cpuChartView;
    QChartView *m_memChartView;
    QLineSeries *m_cpuSeries;
    QLineSeries *m_memSeries;
    
    // Labels
    QLabel *m_tempLabel;
    QLabel *m_cpuLabel;
    QLabel *m_memLabel;
    
    // Chart data management
    static const int MAX_DATA_POINTS = 50;
    QQueue<QPointF> m_cpuData;
    QQueue<QPointF> m_memData;
    qint64 m_timeCounter;

    QLabel *m_kernelLabel;
    QLabel *m_uptimeLabel;
    QTextEdit *m_processText;
    QLabel *m_diskLabel;
};