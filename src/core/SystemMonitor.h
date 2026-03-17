#pragma once

#include <QObject>
#include <QVector>

#include "CpuProvider.h"
#include "DiskProvider.h"
#include "MemoryProvider.h"
#include "NetworkProvider.h"
#include "ProcessProvider.h"

class QThread;
class QTimer;

class SystemMonitor : public QObject {
  Q_OBJECT

public:
  explicit SystemMonitor(QObject *parent = nullptr);
  ~SystemMonitor() override;

  void start(int intervalMs = 1000);
  void stop();

signals:
  void cpuDataReady(double overallUsage, QVector<double> perCoreUsage);
  void memoryDataReady(qint64 totalPhysical, qint64 usedPhysical,
                       qint64 totalSwap, qint64 usedSwap);
  void diskDataReady(QVector<DiskStats> disks);
  void networkDataReady(QVector<NetworkInterfaceStats> interfaces);
  void processDataReady(QVector<ProcessInfo> processes);

private:
  int m_intervalMs{1000};
  QThread *m_thread{nullptr};
  QTimer *m_timer{nullptr};
  QTimer *m_processTimer{nullptr};

  CpuProvider *m_cpu{nullptr};
  MemoryProvider *m_memory{nullptr};
  DiskProvider *m_disk{nullptr};
  NetworkProvider *m_network{nullptr};
  ProcessProvider *m_process{nullptr};
};