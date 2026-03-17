#pragma once

#include <QDateTime>
#include <QObject>
#include <QTimer>
#include <QVariantList>
#include <QVariantMap>

#include "CpuHistoryModel.h"
#include "DiskHistoryModel.h"
#include "MemoryHistoryModel.h"
#include "NetworkHistoryModel.h"
#include "ProcessModel.h"
#include "SystemMonitor.h"

class MonitorBridge : public QObject {
  Q_OBJECT

  // ── Current values ────────────────────────────────────────────────────
  Q_PROPERTY(double cpuOverall READ cpuOverall NOTIFY cpuChanged)
  Q_PROPERTY(QVariantList cpuCores READ cpuCores NOTIFY cpuChanged)

  Q_PROPERTY(qint64 ramUsed READ ramUsed NOTIFY memoryChanged)
  Q_PROPERTY(qint64 ramTotal READ ramTotal NOTIFY memoryChanged)
  Q_PROPERTY(qint64 swapUsed READ swapUsed NOTIFY memoryChanged)
  Q_PROPERTY(qint64 swapTotal READ swapTotal NOTIFY memoryChanged)
  Q_PROPERTY(double ramRatio READ ramRatio NOTIFY memoryChanged)
  Q_PROPERTY(double swapRatio READ swapRatio NOTIFY memoryChanged)

  Q_PROPERTY(QVariantList disks READ disks NOTIFY diskChanged)
  Q_PROPERTY(QVariantList networks READ networks NOTIFY networkChanged)

  // ── History models ────────────────────────────────────────────────────
  Q_PROPERTY(CpuHistoryModel *cpuHistory READ cpuHistory CONSTANT)
  Q_PROPERTY(MemoryHistoryModel *memoryHistory READ memoryHistory CONSTANT)
  Q_PROPERTY(DiskHistoryModel *diskHistory READ diskHistory CONSTANT)
  Q_PROPERTY(NetworkHistoryModel *networkHistory READ networkHistory CONSTANT)

  // ── Process Model ────────────────────────────────────────────────────
  Q_PROPERTY(ProcessProxyModel *processModel READ processModel CONSTANT)
public:
  explicit MonitorBridge(SystemMonitor *monitor, QObject *parent = nullptr);

  double cpuOverall() const { return m_cpuOverall; }
  QVariantList cpuCores() const { return m_cpuCores; }

  qint64 ramUsed() const { return m_ramUsed; }
  qint64 ramTotal() const { return m_ramTotal; }
  qint64 swapUsed() const { return m_swapUsed; }
  qint64 swapTotal() const { return m_swapTotal; }
  double ramRatio() const {
    return m_ramTotal > 0 ? double(m_ramUsed) / double(m_ramTotal) : 0.0;
  }
  double swapRatio() const {
    return m_swapTotal > 0 ? double(m_swapUsed) / double(m_swapTotal) : 0.0;
  }

  QVariantList disks() const { return m_disks; }
  QVariantList networks() const { return m_networks; }

  CpuHistoryModel *cpuHistory() { return m_cpuHistory; }
  MemoryHistoryModel *memoryHistory() { return m_memoryHistory; }
  DiskHistoryModel *diskHistory() { return m_diskHistory; }
  NetworkHistoryModel *networkHistory() { return m_networkHistory; }
  ProcessProxyModel *processModel() { return m_processProxy; }

  Q_INVOKABLE bool killProcess(int pid, bool graceful = true);
  Q_INVOKABLE void forceKillProcess(int pid);

signals:
  void cpuChanged();
  void memoryChanged();
  void diskChanged();
  void networkChanged();

private slots:
  void onCpuData(double overall, QVector<double> perCore);
  void onMemoryData(qint64 totalPhys, qint64 usedPhys, qint64 totalSwap,
                    qint64 usedSwap);
  void onDiskData(QVector<DiskStats> disks);
  void onNetworkData(QVector<NetworkInterfaceStats> interfaces);
  void onProcessData(QVector<ProcessInfo> processes);

private:
  double m_cpuOverall{0.0};
  QVariantList m_cpuCores;

  qint64 m_ramUsed{0};
  qint64 m_ramTotal{0};
  qint64 m_swapUsed{0};
  qint64 m_swapTotal{0};

  QVariantList m_disks;
  QVariantList m_networks;

  CpuHistoryModel *m_cpuHistory{nullptr};
  MemoryHistoryModel *m_memoryHistory{nullptr};
  DiskHistoryModel *m_diskHistory{nullptr};
  NetworkHistoryModel *m_networkHistory{nullptr};
  ProcessModel *m_processModel{nullptr};
  ProcessProxyModel *m_processProxy{nullptr};

  QTimer *m_killTimer{nullptr};
  int m_killPid{-1};

  SystemMonitor *m_monitor{nullptr};
};