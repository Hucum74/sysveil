#pragma once

#include "HistoryModel.h"

struct MemorySample {
  qint64 timestamp{0};
  qint64 usedBytes{0};
  qint64 totalBytes{0};
  qint64 usedSwap{0};
  qint64 totalSwap{0};
};

class MemoryHistoryModel : public HistoryModel<MemorySample> {
  Q_OBJECT

public:
  enum Roles {
    TimestampRole = Qt::UserRole + 1,
    UsedBytesRole,
    TotalBytesRole,
    UsedSwapRole,
    TotalSwapRole,
    UsageRatioRole
  };

  explicit MemoryHistoryModel(int capacity = 60, QObject *parent = nullptr)
      : HistoryModel<MemorySample>(capacity, parent) {}

  QHash<int, QByteArray> roleNames() const override {
    return {{TimestampRole, "timestamp"},   {UsedBytesRole, "usedBytes"},
            {TotalBytesRole, "totalBytes"}, {UsedSwapRole, "usedSwap"},
            {TotalSwapRole, "totalSwap"},   {UsageRatioRole, "usageRatio"}};
  }

  QVariant data(const QModelIndex &index, int role) const override {
    if (!index.isValid() || index.row() >= m_data.size())
      return {};

    const auto &s = m_data[index.row()];
    switch (role) {
    case TimestampRole:
      return s.timestamp;
    case UsedBytesRole:
      return s.usedBytes;
    case TotalBytesRole:
      return s.totalBytes;
    case UsedSwapRole:
      return s.usedSwap;
    case TotalSwapRole:
      return s.totalSwap;
    case UsageRatioRole:
      return s.totalBytes > 0 ? double(s.usedBytes) / double(s.totalBytes)
                              : 0.0;
    default:
      return {};
    }
  }
};