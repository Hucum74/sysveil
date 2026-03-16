#include "MonitorBridge.h"

#include <QDebug>

MonitorBridge::MonitorBridge(SystemMonitor *monitor, QObject *parent)
    : QObject(parent), m_cpuHistory(new CpuHistoryModel(60, this)),
      m_memoryHistory(new MemoryHistoryModel(60, this)),
      m_diskHistory(new DiskHistoryModel(60, this)),
      m_networkHistory(new NetworkHistoryModel(60, this)) {
  Q_ASSERT(monitor);

  connect(monitor, &SystemMonitor::cpuDataReady, this,
          &MonitorBridge::onCpuData);
  connect(monitor, &SystemMonitor::memoryDataReady, this,
          &MonitorBridge::onMemoryData);
  connect(monitor, &SystemMonitor::diskDataReady, this,
          &MonitorBridge::onDiskData);
  connect(monitor, &SystemMonitor::networkDataReady, this,
          &MonitorBridge::onNetworkData);
}

void MonitorBridge::onCpuData(double overall, QVector<double> perCore) {
  m_cpuOverall = overall;
  m_cpuCores.clear();
  for (double v : perCore)
    m_cpuCores.append(v);

  // Feed history model
  CpuSample s;
  s.timestamp = QDateTime::currentMSecsSinceEpoch();
  s.overall = overall;
  s.cores = perCore;
  m_cpuHistory->append(s);

  emit cpuChanged();
}

void MonitorBridge::onMemoryData(qint64 totalPhys, qint64 usedPhys,
                                 qint64 totalSwap, qint64 usedSwap) {
  m_ramTotal = totalPhys;
  m_ramUsed = usedPhys;
  m_swapTotal = totalSwap;
  m_swapUsed = usedSwap;

  MemorySample s;
  s.timestamp = QDateTime::currentMSecsSinceEpoch();
  s.usedBytes = usedPhys;
  s.totalBytes = totalPhys;
  s.usedSwap = usedSwap;
  s.totalSwap = totalSwap;
  m_memoryHistory->append(s);

  emit memoryChanged();
}

void MonitorBridge::onDiskData(QVector<DiskStats> disks) {
  m_disks.clear();
  for (const DiskStats &d : disks) {
    QVariantMap entry;
    entry[QStringLiteral("mountPoint")] = d.mountPoint;
    entry[QStringLiteral("totalBytes")] = d.totalBytes;
    entry[QStringLiteral("usedBytes")] = d.usedBytes;
    entry[QStringLiteral("readBytesPerSec")] = d.readBytesPerSec;
    entry[QStringLiteral("writeBytesPerSec")] = d.writeBytesPerSec;
    entry[QStringLiteral("usageRatio")] =
        d.totalBytes > 0 ? double(d.usedBytes) / double(d.totalBytes) : 0.0;
    m_disks.append(entry);

    // Feed disk history — one model tracks the primary mount point
    if (d.mountPoint == QStringLiteral("/")) {
      DiskSample s;
      s.timestamp = QDateTime::currentMSecsSinceEpoch();
      s.mountPoint = d.mountPoint;
      s.usedBytes = d.usedBytes;
      s.totalBytes = d.totalBytes;
      s.readBytesPerSec = d.readBytesPerSec;
      s.writeBytesPerSec = d.writeBytesPerSec;
      m_diskHistory->append(s);
    }
  }

  emit diskChanged();
}

void MonitorBridge::onNetworkData(QVector<NetworkInterfaceStats> interfaces) {
  m_networks.clear();
  for (const NetworkInterfaceStats &iface : interfaces) {
    const QString &name = iface.name;
    if (name.startsWith("veth") || name.startsWith("br-") ||
        name.startsWith("docker") || name.startsWith("lo"))
      continue;

    QVariantMap entry;
    entry[QStringLiteral("name")] = iface.name;
    entry[QStringLiteral("rxBytesPerSec")] = iface.rxBytesPerSec;
    entry[QStringLiteral("txBytesPerSec")] = iface.txBytesPerSec;
    m_networks.append(entry);

    // Feed network history for the primary interface
    NetworkSample s;
    s.timestamp = QDateTime::currentMSecsSinceEpoch();
    s.interfaceName = iface.name;
    s.rxBytesPerSec = iface.rxBytesPerSec;
    s.txBytesPerSec = iface.txBytesPerSec;
    m_networkHistory->append(s);
  }

  emit networkChanged();
}