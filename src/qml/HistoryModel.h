#pragma once

#include <QAbstractListModel>
#include <QVector>

// HistoryModel<T>
// Rolling buffer of N samples. Subclasses define the data type T
// and implement roleNames() + data(). append() adds a sample and
// removes the oldest when the buffer is full — one dataChanged/
// beginRemoveRows/beginInsertRows cycle per poll, keeping QML charts
// smooth with no full resets.

template <typename T> class HistoryModel : public QAbstractListModel {
public:
  explicit HistoryModel(int capacity = 60, QObject *parent = nullptr)
      : QAbstractListModel(parent), m_capacity(capacity) {
    m_data.reserve(capacity);
  }

  int rowCount(const QModelIndex &parent = QModelIndex()) const override {
    if (parent.isValid())
      return 0;
    return m_data.size();
  }

  // Append a new sample, evicting the oldest if at capacity.
  void append(const T &sample) {
    if (m_data.size() >= m_capacity) {
      beginRemoveRows(QModelIndex(), 0, 0);
      m_data.removeFirst();
      endRemoveRows();
    }

    beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
    m_data.append(sample);
    endInsertRows();
  }

  void clear() {
    if (m_data.isEmpty())
      return;
    beginResetModel();
    m_data.clear();
    endResetModel();
  }

protected:
  QVector<T> m_data;
  int m_capacity;
};