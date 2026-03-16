#pragma once

#include "HistoryModel.h"
#include <QString>

struct NetworkSample {
  qint64 timestamp{0};
  QString interfaceName;
  qint64 rxBytesPerSec{0};
  qint64 txBytesPerSec{0};
};

class NetworkHistoryModel : public HistoryModel<NetworkSample> {
  Q_OBJECT

public:
  enum Roles {
    TimestampRole = Qt::UserRole + 1,
    InterfaceNameRole,
    RxBytesPerSecRole,
    TxBytesPerSecRole
  };

  explicit NetworkHistoryModel(int capacity = 60, QObject *parent = nullptr)
      : HistoryModel<NetworkSample>(capacity, parent) {}

  QHash<int, QByteArray> roleNames() const override {
    return {{TimestampRole, "timestamp"},
            {InterfaceNameRole, "interfaceName"},
            {RxBytesPerSecRole, "rxBytesPerSec"},
            {TxBytesPerSecRole, "txBytesPerSec"}};
  }

  QVariant data(const QModelIndex &index, int role) const override {
    if (!index.isValid() || index.row() >= m_data.size())
      return {};

    const auto &s = m_data[index.row()];
    switch (role) {
    case TimestampRole:
      return s.timestamp;
    case InterfaceNameRole:
      return s.interfaceName;
    case RxBytesPerSecRole:
      return s.rxBytesPerSec;
    case TxBytesPerSecRole:
      return s.txBytesPerSec;
    default:
      return {};
    }
  }
};