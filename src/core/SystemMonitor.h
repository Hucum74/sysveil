#pragma once

#include <QObject>
#include <QVector>

#include "CpuProvider.h"
#include "DiskProvider.h"
#include "MemoryProvider.h"
#include "NetworkProvider.h"

// SystemMonitor
// Owns all four providers and a QThread they run on.
// Call start() once — it initializes every provider and begins polling.
// Call stop() before destruction (or let the destructor handle it).
//
// All dataReady signals are re-emitted from this class so the rest of
// the app has a single object to connect to.

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
  // Re-emitted from individual providers — same signatures
  void cpuDataReady(double overallUsage, QVector<double> perCoreUsage);
  void memoryDataReady(qint64 totalPhysical, qint64 usedPhysical,
                       qint64 totalSwap, qint64 usedSwap);
  void diskDataReady(QVector<DiskStats> disks);
  void networkDataReady(QVector<NetworkInterfaceStats> interfaces);

private:
  QThread *m_thread{nullptr};
  QTimer *m_timer{nullptr};

  CpuProvider *m_cpu{nullptr};
  MemoryProvider *m_memory{nullptr};
  DiskProvider *m_disk{nullptr};
  NetworkProvider *m_network{nullptr};
};