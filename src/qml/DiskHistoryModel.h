#pragma once

#include "HistoryModel.h"
#include <QString>

struct DiskSample {
  qint64 timestamp{0};
  QString mountPoint;
  qint64 usedBytes{0};
  qint64 totalBytes{0};
  qint64 readBytesPerSec{0};
  qint64 writeBytesPerSec{0};
};

class DiskHistoryModel : public HistoryModel<DiskSample> {
  Q_OBJECT

public:
  enum Roles {
    TimestampRole = Qt::UserRole + 1,
    MountPointRole,
    UsedBytesRole,
    TotalBytesRole,
    ReadBytesPerSecRole,
    WriteBytesPerSecRole,
    UsageRatioRole
  };

  explicit DiskHistoryModel(int capacity = 60, QObject *parent = nullptr)
      : HistoryModel<DiskSample>(capacity, parent) {}

  QHash<int, QByteArray> roleNames() const override {
    return {{TimestampRole, "timestamp"},
            {MountPointRole, "mountPoint"},
            {UsedBytesRole, "usedBytes"},
            {TotalBytesRole, "totalBytes"},
            {ReadBytesPerSecRole, "readBytesPerSec"},
            {WriteBytesPerSecRole, "writeBytesPerSec"},
            {UsageRatioRole, "usageRatio"}};
  }

  QVariant data(const QModelIndex &index, int role) const override {
    if (!index.isValid() || index.row() >= m_data.size())
      return {};

    const auto &s = m_data[index.row()];
    switch (role) {
    case TimestampRole:
      return s.timestamp;
    case MountPointRole:
      return s.mountPoint;
    case UsedBytesRole:
      return s.usedBytes;
    case TotalBytesRole:
      return s.totalBytes;
    case ReadBytesPerSecRole:
      return s.readBytesPerSec;
    case WriteBytesPerSecRole:
      return s.writeBytesPerSec;
    case UsageRatioRole:
      return s.totalBytes > 0 ? double(s.usedBytes) / double(s.totalBytes)
                              : 0.0;
    default:
      return {};
    }
  }
};