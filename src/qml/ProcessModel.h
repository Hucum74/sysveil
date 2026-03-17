#pragma once

#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include <QVector>

#include "ProcessProvider.h"

class ProcessModel : public QAbstractListModel {
  Q_OBJECT

public:
  enum Roles {
    PidRole = Qt::UserRole + 1,
    NameRole,
    UserRole,
    CpuUsageRole,
    MemoryPercRole,
    MemoryMiBRole
  };

  explicit ProcessModel(QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

public slots:
  void update(QVector<ProcessInfo> processes);

private:
  QVector<ProcessInfo> m_data;
};

// ── Proxy model — handles sorting + filtering
// ─────────────────────────────────

class ProcessProxyModel : public QSortFilterProxyModel {
  Q_OBJECT
  Q_PROPERTY(QString filterText READ filterText WRITE setFilterText NOTIFY
                 filterTextChanged)

public:
  explicit ProcessProxyModel(QObject *parent = nullptr);

  QString filterText() const { return m_filterText; }
  void setFilterText(const QString &text);

  // Called from QML to sort by a column
  Q_INVOKABLE void sortByRole(int role, bool ascending = false);

signals:
  void filterTextChanged();

protected:
  bool filterAcceptsRow(int sourceRow,
                        const QModelIndex &sourceParent) const override;

private:
  QString m_filterText;
};