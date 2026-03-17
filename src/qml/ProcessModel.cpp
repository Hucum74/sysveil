#include "ProcessModel.h"

// ── ProcessModel
// ──────────────────────────────────────────────────────────────

ProcessModel::ProcessModel(QObject *parent) : QAbstractListModel(parent) {}

int ProcessModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid())
    return 0;
  return m_data.size();
}

QVariant ProcessModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() >= m_data.size())
    return {};

  const ProcessInfo &p = m_data[index.row()];
  switch (role) {
  case PidRole:
    return p.pid;
  case NameRole:
    return p.name;
  case UserRole:
    return p.user;
  case CpuUsageRole:
    return p.cpuUsage;
  case MemoryPercRole:
    return p.memoryPerc;
  case MemoryMiBRole:
    return p.memoryMiB;
  default:
    return {};
  }
}

QHash<int, QByteArray> ProcessModel::roleNames() const {
  return {{PidRole, "pid"},
          {NameRole, "name"},
          {UserRole, "user"},
          {CpuUsageRole, "cpuUsage"},
          {MemoryPercRole, "memoryPerc"},
          {MemoryMiBRole, "memoryMiB"}};
}

void ProcessModel::update(QVector<ProcessInfo> processes) {
  beginResetModel();
  m_data = std::move(processes);
  endResetModel();
}

// ── ProcessProxyModel
// ─────────────────────────────────────────────────────────

ProcessProxyModel::ProcessProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent) {
  setFilterCaseSensitivity(Qt::CaseInsensitive);
  // Default sort: CPU descending
  sort(0, Qt::DescendingOrder);
}

void ProcessProxyModel::setFilterText(const QString &text) {
  if (m_filterText == text)
    return;
  m_filterText = text;
  beginFilterChange();
  endFilterChange();
  emit filterTextChanged();
}

void ProcessProxyModel::sortByRole(int role, bool ascending) {
  setSortRole(role);
  sort(0, ascending ? Qt::AscendingOrder : Qt::DescendingOrder);
}

bool ProcessProxyModel::filterAcceptsRow(
    int sourceRow, const QModelIndex &sourceParent) const {
  if (m_filterText.isEmpty())
    return true;

  const QModelIndex idx = sourceModel()->index(sourceRow, 0, sourceParent);

  const QString name =
      sourceModel()->data(idx, ProcessModel::NameRole).toString();
  const QString user =
      sourceModel()->data(idx, ProcessModel::UserRole).toString();
  const QString pid =
      sourceModel()->data(idx, ProcessModel::PidRole).toString();

  return name.contains(m_filterText, Qt::CaseInsensitive) ||
         user.contains(m_filterText, Qt::CaseInsensitive) ||
         pid.contains(m_filterText, Qt::CaseInsensitive);
}