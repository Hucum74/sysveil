#pragma once

#include <QObject>
#include <QVector>
#include <QString>

struct ProcessInfo {
    int     pid        { 0 };
    QString name;
    QString user;
    double  cpuUsage   { 0.0 };  // 0.0–100.0
    double  memoryPerc { 0.0 };  // 0.0–100.0
    qint64  memoryMiB  { 0 };    // RSS in MiB
};

class ProcessProvider : public QObject
{
    Q_OBJECT

public:
    explicit ProcessProvider(QObject* parent = nullptr);
    ~ProcessProvider() override;

    void initialize();

    // Kill a process gracefully (SIGTERM → wait → SIGKILL)
    // Returns false if permission denied or process not found
    bool killProcess(int pid, bool graceful = true);

public slots:
    void poll();

signals:
    void processDataReady(QVector<ProcessInfo> processes);

private:
    struct Impl;
    Impl* d { nullptr };
};
