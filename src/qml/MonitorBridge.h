#pragma once

#include <QObject>
#include <QVariantList>
#include <QVariantMap>

#include "SystemMonitor.h"

// MonitorBridge
// Sits between SystemMonitor (C++ worker thread) and the QML engine.
// All properties are updated on the main thread via queued connections.
// QML binds to properties directly — no polling, no timers in QML.

class MonitorBridge : public QObject {
  Q_OBJECT

  // ── CPU ───────────────────────────────────────────────────────────────
  Q_PROPERTY(double cpuOverall READ cpuOverall NOTIFY cpuChanged)
  Q_PROPERTY(QVariantList cpuCores READ cpuCores NOTIFY cpuChanged)

  // ── Memory ────────────────────────────────────────────────────────────
  Q_PROPERTY(qint64 ramUsed READ ramUsed NOTIFY memoryChanged)
  Q_PROPERTY(qint64 ramTotal READ ramTotal NOTIFY memoryChanged)
  Q_PROPERTY(qint64 swapUsed READ swapUsed NOTIFY memoryChanged)
  Q_PROPERTY(qint64 swapTotal READ swapTotal NOTIFY memoryChanged)

  // ── Convenience: 0.0–1.0 ratios for progress bars ────────────────────
  Q_PROPERTY(double ramRatio READ ramRatio NOTIFY memoryChanged)
  Q_PROPERTY(double swapRatio READ swapRatio NOTIFY memoryChanged)

  // ── Disk ──────────────────────────────────────────────────────────────
  // Each entry is a QVariantMap with keys:
  //   mountPoint (string), totalBytes (qint64), usedBytes (qint64),
  //   readBytesPerSec (qint64), writeBytesPerSec (qint64), usageRatio (double)
  Q_PROPERTY(QVariantList disks READ disks NOTIFY diskChanged)

  // ── Network ───────────────────────────────────────────────────────────
  // Each entry is a QVariantMap with keys:
  //   name (string), rxBytesPerSec (qint64), txBytesPerSec (qint64)
  Q_PROPERTY(QVariantList networks READ networks NOTIFY networkChanged)

public:
  explicit MonitorBridge(SystemMonitor *monitor, QObject *parent = nullptr);

  // Property accessors
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

private:
  // ── Cached values ─────────────────────────────────────────────────────
  double m_cpuOverall{0.0};
  QVariantList m_cpuCores;

  qint64 m_ramUsed{0};
  qint64 m_ramTotal{0};
  qint64 m_swapUsed{0};
  qint64 m_swapTotal{0};

  QVariantList m_disks;
  QVariantList m_networks;
};