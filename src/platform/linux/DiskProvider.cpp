#include "DiskProvider.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QStorageInfo>

#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

struct DiskProvider::Impl {

  struct IoTicks {
    long long readBytes{0};
    long long writeBytes{0};
  };

  std::unordered_map<std::string, IoTicks> previousIo;

  // Returns all block devices (disks + partitions) from /proc/diskstats
  static std::unordered_map<std::string, IoTicks> readDiskStats() {
    std::unordered_map<std::string, IoTicks> result;
    std::ifstream file("/proc/diskstats");

    if (!file.is_open()) {
      qWarning() << "DiskProvider: cannot open /proc/diskstats";
      return result;
    }

    std::string line;
    while (std::getline(file, line)) {
      std::istringstream ss(line);
      int major, minor;
      std::string name;
      long long readsCompleted, readsMerged, sectorsRead, readMs;
      long long writesCompleted, writesMerged, sectorsWritten, writeMs;

      ss >> major >> minor >> name >> readsCompleted >> readsMerged >>
          sectorsRead >> readMs >> writesCompleted >> writesMerged >>
          sectorsWritten >> writeMs;

      // Skip zram and loop devices — not real disks
      if (name.rfind("zram", 0) == 0)
        continue;
      if (name.rfind("loop", 0) == 0)
        continue;

      IoTicks t;
      t.readBytes = sectorsRead * 512LL;
      t.writeBytes = sectorsWritten * 512LL;
      result[name] = t;
    }

    return result;
  }

  // Resolve a device path like /dev/nvme1n1p3 to its bare name nvme1n1p3
  static std::string devName(const QString &devicePath) {
    // QStorageInfo::device() may return /dev/xxx or a UUID path —
    // resolve symlinks first
    QFileInfo fi(devicePath);
    const QString resolved = fi.exists() ? fi.canonicalFilePath() : devicePath;
    return QFileInfo(resolved).fileName().toStdString();
  }
};

DiskProvider::DiskProvider(QObject *parent) : QObject(parent), d(new Impl{}) {}

DiskProvider::~DiskProvider() { delete d; }

void DiskProvider::initialize() {
  d->previousIo = Impl::readDiskStats();
  qDebug() << "DiskProvider: initialized," << d->previousIo.size()
           << "block devices in /proc/diskstats";
}

void DiskProvider::poll() {
  const auto currentIo = Impl::readDiskStats();

  QVector<DiskStats> result;

  // Deduplicate: track which devices we have already added.
  // On Btrfs, /home /var/log etc. are subvolumes of the same partition —
  // they share the same device path and identical capacity numbers.
  // We keep the entry with the shortest (most canonical) mount point.
  std::unordered_set<std::string> seenDevices;

  // Sort volumes so that shorter mount points come first —
  // that way / beats /home when both map to the same device.
  auto volumes = QStorageInfo::mountedVolumes();
  std::sort(volumes.begin(), volumes.end(),
            [](const QStorageInfo &a, const QStorageInfo &b) {
              return a.rootPath().length() < b.rootPath().length();
            });

  for (const QStorageInfo &vol : volumes) {
    if (!vol.isValid() || !vol.isReady())
      continue;
    if (vol.bytesTotal() <= 0)
      continue;

    // Skip pseudo / kernel filesystems
    const QString fsType = vol.fileSystemType();
    const QStringList skip = {
        "tmpfs",   "devtmpfs",  "sysfs",  "proc",    "cgroup",     "cgroup2",
        "pstore",  "efivarfs",  "bpf",    "tracefs", "securityfs", "configfs",
        "debugfs", "hugetlbfs", "mqueue", "fusectl", "overlay"};
    if (skip.contains(fsType))
      continue;

    const std::string dev = Impl::devName(vol.device());

    // Skip if we already have an entry for this physical device
    if (seenDevices.count(dev))
      continue;
    seenDevices.insert(dev);

    DiskStats s;
    s.mountPoint = vol.rootPath();
    s.totalBytes = vol.bytesTotal();
    s.usedBytes = vol.bytesTotal() - vol.bytesFree();

    // I/O rates — match against the partition name in /proc/diskstats
    auto prevIt = d->previousIo.find(dev);
    auto currIt = currentIo.find(dev);

    if (prevIt != d->previousIo.end() && currIt != currentIo.end()) {
      s.readBytesPerSec =
          std::max(0LL, currIt->second.readBytes - prevIt->second.readBytes);
      s.writeBytesPerSec =
          std::max(0LL, currIt->second.writeBytes - prevIt->second.writeBytes);
    }

    result.append(s);
  }

  d->previousIo = currentIo;

  emit diskDataReady(result);
}