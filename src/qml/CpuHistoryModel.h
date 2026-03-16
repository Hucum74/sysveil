#pragma once

#include "HistoryModel.h"
#include <QVector>

struct CpuSample {
  qint64 timestamp{0}; // ms since epoch
  double overall{0.0};
  QVector<double> cores;
};

class CpuHistoryModel : public HistoryModel<CpuSample> {
  Q_OBJECT

public:
  enum Roles { TimestampRole = Qt::UserRole + 1, OverallRole, CoresRole };

  explicit CpuHistoryModel(int capacity = 60, QObject *parent = nullptr)
      : HistoryModel<CpuSample>(capacity, parent) {}

  QHash<int, QByteArray> roleNames() const override {
    return {{TimestampRole, "timestamp"},
            {OverallRole, "overall"},
            {CoresRole, "cores"}};
  }

  QVariant data(const QModelIndex &index, int role) const override {
    if (!index.isValid() || index.row() >= m_data.size())
      return {};

    const auto &s = m_data[index.row()];
    switch (role) {
    case TimestampRole:
      return s.timestamp;
    case OverallRole:
      return s.overall;
    case CoresRole: {
      QVariantList list;
      for (double v : s.cores)
        list.append(v);
      return list;
    }
    default:
      return {};
    }
  }
};